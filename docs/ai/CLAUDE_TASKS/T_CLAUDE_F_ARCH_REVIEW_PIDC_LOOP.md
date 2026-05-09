# T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP — Revisão arquitetural pós-smoke PIDC

**Para:** Claude  
**De:** Codex  
**Data:** 2026-05-09  
**Prioridade:** alta antes da comparação PIC-PIDC

## Contexto

Codex fechou o Marco 5 com um ciclo PIDC 2D pequeno, Dirichlet e não-periódico.
O objetivo foi validar o fluxo de dados, não resolver ainda a simulação periódica
da tese.

## Pontos de revisão

1. Verificar se `EFGPoissonSolver::solve(rhs)` está arquiteturalmente claro após
   a separação entre RHS externo e RHS de penalidade Dirichlet.
2. Revisar se `DiffuseCell`, `deposit_charge_from_cells`,
   `interpolate_field_pidc` e `pidc_advance_one_step` têm fronteiras limpas para
   evoluir para `PIDCLoop` completo.
3. Reavaliar R-015/R-016 e definir a ordem recomendada para periodicidade MLS,
   busca periódica e comparação PIC-PIDC.

## Arquivos a ler

- `include/pidc/efg/EFGPoissonSolver.hpp`
- `include/pidc/pidc/DiffuseCell.hpp`
- `include/pidc/pidc/ChargeDeposition.hpp`
- `include/pidc/pidc/PIDCFieldInterpolator.hpp`
- `include/pidc/pidc/PIDCLoop.hpp`
- `apps/pidc_smoke_2d.cpp`
- `tests/test_efg_poisson_external_rhs.cpp`
- `tests/test_pidc_loop.cpp`
- `docs/ai/DECISIONS.md` — DEC-0031 e DEC-0033

## Entregáveis

- Registrar conclusão em `ai_logs/`.
- Atualizar `docs/ai/AI_STATUS.md` e `docs/ai/RISKS.md` se necessário.
- Propor tarefas pequenas para periodicidade/compare PIC-PIDC.
