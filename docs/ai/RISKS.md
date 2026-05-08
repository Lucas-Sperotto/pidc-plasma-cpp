# RISKS — Riscos registrados pelas IAs

## R-001 — Validação visual enganosa

Gráficos de potencial/campo podem parecer fisicamente plausíveis mesmo com erro de sinal, escala ou deposição.

Mitigação:
Exigir validações quantitativas antes da reprodução qualitativa.

## R-002 — Vizinhança insuficiente

A matriz MLS pode ficar singular se a célula difusa não tiver nós suficientes.

Mitigação:
Monitorar `n >= m`, número de condição e padrões patológicos.

## R-003 — Alteração silenciosa da física

Refatorações podem alterar sinal, unidade ou normalização.

Mitigação:
Toda alteração em fórmula deve atualizar `DECISIONS.md` e testes.

---

## R-004 — Acoplamento de `Domain2D` com `Particle` (concreto, existe agora)

`Domain2D::applyPeriodic(Particle&)` faz uma primitiva geométrica depender de uma entidade de física de partículas.
Se `Particle` se tornar polimórfico, ganhar suporte a `Species`, ou for substituído, `Domain2D` precisa mudar junto.
O método `wrapPeriodic(Vec2) -> Vec2` já existe e cobre a operação geométrica pura.

Mitigação:
Remover `applyPeriodic(Particle&)` de `Domain2D`. A chamada `particle.position = domain.wrapPeriodic(particle.position)` deve ficar no código do chamador (PIDC loop, PIC loop). Ver DEC-0008.

Arquivo afetado: `include/pidc/Domain2D.hpp`, `src/Domain2D.cpp`.

---

## R-005 — Ausência de contrato para `ShapeFunctionData`

O produto central do módulo MLS — os valores $\phi_i(\mathbf{x})$ e os gradientes $\nabla\phi_i(\mathbf{x})$ para um ponto dado — não tem struct definida.
O TODO já menciona "reaproveitar `ShapeFunctionData` entre deposição e interpolação", mas sem interface o Codex e o Gemini vão implementar representações incompatíveis: um pode usar `std::vector<double>`, outro `Eigen::VectorXd`, outro par de arrays separados.

Mitigação:
Definir `ShapeFunctionData` antes de implementar MLS. Ver DEC-0009.

---

## R-006 — K reconstruído em todo passo de tempo por falta de interface clara

Se `EFGPoissonSolver` não separar explicitamente a montagem de `K` do `solve(b)`, o chamador (PIDC loop) vai invocar uma função de "resolver" que internamente remonta `K` a cada passo — desperdiçando o fato de que `K` é constante enquanto a nuvem de nós não muda.
Para N nós, montar `K` é O(N²) em quadratura; resolver apenas `K u = b` com fatoração prévia é O(N²) para LU denso mas sem o custo da integração.

Mitigação:
Separar `assemble()` de `solve()` na interface do solver. Ver DEC-0010.

Arquivo afetado: interface futura de `EFGPoissonSolver`.

---

## R-007 — Charge e mass por instância de `Particle`

`Particle` armazena `charge` e `mass` como `double` por instância. Em uma simulação com 1.000 elétrons, todos têm $q = -e$ e $m = m_e$: armazenar por instância duplica 16 bytes por partícula sem ganho físico e abre risco de inconsistência (uma partícula com massa errada por bug de inicialização).

Mitigação:
Introduzir `Species` com as propriedades escalares e referenciar via índice ou ponteiro em `Particle`. Ver DEC-0011.

---

## R-008 — Posse do vetor de nós indefinida

`EFGPoissonSolver`, `MLSShapeFunction`, `PIDCChargeDepositor` e `PIDCFieldInterpolator` precisam do vetor de nós. Sem decisão sobre propriedade, cada módulo pode receber cópias diferentes ou ponteiros brutos com tempo de vida ambíguo.

Mitigação:
Definir `NodeCloud` como proprietário canônico do vetor, passado por referência constante a todos os consumidores. Ver DEC-0012.

---

## R-009 — Semântica de `Node::volume` indefinida

`Node` tem campo `volume` mas sua semântica não está documentada: é a área da célula de background para quadratura? O volume de Voronoi? A área do domínio de influência? Essa ambiguidade vai se propagar para a montagem da matriz `K` e para a deposição de carga.

Mitigação:
Documentar a semântica antes de usar `volume` em qualquer cálculo. Candidato natural: área da célula de background de integração (não o domínio de influência MLS). Registrar decisão antes da Fase C.

