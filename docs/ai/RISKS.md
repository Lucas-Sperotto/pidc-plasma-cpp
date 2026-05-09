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

**Atualização (T-023, 2026-05-08):** resolvido por DEC-0019 aceita.
`Node::volume` foi removido, pois a montagem de K por quadratura de Gauss usa
células de integração explícitas e não pesos nodais ambíguos.

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

**Atualização (T-Poisson, 2026-05-08):** resolvido para a Fase D inicial.
`MLSConfig` centraliza o raio de suporte e `DEC-0023` fixa
`support_radius = 1.8*h_g` para o teste MMS. Fases futuras ainda devem evitar
valores ad hoc fora de configuração explícita.

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

**Atualização (T-Poisson, 2026-05-08):** resolvido para a Fase D inicial por
`GaussCell2D` e `gauss2x2_cells`, com teste específico de integração.

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

---

## R-014 — `NeighborSearchGrid` armazena ponteiro bruto para `NodeCloud`

Registrado por: Claude — 2026-05-08 (T-029)

`NeighborSearchGrid` guarda `const NodeCloud* cloud_` como ponteiro bruto.
Se a `NodeCloud` for destruída antes do grid (ex.: grid em escopo mais longo,
cloud recriada após refinamento), o comportamento é indefinido — sem qualquer
diagnóstico em tempo de execução.

No EFG assembler (T-Poisson), `EFGPoissonSolver`, `NeighborSearchGrid` e
`NodeCloud` coexistirão: o chamador deve garantir que a cloud sobrevive ao grid.

Mitigação:
Documentar explicitamente o contrato de lifetime em `NeighborSearchGrid.hpp`.
Em fase posterior (se a cloud puder mudar), considerar `std::shared_ptr<const NodeCloud>`
ou passar a cloud por referência em cada `query_radius` em vez de armazená-la.
Para Phase D (nuvem estática), o contrato por ponteiro é aceitável se documentado.

Arquivo afetado: `include/pidc/geometry/NeighborSearchGrid.hpp`.

---

## R-015 — `mls_evaluate` usa distância euclidiana simples (não periódica)

Registrado por: Claude — 2026-05-08 (T-029)

`mls_evaluate` calcula `dx = x.x - xi.x`, `dy = x.y - xi.y` e `dist = sqrt(dx²+dy²)`
diretamente. Em domínio periódico, um ponto de consulta perto de x=0 deveria
"ver" nós perto de x=L como vizinhos próximos — o que não acontece com distância
euclidiana simples.

Para Phase D (EFG Poisson com BC Dirichlet em [0,1]²): **não é bloqueante** —
nenhuma periodicidade é usada no solver.

Para Phase F (PIDC com partículas e domínio periódico): **bloqueante** — partículas
perto de uma fronteira usarão funções de forma incorretas se `mls_evaluate` não
usar `minimum_image`.

Mitigação:
Antes de Phase F, criar sobrecarga `mls_evaluate_periodic(Vec2, const NodeCloud&,
double, const PeriodicBoundary2D&)` que substitui `dx/dy` por
`boundary.minimum_image(x - xi)`. Ver DEC-0022 proposta.

Arquivos afetados: `include/pidc/mls/MLSShapeFunction.hpp` (futuro).

---

## R-016 — `NeighborSearchGrid::query_radius` não é periódico

Registrado por: Claude — 2026-05-08 (T-029)

`query_radius` não procura vizinhos além das fronteiras periódicas. Um ponto de
consulta em x=0.05 com suporte h=0.3 em domínio [0,1] não encontrará nós em
x=0.75–1.0, embora a distância mínima periódica seja < h.

Para Phase D (EFG Poisson Dirichlet): **não é bloqueante**.
Para Phase F (PIDC periódico): **bloqueante** — busca de vizinhança produzirá
conjunto incompleto de vizinhos para partículas perto de fronteiras.

Mitigação:
Antes de Phase F, criar `NeighborSearchGrid::query_radius_periodic(Vec2, double,
const PeriodicBoundary2D&)` que replica o ponto de consulta nas 8 imagens
periódicas vizinhas e une os resultados (removendo duplicatas). DEC-0022 proposta.

---

## R-017 — `EFGPoissonSolver::solve()` recria fatoração LDLT a cada chamada

Registrado por: Claude — 2026-05-09 (T-036)
Status: resolvido para Fase F Dirichlet mínima — 2026-05-09, Codex, T-045D

`solve()` instancia `Eigen::SimplicialLDLT` localmente e chama `.compute(stiffness_)`
em toda invocação. Para Phase D (MMS estático: uma montagem + um solve), o custo
é negligível. Para Phase F (ciclo PIDC: `K` constante por passo de tempo, apenas
`b` varia), re-fatorar K a cada passo desperdiça O(n^1.5) sem benefício.

Contexto: o R-006 original apontou o risco de K ser remontado em todo passo.
A interface atual já separa `assemble()` de `solve()` (R-006 parcialmente resolvido),
mas a fatoração de K não é cacheada — o que ainda bloqueia uso eficiente em Phase F.

