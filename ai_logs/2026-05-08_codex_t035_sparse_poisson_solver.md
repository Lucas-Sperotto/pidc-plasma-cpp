# Log de Tarefa: T-035 — Migração esparsa do EFGPoissonSolver

**Agente:** Codex
**Data:** 2026-05-08

## Tarefa

Migrar o backend de `K` em `EFGPoissonSolver` de matriz densa para matriz
esparsa, conforme DEC-0025, preservando o teste MMS e as métricas ampliadas de
T-032.

## Arquivos alterados

- `include/pidc/efg/EFGPoissonSolver.hpp`
- `tests/test_efg_poisson_mms.cpp`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/ai/DECISIONS.md`
- `docs/validation/VALIDATION_PLAN.md`

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
./build/pidc_smoke
./build/pidc_test_efg_poisson_mms
/usr/bin/ctest --test-dir build --output-on-failure
PATH=/usr/bin:$PATH bash scripts/run_tests.sh
git diff --check
```

## Resultados

`EFGPoissonSolver` agora monta `K` por `Eigen::Triplet<double>`, armazena
`Eigen::SparseMatrix<double>` e resolve com `Eigen::SimplicialLDLT`. A penalidade
Dirichlet continua sendo acréscimo diagonal, compatível com o backend esparso.

Métricas MMS com backend esparso:

| Nuvem | Potencial L2 | Potencial L∞ | Campo L2 | Campo L∞ |
| --- | --- | --- | --- | --- |
| 5×5 | 0.00359683506821715 | 0.00698519578344722 | 0.0330520262581074 | 0.105393836778107 |
| 9×9 | 0.000827503569222836 | 0.00169751665145062 | 0.0136450944195131 | 0.0554928606259795 |

As métricas permanecem finitas e refinam de 5×5 para 9×9. A comparação com os
valores densos de T-032 ficou abaixo de `1e-12`.

## Hipóteses assumidas

- A matriz penalizada por DEC-0024 é adequada para `Eigen::SimplicialLDLT` nesta
  validação Dirichlet MMS.
- A cópia densa retornada por `stiffness_matrix()` é uma interface de diagnóstico,
  não o armazenamento interno do solver.

## Pendências

- Auditoria independente do solver esparso e das métricas MMS por Gemini/Claude.
- Definição do primeiro teste conservativo de deposição de carga antes de iniciar
  o ciclo PIDC.

## Próximo passo sugerido

Executar T-036 antes de qualquer implementação de deposição ou interpolação PIDC.
