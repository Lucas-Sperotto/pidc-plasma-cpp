# AI_BOARD — Quadro de comunicação entre IAs

Atualizado em: 2026-05-08

## Regras

- Adicionar novas tarefas no topo da seção correspondente.
- Não apagar registros antigos.
- Marcar responsável: `Codex`, `Gemini`, `Claude`, `Professor`, `ChatGPT`.
- Marcar status: `proposta`, `em andamento`, `bloqueada`, `concluída`, `revisar`.

---

## Em andamento

Nenhuma tarefa em andamento no momento.

---

## Próximas tarefas sugeridas

| ID | Tarefa | Responsável sugerido | Status |
| --- | --- | --- | --- |
| T-023 | Definir semântica de `Node::volume`; aceitar ou rejeitar DEC-0019 | Professor + Gemini | proposta |
| T-028 | Revisar `docs/validation/VALIDATION_PLAN.md` contra a tese; aceitar ou ajustar tolerâncias | Gemini | proposta |
| T-030 | Confirmar que R-014/R-015/R-016 e DEC-0022 são corretos antes de T-Poisson | Gemini | proposta |
| T-Poisson | Implementar assembler EFG Poisson (GaussCell2D + Gauss 2×2); teste MMS — DEC-0018 aceita | Codex | proposta |

---

## Concluídas

| ID | Tarefa | Responsável | Data |
| --- | --- | --- | --- |
| T-029 | Revisar contratos `NeighborSearchGrid` + `PeriodicBoundary2D`; registrar R-014/015/016, DEC-0022 | Claude | 2026-05-08 |
| T-026 | Implementar `PeriodicBoundary2D` com `wrap` e `minimum_image` (Fase B) | Codex | 2026-05-08 |
| T-025 | Implementar `NeighborSearchGrid` — grade 2D para busca de vizinhos (Fase B) | Codex | 2026-05-08 |
| T-024 | Implementar teste explícito de ausência de NaN/Inf e condicionamento em `mls_evaluate` | Codex | 2026-05-08 |
| T-022 | Auditar resolução de R-013 e confirmar que T-Poisson está desbloqueada | Gemini | 2026-05-08 |
| T-021 | Verificar R-013; confirmar fórmula correta; testes assimétricos {0.3,0.7}, {0.6,0.4}, {0.37,0.61} | Claude | 2026-05-08 |
| T-020 | Auditar matematicamente MLSShapeFunction e gradientes | Gemini | 2026-05-08 |
| T-018 | Implementar RegularNodeCloud2D com teste determinístico | Gemini + Codex revisão | 2026-05-08 |
| T-019 | Revisar arquitetura MLS → EFG; registrar R-010/011/012, propor DEC-0018 | Claude | 2026-05-08 |
| T-005 | Definir framework de testes | Codex | 2026-05-08 |
| T-017 | Implementar MLSShapeFunction::evaluate com PU e LR (DEC-0016, Fase C/MLS) | Claude | 2026-05-08 |
| T-016 | Adicionar dependência da biblioteca Eigen ao CMake | Codex | 2026-05-08 |
| T-015 | Confirmar sincronia e propor tarefa para adicionar Eigen | Gemini | 2026-05-08 |
| T-014 | Revisão e conclusão de tarefas pendentes (T-004, T-008) | Gemini | 2026-05-08 |
| T-013 | Sincronizar TODO.md com o estado real do projeto | Gemini | 2026-05-08 |
| T-002 | Auditar equações mínimas e sinais | Gemini | 2026-05-08 |
| T-012 | Criar PolynomialBasis linear 2D (DEC-0014, Fase C/MLS) | Claude | 2026-05-08 |
| T-011 | Criar WeightFunction spline quártica (DEC-0013, Fase C/MLS) | Claude | 2026-05-08 |
| T-010 | Resgatar T-008: Species.hpp, refatorar Particle, teste (DEC-0011) | Claude | 2026-05-08 |
| T-004 | Definir convenção de unidades (DEC-0005) | Gemini | 2026-05-08 |
| T-009 | Implementar DEC-0012: criar NodeCloud | Codex | 2026-05-08 |
| T-007 | Implementar DEC-0009: criar ShapeFunctionData.hpp | Claude | 2026-05-08 |
| T-006 | Implementar DEC-0008: remover applyPeriodic de Domain2D | Codex | 2026-05-08 |
| T-003 | Revisar arquitetura e riscos | Claude | 2026-05-08 |
| T-001 | Criar bootstrap CMake + smoke test | Codex | 2026-05-08 |
| P-000 | Planejamento inicial do repositório | ChatGPT | 2026-05-08 |

---

## Bloqueios

Nenhum bloqueio técnico no momento. Bloqueios esperados:

- convenção de unidades (T-004);
- sinal da equação de Poisson;
- estratégia para Poisson periódico;
- ~~escolha de função peso inicial~~ — resolvido: spline quártica (DEC-0013, hipótese).
