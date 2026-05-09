# Log de Tarefa: T-032 — Métricas de potencial e campo no Poisson MMS

**Agente:** Codex
**Data:** 2026-05-08

## Tarefa

Adicionar métricas L∞ do potencial e L2/L∞ do campo manufaturado ao teste
`efg_poisson_mms`, sem avançar para PIC, PIDC, deposição de carga ou caso grande.

## Arquivos alterados

- `tests/test_efg_poisson_mms.cpp`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`

## Comandos executados

```bash
git status --short
/usr/bin/ctest --test-dir build --output-on-failure
cmake --build build -j
./build/pidc_test_efg_poisson_mms
```

## Resultados

O teste `efg_poisson_mms` passou. Métricas registradas:

| Nuvem | Potencial L2 | Potencial L∞ | Campo L2 | Campo L∞ |
| --- | --- | --- | --- | --- |
| 5×5 | 0.00359684 | 0.0069852 | 0.033052 | 0.105394 |
| 9×9 | 0.000827504 | 0.00169752 | 0.0136451 | 0.0554929 |

Todas as métricas são finitas e melhoram ao refinar de 5×5 para 9×9.

## Hipóteses assumidas

- Campo elétrico manufaturado avaliado como `E = -grad(u)`.
- Erro L∞ do campo definido pela norma Euclidiana do erro vetorial em cada ponto
  de quadratura.

## Pendências

- Migrar `EFGPoissonSolver` para backend esparso conforme DEC-0025 (T-035).

## Próximo passo sugerido

Executar T-035 usando as métricas ampliadas como regressão do solver.