**Atualização (T-019, 2026-05-08):** R-009 é BLOQUEANTE para EFG. A montagem de K
por quadratura de Gauss não usa `Node::volume` — usa as células de integração
explicitamente. `volume` pode ser removido de `Node` ou documentado como
"peso nodal para integração direta nodal" (abordagem alternativa). Decisão
necessária antes de T-Poisson.

---

## R-010 — Raio de suporte sem propriedade definida

`mls_evaluate` recebe `support_radius` como `double` anônimo. No ciclo EFG,
o assembler, o loop PIDC e os testes usarão esse valor. Se cada chamador usar
um valor ligeiramente diferente — por exemplo, `1.5 * dx` calculado
independentemente —, $A(\mathbf{x})$ será inconsistente entre montagem de K
e avaliação nas partículas, gerando erro sutil impossível de rastrear.

Mitigação:
Centralizar `support_radius` em um `MLSConfig` struct ou constante de configuração
antes de implementar EFG. No mínimo, documentar que o mesmo valor deve ser usado
em todas as chamadas para uma dada nuvem. Ver proposta DEC-0017.

Arquivo afetado: `include/pidc/mls/MLSShapeFunction.hpp` (interface futura).

---

## R-011 — Busca de vizinhança O(N) em `mls_evaluate` acumulada em EFG

`mls_evaluate` percorre todos os N nós para encontrar vizinhos dentro do suporte.
Para a validação com N = 25 e ~100 pontos de Gauss: ~2 500 comparações. Aceitável.
Para simulação com N = 1 024 e ~4 000 pontos de Gauss: ~4 × 10⁶ comparações por
montagem de K. Ainda tolerável. Para N = 10 000+: passa a ser gargalo real.

Mitigação:
Aceitar a busca linear para todas as validações (Fase C, D). Registrar que
`NeighborSearchGrid` (fase B, ainda não implementada) é pré-requisito antes
de qualquer execução de simulação com N > 500. Não otimizar antes de validar.

---

## R-012 — Células de integração para EFG não definidas

A formulação de Galerkin (DEC-0006) exige integração numérica de
$K_{ij} = \int (\nabla\phi_i \cdot \nabla\phi_j)\,d\Omega$.
Sem células de integração definidas, o assembler EFG não pode ser implementado.
O campo `Node::volume` (R-009) não é suficiente: quadratura de Gauss de ordem 2
ou 3 requer posições e pesos explícitos dentro de cada célula.

Mitigação:
Decidir a estratégia de quadratura (DEC-0017) antes de implementar o assembler.
Candidato natural para grade regular: células retangulares com Gauss 2×2.
O assembler recebe as células como parâmetro (não as embutidas em `NodeCloud`).

---

## R-013 — Teste de MLS frágil (gradiente não verificado em ponto assimétrico)

Status: **falso positivo — fórmula verificada correta** (2026-05-08, Claude, T-021)

Contexto original (Gemini, T-020):
A auditoria apontou um possível termo extra `(∇p)ᵀ c wᵢ` na fórmula do gradiente.

Resolução (Claude, T-021):
A análise matemática detalhada mostra que a implementação está correta. A fórmula usa
diferenciação implícita de `A c = p(x)`:

```text
A ∂c/∂xₖ = ∂p/∂xₖ − ∂A/∂xₖ c
∂φᵢ/∂xₖ = (∂c/∂xₖ)ᵀ pᵢ wᵢ + cᵀ pᵢ ∂wᵢ/∂xₖ
```

O termo `(∂p/∂x)ᵀ A⁻¹ pᵢ wᵢ` identificado por Gemini como "extra" é na verdade o
primeiro componente de `(∂c/∂x)ᵀ pᵢ wᵢ`, já incluído corretamente via
`dc_dx = lu.solve(dpx − dA_dx * c)`. A confusão foi entre `A⁻¹ pᵢ` (vector
por vizinho `i`) e `c = A⁻¹ p(x)` (vector no ponto de consulta): eles são
distintos salvo quando o vizinho coincide com o ponto de consulta.

Verificação empírica: testes assimétricos adicionados para `{0.3, 0.7}`,
`{0.6, 0.4}` e `{0.37, 0.61}` — todos os 9 testes passam com tolerância 10⁻¹⁰.
A simetria não cancela o erro hipotético (c[1] ≠ 0 no caso geral), portanto
os testes assimétricos são discriminativos. **T-Poisson desbloqueada.**

Lição: o teste original era frágil por não verificar gradientes em pontos
assimétricos. Isso foi corrigido em T-021 — o teste agora cobre 4 pontos de
consulta com todas as 9 propriedades de gradiente.
