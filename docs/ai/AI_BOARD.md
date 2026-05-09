# AI_BOARD — Quadro de comunicação entre IAs

Atualizado em: 2026-05-09 (T-REVIEW-E)

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
| T-045C | Implementar app `pidc_smoke_2d` com ciclo PIDC mínimo em domínio Dirichlet (usa T-045A, T-045B) | Claude | proposta |
| T-045B | Implementar `interpolate_field_pidc` e teste com campo manufaturado | Codex | proposta |
| T-045A | Implementar DEC-0031 (cache da fatoração em `EFGPoissonSolver`) e resolver R-017 | Claude | proposta |
| T-045 | Iniciar Fase F — PIDC completo (desmembrada em T-045A/B/C) | Professor | concluída |

---

## Concluídas

| ID | Tarefa | Responsável | Data |
| --- | --- | --- | --- |
| T-GEMINI-F-READINESS | Auditar prontidão para Fase F; propor DEC-0031 e plano para T-045 | Gemini | 2026-05-09 |
| T-REVIEW-E | Revisar Fase E: diagnóstico convergência O(h²) + Langmuir ω_obs=1.005; app `pidc_phase_e_diagnostics`; Marco 4 fechado; spec Gemini pré-F | Claude | 2026-05-09 |
| T-044 | Implementar oscilação de Langmuir 1D (`Langmuir1D.hpp`) + `test_langmuir_1d`; ω_obs=1.005∈[0.8,1.2]; CTest 24/24 | Claude | 2026-05-09 |
| T-039B | Adicionar casos de referência CIC 1D: partícula central e conjunto determinístico com semente fixa; CTest 23/23 | Codex | 2026-05-09 |
| T-043B | Validar contorno periódico PIC 1D em movimento (`test_pic_periodic_motion_1d`); CTest 22/22 | Codex | 2026-05-09 |
| T-043 | Implementar leap-frog 1D isolado (`LeapFrog1D.hpp`) + `test_leapfrog_1d`; CTest 21/21 | Codex | 2026-05-09 |
| T-042 | Implementar interpolação CIC campo → partícula (`FieldInterpolation1D.hpp`) + `test_field_interpolation_1d`; CTest 20/20 | Codex | 2026-05-09 |
| T-041 | Implementar Poisson 1D periódico por FD/DFT manual (`PoissonSolver1D.hpp`) + `test_poisson_solver_1d`; CTest 19/19 | Codex | 2026-05-09 |
| T-040 | Implementar campo elétrico manufaturado em `Grid1D` (`ManufacturedField1D.hpp`) + `test_manufactured_field_1d`; CTest 18/18 | Codex | 2026-05-09 |
| T-AUDIT-E-MATH | Auditar matematicamente o plano da Fase E PIC 1D | Gemini | 2026-05-09 |
| T-PLAN-E | Criar `PHASE_E_PIC1D_TECH_PLAN.md`; corrigir numeração T-040–T-044 após plano divergente; CTest 17/17 | Codex | 2026-05-09 |
| T-039 | Plano PHASE_E_PIC1D_PLAN.md aprovado; `deposit_charge_cic_1d` + `test_cic_deposition_1d`; CTest 17/17 | Claude | 2026-05-09 |
| T-038C | Revisão arquitetural PIC 1D: DEC-0027 aceita, DEC-0028, R-018–R-021 registrados | Claude | 2026-05-09 |
| T-038B | Auditar matematicamente o contrato da grade PIC 1D e propor T-039 | Gemini | 2026-05-09 |
| T-038A | Criar `pic::Grid1D` periódico semiaberto e `test_pic_grid1d`; CTest 16/16 passando | Codex | 2026-05-09 |
| T-037 | Definir DEC-0026; implementar deposit_charge; test_charge_conservation 15/15 passando | Gemini + Claude | 2026-05-09 |
| T-036 | Auditar T-032/T-035; revisão arquitetural EFGPoissonSolver; fechamento Marco 3; R-017 registrado | Gemini + Claude | 2026-05-09 |
| T-035 | Implementar migração densa → esparsa em EFGPoissonSolver (DEC-0025) | Codex | 2026-05-08 |
| T-032 | Adicionar métricas L∞ do potencial e L2/L∞ do campo manufaturado para Poisson MMS | Codex | 2026-05-08 |
| T-034 | Aceitar DEC-0024; refatorar `impose_dirichlet` → penalidade; planejar DEC-0025 (T-033) | Claude | 2026-05-08 |
| T-033 | Planejar migração densa → esparsa: DEC-0025 registrada em DECISIONS.md | Claude | 2026-05-08 |
| T-031 | Auditar `EFGPoissonSolver` e propor DEC-0024 para BCs de Dirichlet | Gemini | 2026-05-08 |
| T-Poisson | Implementar assembler EFG Poisson (GaussCell2D + Gauss 2×2); teste MMS | Codex | 2026-05-08 |
| T-023 | Decidir e implementar destino de `Node::volume` (DEC-0019) | Gemini | 2026-05-08 |
| T-030 | Confirmar riscos R-014/015/016 e DEC-0022 (desbloqueia T-Poisson) | Gemini | 2026-05-08 |
| T-028 | Revisar e anotar `VALIDATION_PLAN.md` | Gemini | 2026-05-08 |
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
