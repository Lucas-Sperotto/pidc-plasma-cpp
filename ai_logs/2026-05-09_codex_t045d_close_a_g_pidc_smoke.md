# Log — Codex — T-045D Fechamento A-G / PIDC smoke 2D

Data: 2026-05-09

## Tarefa

Implementar o plano de fechamento das fases A-G: infraestrutura JSON opcional,
domínios de influência, endurecimento do RHS externo do solver EFG, célula
difusa, deposição cacheada, interpolação PIDC, loop mínimo e smoke 2D com CSV.

## Arquivos alterados

- `CMakeLists.txt`
- `TODO.md`
- `ROADMAP.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/ai/DECISIONS.md`
- `docs/ai/OPEN_QUESTIONS.md`
- `docs/ai/RISKS.md`
- `include/pidc/efg/EFGPoissonSolver.hpp`
- `include/pidc/pidc/ChargeDeposition.hpp`
- `scripts/run_tests.sh`

## Arquivos criados

- `include/pidc/mls/InfluenceDomain.hpp`
- `include/pidc/pidc/DiffuseCell.hpp`
- `include/pidc/pidc/PIDCFieldInterpolator.hpp`
- `include/pidc/pidc/PIDCLoop.hpp`
- `apps/pidc_smoke_2d.cpp`
- `tests/test_influence_domain.cpp`
- `tests/test_efg_poisson_external_rhs.cpp`
- `tests/test_pidc_diffuse_cell.cpp`
- `tests/test_pidc_field_interpolation.cpp`
- `tests/test_pidc_loop.cpp`
- `docs/ai/GEMINI_TASKS/T_GEMINI_F_AUDIT_PIDC_RHS_DOMAINS.md`
- `docs/ai/CLAUDE_TASKS/T_CLAUDE_F_ARCH_REVIEW_PIDC_LOOP.md`
- `ai_logs/2026-05-09_codex_t045d_close_a_g_pidc_smoke.md`

## Implementação

- `PIDC_ENABLE_JSON=OFF` adicionado ao CMake; `nlohmann_json` só é exigido se a opção for ligada.
- `CircularInfluenceDomain` e `RectangularInfluenceDomain` adicionados como helpers, sem alterar a formulação MLS validada.
- `EFGPoissonSolver::solve(rhs)` agora valida tamanho/finitude e soma o RHS de penalidade Dirichlet cacheado ao RHS externo.
- `DiffuseCell` guarda `particle_index` + `ShapeFunctionData`.
- `deposit_charge_from_cells` deposita usando células difusas pré-computadas.
- `interpolate_field_pidc` calcula `E = -Σ u_i grad_phi_i`.
- `pidc_advance_one_step` executa deposição → RHS `Q/epsilon0` → solve cacheado → interpolação → avanço leap-frog 2D.
- `pidc_smoke_2d` roda 3 passos Dirichlet em `[0,1]^2` e exporta CSV.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
./build/pidc_smoke_2d
./scripts/run_tests.sh
```

## Resultados

- CTest: **29/29 testes passando**.
- CSV: `data/output/pidc_smoke_2d.csv`.
- Smoke PIDC 2D:
  - 4 partículas;
  - 3 passos;
  - carga total ≈ `-5.97224e-22`;
  - `max|E| = 6.03262e-07`;
  - finitude = 1.

## Hipóteses assumidas

- O primeiro PIDC 2D usa Dirichlet não-periódico para evitar R-015/R-016.
- `rhs = Q/epsilon0` é aceito apenas como contrato de smoke; Gemini deve auditar
  se casos físicos precisam de normalização por área/volume nodal.
- Domínios retangulares são helpers, não substitutos da distância circular em
  `mls_evaluate`.

## Pendências

- Gemini: auditar RHS PIDC, domínios e hipóteses tese ↔ código.
- Claude: revisar arquitetura do loop, solver cacheado e riscos de periodicidade.
- Implementar MLS/busca periódica antes de simulações PIDC periódicas ou comparação PIC-PIDC.
