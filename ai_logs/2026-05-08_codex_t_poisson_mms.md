# Log — Codex — T-Poisson Poisson EFG MMS inicial

Data: 2026-05-08

## Resumo

Implementei a primeira validação Poisson por EFG: quadratura Gauss 2×2,
montagem densa de `K`, montagem de `b`, Dirichlet homogêneo por substituição
direta e teste MMS em nuvens 5×5 e 9×9.

## Arquivos alterados

- `include/pidc/mls/MLSConfig.hpp`
- `include/pidc/mls/MLSShapeFunction.hpp`
- `include/pidc/efg/GaussCell2D.hpp`
- `include/pidc/efg/EFGPoissonSolver.hpp`
- `tests/test_gauss_cell2d.cpp`
- `tests/test_efg_poisson_mms.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/ai/DECISIONS.md`
- `docs/ai/RISKS.md`
- `docs/validation/VALIDATION_PLAN.md`

## Comandos executados

```bash
cmake --build build -j
./build/pidc_test_mls_robustness
./build/pidc_test_gauss_cell2d
./build/pidc_test_efg_poisson_mms
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
cmake -S . -B build
cmake --build build -j
./build/pidc_smoke
./build/pidc_test_gauss_cell2d
./build/pidc_test_efg_poisson_mms
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## Testes e resultados

- Preflight após remover `Node::volume`: 12/12 testes passaram.
- `gauss_cell2d`: passou.
- `efg_poisson_mms`: passou.
- Erro L2 absoluto MMS:
  - 5×5: 0.00359684;
  - 9×9: 0.000827504.
- Suíte final: 14/14 testes passaram.

## Hipóteses assumidas

- O MMS inicial é não periódico, com Dirichlet homogêneo em `[0,1]^2`.
- Para nuvem regular $N \times N$, `support_radius = 1.8*h_g`.
- A versão inicial usa matriz densa para preservar legibilidade e validação.

## Pendências

- Auditar o solver contra a formulação fraca e o sinal da equação.
- Adicionar métricas L∞ do potencial e L2/L∞ do campo elétrico.
- Migrar para matriz esparsa apenas depois de preservar o teste MMS.

## Próximo passo sugerido

Gemini + Claude devem revisar `EFGPoissonSolver` e o teste MMS antes de avançar
para métricas de campo, deposição de carga ou interpolação PIDC.
