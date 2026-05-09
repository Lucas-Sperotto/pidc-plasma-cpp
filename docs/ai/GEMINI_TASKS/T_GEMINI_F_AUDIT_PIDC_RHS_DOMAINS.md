# T-GEMINI-F-AUDIT-PIDC-RHS-DOMAINS — Auditoria matemática pós-smoke PIDC

**Para:** Gemini  
**De:** Codex  
**Data:** 2026-05-09  
**Prioridade:** alta antes da comparação PIC-PIDC

## Contexto

Codex implementou o primeiro smoke PIDC 2D em domínio Dirichlet não-periódico:

- `DiffuseCell` e reuso de `ShapeFunctionData`;
- `deposit_charge_from_cells`;
- `interpolate_field_pidc`, com `E(x_p) = -Σ_i u_i grad_phi_i(x_p)`;
- `pidc_advance_one_step`, com RHS `Q/epsilon0`;
- `pidc_smoke_2d`, exportando `data/output/pidc_smoke_2d.csv`.

## Perguntas

1. Confirmar se, no nível discreto adotado, usar `rhs = Q/epsilon0` é aceitável
   para o smoke PIDC ou se `Q` deve ser convertido para densidade por área/volume
   nodal antes do solver EFG.
2. Auditar se `CircularInfluenceDomain` e `RectangularInfluenceDomain` devem ser
   tratados como helpers geométricos ou se alguma forma retangular deve entrar no
   `mls_evaluate` validado.
3. Separar explicitamente o que está ancorado na tese de Marques do que permanece
   como hipótese de implementação incremental.

## Arquivos a ler

- `include/pidc/pidc/DiffuseCell.hpp`
- `include/pidc/pidc/ChargeDeposition.hpp`
- `include/pidc/pidc/PIDCFieldInterpolator.hpp`
- `include/pidc/pidc/PIDCLoop.hpp`
- `include/pidc/mls/InfluenceDomain.hpp`
- `tests/test_pidc_loop.cpp`
- `apps/pidc_smoke_2d.cpp`
- `docs/ai/DECISIONS.md` — DEC-0032 e DEC-0033

## Entregáveis

- Registrar conclusão em `ai_logs/`.
- Atualizar `docs/ai/AI_STATUS.md`.
- Se houver ajuste matemático necessário, propor DEC antes de pedir mudança de código.