Mitigação:
Antes de Phase F, adicionar `factorize()` à interface do solver (ou cache interno
lazy) para que a fatoração de K seja reutilizada entre passos de tempo. Manter
`assemble()` + `factorize()` separados de `solve(b)` no ciclo temporal do PIDC.

Arquivo afetado: `include/pidc/efg/EFGPoissonSolver.hpp`.

**Nota adicional:** `stiffness_matrix()` retorna `Eigen::MatrixXd` (cópia densa
O(n²)). Uso legítimo em testes de simetria; não deve ser chamado no ciclo temporal.
Se Phase F precisar inspecionar K, preferir iterador esparso direto.

**Atualização R-006 (2026-05-09):** assemble/solve já são métodos distintos desde
T-034/T-035. O problema residual é o cache da fatoração (descrito acima como R-017).

**Atualização T-045D (2026-05-09):** `EFGPoissonSolver` armazena
`Eigen::SimplicialLDLT` em `ldlt_`, computado em `assemble()` ou
`assemble_stiffness_only()`. `solve(rhs)` reutiliza a fatoração, valida
tamanho/finitude do RHS externo e soma o RHS de penalidade Dirichlet cacheado.
Coberto por `tests/test_efg_poisson_external_rhs.cpp`.

---

## R-018 — Acoplamento prematuro entre módulos `pic/` e `pidc/` ou `efg/`

Registrado por: Claude — 2026-05-09 (T-038C)

Se `include/pidc/pic/` importar headers de `mls/` ou `efg/`, o PIC 1D perde
sua função de **referência independente** para comparação com o PIDC. A deposição
CIC e o solver Poisson FD/FFT não precisam de MLS, EFG ou Eigen.

Mitigação:
Proibir `#include "pidc/mls/..."`, `#include "pidc/efg/..."` e `#include <Eigen/...>`
em qualquer header de `include/pidc/pic/`. A DEC-0028 formaliza isso para vetores.
Inspecionar includes antes de aceitar qualquer PR na Fase E.

---

## R-019 — Ambiguidade "nx nós" vs "nx células" em grade periódica

Registrado por: Claude — 2026-05-09 (T-038C)

Para grade **não-periódica** (FEM clássico), `n_nodes = n_cells + 1`. Para grade
**periódica** (PIC canônico), `n_nodes = n_cells = nx`. Colaboradores familiarizados
com FEM ou grade não-periódica podem inicializar `Grid1D` com `nx+1` nós por engano,
gerando um nó fantasma e off-by-one em deposição/interpolação.

`DEC-0027` registra a convenção correta, mas o risco persiste enquanto não houver
um comentário explícito no construtor de `Grid1D`.

Mitigação:
Adicionar ao construtor de `Grid1D` um comentário de uma linha que explicite:
`// periodic: nx nodes == nx cells; node nx is a periodic image of node 0`.

---

## R-020 — `Particle` 2D usado em PIC 1D: componente y ignorado

Registrado por: Claude — 2026-05-09 (T-038C)

`pidc::Particle` armazena `Vec2 position` e `Vec2 velocity`. O PIC 1D usa apenas
`particle.position.x` e `particle.velocity.x`. O componente `.y` é zero não
inicializado pelo código PIC e nunca verificado.

Isso não é um bug no estado atual (y nunca entra em nenhuma equação PIC 1D),
mas pode enganar leitores e causar inicialização silenciosa incorreta se
o PIC 1D for estendido para 2D sem revisar todas as inicializações.

Mitigação:
Documentar nos testes PIC 1D que `.position.y` e `.velocity.y` não são utilizados.
Se o PIC for estendido para 2D, criar `Particle2D` ou reutilizar `Particle` com
inicialização explícita de ambos os componentes.

---

## R-021 — Tentativa prematura de Langmuir antes de validar deposição e campo

Registrado por: Claude — 2026-05-09 (T-038C)

O critério de aceite da Fase E é a oscilação de Langmuir com frequência correta.
Isso requer: (1) deposição CIC conservativa, (2) solver Poisson 1D com teste MMS,
(3) interpolação de campo com teste manufaturado, (4) leap-frog com teste de
reversibilidade. Pular diretamente para Langmuir sem validar cada passo
individualmente vai mascarar erros de sinal, escala ou fase.

Mitigação:
Seguir a sequência canônica: T-039 (CIC) → T-040 (campo manufaturado) → T-041
(Poisson 1D) → T-042 (interpolação) → T-043 (leap-frog) → T-044 (Langmuir).
Não fundir etapas. Cada etapa deve ter seu próprio teste antes de avançar.

---

## R-022 — `advance_particle_leapfrog_2d` sem inicialização de meio-passo

Registrado por: Claude — 2026-05-09 (T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP)

`advance_particle_leapfrog_2d` em `PIDCLoop.hpp` executa:

```cpp
v += (q/m) * E * dt
x += v * dt
```

