# Log — 2026-05-08 — Claude — T-029: Revisão de contratos NeighborSearchGrid + PeriodicBoundary2D

## Objetivo

Auditar os contratos das classes `NeighborSearchGrid` (T-025) e
`PeriodicBoundary2D` (T-026) antes de acoplá-las a MLS e PIDC. Identificar
riscos, propor decisões e confirmar o que está pronto para T-Poisson.

## Arquivos lidos

| Arquivo | Motivo |
| --- | --- |
| `include/pidc/geometry/NeighborSearchGrid.hpp` | contrato principal |
| `include/pidc/geometry/PeriodicBoundary2D.hpp` | contrato principal |
| `tests/test_neighbor_search_grid.cpp` | cobertura de testes |
| `tests/test_periodic_boundary2d.cpp` | cobertura de testes |
| `include/pidc/mls/MLSShapeFunction.hpp` | acoplamento futuro |
| `include/pidc/geometry/NodeCloud.hpp` | proprietário canônico dos nós |

## Análise: NeighborSearchGrid

### Interface real (Codex)

```cpp
NeighborSearchGrid(const NodeCloud& cloud, const Domain2D& domain, double cell_size)
std::vector<std::size_t> query_radius(Vec2 point, double radius) const
```

### O que está correto

- Grade de células `nx_ × ny_` calculada a partir de `domain.width()/cell_size`.
- Construção rejeita `cell_size ≤ 0`, `cell_size` infinito e nós fora do domínio.
- `query_radius` rejeita raio negativo e NaN.
- Resultado ordenado por ID — determinístico.
- Comparado contra busca bruta em 6 pontos × 4 raios: idênticos.
- Casos de borda testados: ponto fora do domínio, cantos, nó externo na construção.
- Tolerância `+1.0e-14` no teste de distância² é consistente com `brute_force_query`.

### Riscos identificados

**R-014** — `cloud_` é ponteiro bruto `const NodeCloud*`. Sem contrato de
lifetime documentado, destruição precoce da cloud causa UB silencioso.
**Fase D: aceitável** se cloud e grid criados no mesmo escopo do assembler.
Deve ser documentado no header.

**R-016** — `query_radius` é não periódico. Ponto em x=0.05 com h=0.3 não
enxerga nós em x>0.75 mesmo que a distância periódica seja < h.
**Fase D (Dirichlet): não bloqueante.** Fase F (PIDC periódico): bloqueante.

### Assinatura inesperada vs. spec

A spec em T-025 dizia `query(Vec2, double)`. Codex implementou `query_radius`.
Nome mais descritivo — aceito. DEC-0020 não precisa ser ajustada.

---

## Análise: PeriodicBoundary2D

### Interface real (Codex)

```cpp
PeriodicBoundary2D(Domain2D domain)
Vec2 wrap(Vec2 point) const noexcept
Vec2 minimum_image(Vec2 displacement) const noexcept
```

### O que está correto

- `wrap` delega para `domain_.wrapPeriodic(point)` — reutiliza código validado.
- `minimum_image` usa `fmod` com correção de sinal:
  - `fmod(x, L) ∈ (-L, L)` sempre;
  - uma correção `±L` traz ao intervalo `(-L/2, L/2]`;
  - empate exato em `L/2` preservado (sem correção aplicada).
- Testes cobrem múltiplas voltas, deslocamentos negativos, empate em meia-caixa,
  domínio retangular não quadrado `[0,1]×[0,2]`.
- Semântica correta: `minimum_image` opera sobre **deslocamentos** (`x - xi`),
  não posições.

### Sem riscos críticos para Phase D

A implementação está matematicamente correta. Nenhuma alteração necessária
antes de T-Poisson.

### Risco para Phase F

**R-015** — `mls_evaluate` usa `dx = x.x - xi.x` diretamente, sem
`minimum_image`. Para PIDC periódico, `mls_evaluate_periodic` deve usar
`auto d = boundary.minimum_image(x - xi); dx = d.x; dy = d.y;`.
**Fase D: não bloqueante.**

---

## Veredicto para T-Poisson

| Módulo | Uso em Phase D | Problemas |
| --- | --- | --- |
| `NeighborSearchGrid::query_radius` | sim, não periódico | R-014 (doc), R-016 (Phase F) |
| `PeriodicBoundary2D` | não usado em Phase D | nenhum |
| `mls_evaluate` | sim, não periódico | R-015 (Phase F) |

**T-Poisson está desbloqueada.** O assembler EFG Poisson pode usar
`NeighborSearchGrid::query_radius` e `mls_evaluate` diretamente — o domínio
de validação é [0,1]² com BC Dirichlet, sem periodicidade.

---

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `docs/ai/RISKS.md` | R-014, R-015, R-016 adicionados |
| `docs/ai/DECISIONS.md` | DEC-0022 proposta |
| `docs/ai/AI_BOARD.md` | T-029 concluída; T-030 proposta para Gemini |
| `docs/ai/AI_STATUS.md` | resumo atualizado |

## Próximo passo sugerido

**T-030 (Gemini)** — confirmar que R-014/015/016 e DEC-0022 estão corretos
matematicamente antes de Codex iniciar T-Poisson.

**T-Poisson (Codex)** — pode iniciar após T-030 ou em paralelo se Gemini
confirmar rapidamente.
