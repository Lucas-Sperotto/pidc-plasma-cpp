# Log — Codex — T-024 MLS robustness

Data: 2026-05-08

## Resumo

Implementei a validação explícita de robustez para `mls_evaluate`: raio de
suporte inválido agora falha cedo com `std::invalid_argument`, e o novo teste
`mls_robustness` verifica ausência de `NaN`/`Inf` em `phi` e `grad_phi`, além
do condicionamento da matriz de momentos MLS.

## Arquivos alterados

- `include/pidc/mls/MLSShapeFunction.hpp`
- `tests/test_mls_robustness.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`

## Comandos executados

```bash
/usr/bin/ctest --test-dir build --output-on-failure
cmake --build build -j
./build/pidc_test_mls_robustness
git diff --check
git commit -m "test: add MLS robustness checks"
cmake -S . -B build
cmake --build build -j
./build/pidc_smoke
./build/pidc_test_mls_robustness
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## Testes e resultados

- Preflight: 9/9 testes passavam antes da tarefa.
- `pidc_test_mls_robustness`: passou.
- Suíte final: 12/12 testes passaram.

## Hipóteses assumidas

- O limite de condicionamento `$cond(A) < 10^8$` vem de
  `docs/validation/VALIDATION_PLAN.md`.
- O teste de condicionamento permanece test-local; nenhuma nova API pública de
  diagnóstico MLS foi introduzida nesta tarefa.

## Pendências

- R-010 permanece: centralizar política de raio de suporte antes de usos mais
  extensos em EFG/PIDC.

## Próximo passo sugerido

Revisar o plano de validação (T-028) e decidir `Node::volume` (T-023) antes de
iniciar T-Poisson.