Essa é a fórmula correta do leapfrog *se* a velocidade já estiver em t = -dt/2
antes do primeiro passo. Porém não existe função `initialize_pidc_velocity_2d`
análoga ao `initialize_leapfrog_velocity_1d` da Fase E. O `test_pidc_loop`
começa com `v = {0, 0}`, interpretado como velocidade em t = 0, não t = -dt/2.
Resultado: o primeiro passo é Euler-Cromer (semi-implícito), não leapfrog, e os
passos subsequentes carregam o offset. Para 2 passos com campo pequeno (smoke)
isso é invisível; para simulações longas (Langmuir 2D, reprodução da tese),
degrada a conservação de energia que é a principal vantagem do leapfrog.

Comparação com a Fase E: `initialize_leapfrog_velocity_1d` existe e é chamada
explicitamente antes do loop. A analogia 2D está ausente.

Mitigação:
Criar `initialize_pidc_velocity_2d` que (1) deposita cargas, (2) resolve Poisson
para obter E₀, (3) avança velocidades por -dt/2 sem avançar posições. Ver DEC-0034.
Arquivos: `include/pidc/pidc/PIDCLoop.hpp`.

---

## R-023 — Contorno periódico ausente no avanço de partículas PIDC 2D

Registrado por: Claude — 2026-05-09 (T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP)

`advance_particle_leapfrog_2d` avança posição sem aplicar nenhum contorno.
Para domínio Dirichlet (caso atual do smoke test), se uma partícula sair do
domínio, `mls_evaluate` vai falhar com `n < m` no próximo passo (R-002).
Para domínio periódico futuro (Fase H, reprodução da tese), o contorno precisaria
chamar `PeriodicBoundary2D::wrap`.

O `test_pidc_loop.cpp` verifica `domain.contains(particle.position)` após cada
passo, mas o loop não aplica contorno: a verificação detecta a falha sem corrigi-la.

Mitigação:
Decidir via DEC se (a) o chamador é responsável por aplicar contorno após cada
passo — o que é a separação de responsabilidades mais limpa — ou (b)
`pidc_advance_one_step` recebe um domínio e aplica contorno internamente. Para
Dirichlet, a opção (a) com verificação de saída (`domain.contains`) é suficiente
e foi implementada no teste. Para periódico, a opção (a) exigiria que o chamador
faça o wrap explicitamente.

---

## R-024 — Carga depositada em nós de contorno não é zerada antes de Q/ε₀

Registrado por: Claude — 2026-05-09 (T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP)

`deposit_charge_from_cells` deposita `q_p * phi_i` em todos os nós, inclusive
nós de fronteira. Se uma partícula estiver próxima a um nó de fronteira, esse
nó receberá carga física Q_boundary ≠ 0. O RHS externo será então
`rhs[boundary] = Q_boundary / ε₀ ≠ 0`.

Em `EFGPoissonSolver::solve(rhs)`, o `dirichlet_rhs_` é somado ao RHS externo.
Para BC homogêneo, `dirichlet_rhs_[boundary] = 0`, então o RHS total em nós de
fronteira é `Q_boundary/ε₀`. O penalty na diagonal de K (`≈ 1e12 * max_diag`)
domina numericamente e força `u_boundary ≈ 0`, mas a carga física foi ignorada.

Em unidades normalizadas de plasma onde as cargas podem ser da mesma ordem
que os coeficientes de rigidez, essa "dominância" é menos garantida. Para BC
não-homogêneo (u = g_boundary ≠ 0), a superposição `Q_boundary/ε₀ + penalty*g`
tem um erro físico explícito.

Mitigação:
Antes de chamar `pidc_rhs_from_charge`, ou dentro dela, zerar as componentes do
vetor de carga correspondentes a nós de fronteira Dirichlet. Isso requer que o
solver ou o loop tenham conhecimento de quais nós são de fronteira — um acoplamento
adicional que deve ser tratado por DEC.

Para o smoke test atual com BC homogêneo e partículas sempre internas (longe da
fronteira) o risco é inativo. Torna-se ativo ao aproximar partículas das paredes.

---

## R-025 — `pidc_advance_one_step` com 7 parâmetros — assinatura frágil

Registrado por: Claude — 2026-05-09 (T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP)

```cpp
PIDCStepDiagnostics pidc_advance_one_step(
    std::vector<Particle>& particles,
    const std::vector<Species>& species_list,
    const NodeCloud& cloud,
    const EFGPoissonSolver& solver,
    const MLSConfig& mls_config,
    double epsilon0,
    double dt);
```

Qualquer expansão futura — adicionar domínio para contorno periódico (R-023),
flag de diagnóstico detalhado, ou parâmetro de amortecimento — vai ampliar esta
assinatura e quebrar todos os chamadores existentes (`test_pidc_loop.cpp`,
`apps/pidc_smoke_2d.cpp`, etc.).

Analogia: a Fase E resolveu o mesmo problema com `LangmuirConfig1D`, que encapsula
`nx`, `dt`, `n_steps`, `perturbation_amplitude`. O resultado é que `run_langmuir_1d`
recebe um único objeto.

Mitigação:
Introduzir `PIDCConfig` ou `PIDCSimulation` que encapsule `solver`, `cloud`,
`mls_config`, `epsilon0`, e opcionalmente `domain`. Essa refatoração é baixo risco
(puramente mecânica) mas deve ser feita antes de adicionar periodicidade, para
evitar proliferação de parâmetros.
