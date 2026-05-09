# AI_STATUS — Estado técnico consolidado

Atualizado em: 2026-05-09

## Último resumo

Codex concluiu T-045D para fechamento operacional das Fases A-G. Adicionada a opção CMake `PIDC_ENABLE_JSON=OFF`; criados helpers `CircularInfluenceDomain` e `RectangularInfluenceDomain`; `EFGPoissonSolver::solve(rhs)` agora valida RHS externo e soma o RHS de penalidade Dirichlet cacheado. Fase F mínima implementada com `DiffuseCell`, `deposit_charge_from_cells`, `interpolate_field_pidc`, `pidc_advance_one_step` e app `pidc_smoke_2d`, que exporta `data/output/pidc_smoke_2d.csv` (3 passos, 4 partículas, carga total ≈ -5.97e-22, `max|E|=6.03e-07`, finito=1). Novos testes: `influence_domain`, `efg_poisson_external_rhs`, `pidc_diffuse_cell`, `pidc_field_interpolation`, `pidc_loop`. **29/29 CTest passando.** Tarefas de auditoria abertas para Gemini e Claude antes da comparação PIC-PIDC.

---

## Último resumo anterior

Claude concluiu os três itens diferidos do Marco 3 (T-MARCO3-DEFERRED). App diagnóstico `apps/marco3_mms_diagnostics.cpp` criado: convergência MMS EFG Poisson para n∈{5,7,9,13,17} — L²(φ) ordem 2.06 no par 13→17 (critério: >1.7 ✓), L²(E) ordem 1.02 (regime assintótico confirmado). CSV exportado em `data/output/efg_mms_convergence.csv`. Script `scripts/plot_efg_mms.py` gera dois gráficos em `docs/figures/`. Relatório de validação formal em `docs/validation/marco3_poisson_efg_validation.md` (10 critérios, todos ✓). ROADMAP.md: Marco 3 agora 100% completo. **24/24 CTest passando.**

---

## Último resumo

Gemini concluiu a auditoria de prontidão para a Fase F (T-GEMINI-F-READINESS). R-017 (ausência de cache de fatoração em `EFGPoissonSolver`) foi confirmado como um bloqueador de desempenho, e a `DEC-0031` foi proposta para corrigi-lo. Foi recomendado iniciar a Fase F com um domínio Dirichlet não-periódico para isolar a validação do ciclo PIDC da complexidade da periodicidade (R-015/R-016). A tarefa T-045 foi desmembrada em subtarefas: T-045A (refatorar `EFGPoissonSolver`), T-045B (implementar `interpolate_field_pidc`), e T-045C (criar app `pidc_smoke_2d` com o ciclo PIDC mínimo).
---

## Último resumo

Claude concluiu T-REVIEW-E. App diagnóstico `apps/phase_e_diagnostics.cpp` criado: (A) convergência Poisson 1D MMS — O(h²) confirmada para nx={16,32,64,128}, L∞(φ)=2.008e-04 e L∞(E)=1.262e-03 em nx=128; (B) interpolação CIC — erro decresce de 1.18e-01 para 1.89e-03 entre nx=16 e nx=128; (C) Langmuir ω_obs=1.005, deriva=2.51%, erro de carga=5.6e-16. CSVs exportados em `data/output/`. Script `scripts/plot_phase_e.py` gera figura de convergência log-log. Marco 4 fechado em ROADMAP.md. Tarefa para Gemini criada em `docs/ai/GEMINI_TASKS/T_GEMINI_F_READINESS.md`. **24/24 CTest passando.**

---

## Último resumo anterior

Claude concluiu T-044 (Langmuir 1D). `run_langmuir_1d` implementada em `include/pidc/pic/Langmuir1D.hpp` (header-only, sem Eigen). Integra a cadeia completa PIC 1D: deposição CIC → Poisson periódico (DFT) → interpolação CIC → leap-frog → diagnóstico. Unidades normalizadas: ε₀=1, q_mac=-L/N, m_mac=L/N, q/m=-1, ω_pe=1. Perturbação modo-1 de amplitude 1e-3; 2500 passos de dt=0.05. Frequência observada extraída por DFT O(n²/2) da série temporal de Re[Ê_k=1]. Resultado: ω_obs=1.005 ∈ [0.8, 1.2]·ω_pe. Deriva de energia ≤ 25%; erro de carga ≤ 1e-10. `test_langmuir_1d` cobre T1–T4; tempo de execução ≈ 0.41 s. **Fase E completa. 24/24 testes passando.**

---

## Último resumo anterior

Codex concluiu T-039B. `tests/test_cic_deposition_reference_1d.cpp` foi criado para registrar casos de referência de deposição CIC 1D: partícula no centro de célula com divisão 50/50, partícula no centro do domínio sobre nó, e conjunto determinístico gerado por semente fixa com conservação global e reprodutibilidade. Nenhum código de produção foi alterado nesta tarefa. **23/23 testes passando.**

---

## Último resumo anterior

Codex concluiu T-043B. `tests/test_pic_periodic_motion_1d.cpp` valida o contorno periódico 1D em movimento usando `Grid1D` e `LeapFrog1D`: cruzamentos pela direita/esquerda, imagem exata de `xmax`, permanência em `[xmin,xmax)`, conservação do número de partículas e posições analíticas embrulhadas em múltiplos passos com campo nulo. Nenhum Langmuir, PIDC, MLS ou EFG foi alterado nesta tarefa. **22/22 testes passando.**

---

## Último resumo anterior

Codex concluiu T-043. `initialize_leapfrog_velocity_1d` e `leapfrog_advance_1d` foram criados em `include/pidc/pic/LeapFrog1D.hpp`, seguindo a convenção de velocidade defasada de meio passo. `test_leapfrog_1d` valida movimento uniforme, aceleração constante, reversibilidade drift-only, wrap periódico e falhas para entradas não finitas. Nenhum Langmuir, PIDC, MLS ou EFG foi alterado nesta tarefa. **21/21 testes passando.**

---

## Último resumo anterior

Codex concluiu T-042. `interpolate_field_cic_1d` foi criado em `include/pidc/pic/FieldInterpolation1D.hpp`, interpolando campos nodais em `Grid1D` com pesos CIC periódicos `E_p = E_left*(1-f) + E_right*f`. `test_field_interpolation_1d` valida casos exatos, wrap periódico, sobrecarga vetorial para partículas, campo manufaturado senoidal e falhas de entrada. Nenhum leap-frog, Langmuir, MLS, EFG ou PIDC foi alterado nesta tarefa. **20/20 testes passando.**

---

## Último resumo anterior

Codex concluiu T-041. `solve_poisson_periodic_1d` foi criado em `include/pidc/pic/PoissonSolver1D.hpp`, resolvendo $\partial^2\phi/\partial x^2 = -\rho/\epsilon_0$ em `Grid1D` periódico via DFT manual e autovalores de diferenças finitas, com gauge de média zero. `test_poisson_solver_1d` valida MMS periódico contra `SineManufacturedField1D`, campo nodal, gauge, densidade zero e falhas de entrada. Nenhuma interpolação campo→partícula, leap-frog ou Langmuir foi implementado. **19/19 testes passando.**

---

## Último resumo anterior

Gemini concluiu T-040B: auditoria matemática da implementação do campo manufaturado 1D (T-040). A solução `phi(x) = A*sin(2πx/L)` e suas derivadas (`E`, `rho`) foram validadas como matematicamente corretas, periódicas e auto-consistentes. A densidade de carga `rho` tem média nula, satisfazendo a condição para o solver de Poisson periódico. A `DEC-0030` foi proposta para formalizar esta convenção. A implementação de T-040 está correta e bem-escopada. **A tarefa T-041 (PoissonSolver1D) está desbloqueada.**
---

## Último resumo

Gemini concluiu a auditoria matemática do plano da Fase E (T-AUDIT-E-MATH). O plano sequencial (T-039 a T-044) foi validado como matematicamente sólido. A convenção de grade (DEC-0027) e o método de deposição CIC (T-039) estão corretos. Foi proposta a DEC-0029 para formalizar a estratégia de "um teste por componente", garantindo que o solver de Poisson, a interpolação e o leap-frog sejam validados com testes de unidade (MMS, campo manufaturado, reversibilidade) antes da simulação integrada de Langmuir. Os riscos de comparação PIC vs. PIDC foram identificados (dimensionalidade, parâmetros, diagnósticos). A Fase E pode prosseguir conforme planejado.
---

## Último resumo

Claude concluiu T-039. Plano `PHASE_E_PIC1D_PLAN.md` aprovado e registrado em `docs/ai/`. `deposit_charge_cic_1d` implementada em `include/pidc/pic/ChargeDeposition1D.hpp` (header-only, sem Eigen, DEC-0028). Acumula `q*(1-f)` no nó esquerdo e `q*f` no nó direito (CIC). Teste `test_cic_deposition_1d` cobre 6 subtestes: 50/50 no centro, 100% sobre nó, periodicidade próxima a xmax, conservação global `|ΣQ_i − Σq_p| < 1e-12`, tamanho de retorno e species_id inválido → throw. **17/17 testes passando.** Próxima tarefa: T-040 (Codex) — `PoissonSolver1D` periódico.

---

## Último resumo anterior

Claude concluiu T-037 (com base no plano do Gemini e DEC-0026). `deposit_charge` implementada em `include/pidc/pidc/ChargeDeposition.hpp`: itera partículas, chama `mls_evaluate`, acumula `q_p * phi_i(x_p)`. Teste `test_charge_conservation` cobre: conservação global multi-partícula (5 subtestes), partícula única, lista vazia, espécie inválida e resolução 9×9. **15/15 testes passando.** A Fase E (PIC baseline) pode ser iniciada pelo Codex (T-038).

---

## Estado geral

Bootstrap mínimo criado e validado. O projeto compila com CMake/C++17, possui biblioteca `pidc_core`, executável smoke e teste simples de contorno periódico registrado no CTest.

## Módulos

| Módulo | Estado |
| --- | --- |
| core | bootstrap mínimo compilando |
| particles | `Vec2`, `Particle` (refatorada) e `Species` implementadas (DEC-0011) |
| geometry | `Domain2D` inicial com contorno periódico via `wrapPeriodic(Vec2)`; `NodeCloud`, `RegularNodeCloud2D`, `NeighborSearchGrid` e `PeriodicBoundary2D` criados |
| mls | `ShapeFunctionData` (contrato); `WeightFunction` quártica; `PolynomialBasis` linear 2D; `MLSShapeFunction` com `mls_evaluate` (PU + LR validados, guarda de raio de suporte e robustez finita testadas) |
| efg | `GaussCell2D` e `EFGPoissonSolver` esparso implementados; Poisson MMS Dirichlet passou em 5×5 e 9×9 com métricas de potencial e campo |
| pic | `Grid1D`, deposição CIC 1D, campo manufaturado 1D, Poisson 1D periódico, interpolação CIC campo→partícula, leap-frog 1D isolado, contorno periódico em movimento e Langmuir 1D implementados/testados |
| pidc | `deposit_charge`, `DiffuseCell`, deposição cacheada, interpolação de campo e loop mínimo Dirichlet implementados/testados |
| validation | CTest com 29 testes; inclui MLS, EFG Poisson MMS, RHS externo EFG, conservação de carga PIDC, smoke PIDC, PIC 1D e Langmuir |
| scripts | `scripts/build.sh` e `scripts/run_tests.sh` criados |

## Testes

| Teste | Estado |
| --- | --- |
| smoke build | passou em 2026-05-08 |
| periodic boundary | passou em 2026-05-08 |
| shape_function_data | passou em 2026-05-08 |
| node_cloud | passou em 2026-05-08 |
| species | passou em 2026-05-08 |
| weight_function | passou em 2026-05-08 |
| polynomial_basis | passou em 2026-05-08 |
| eigen_dependency | passou em 2026-05-08 |
| mls_shape_function | passou em 2026-05-08 |
| regular_node_cloud | passou em 2026-05-08 |
| mls_robustness | passou em 2026-05-08 |
| neighbor_search_grid | passou em 2026-05-08 |
| periodic_boundary2d | passou em 2026-05-08 |
| gauss_cell2d | passou em 2026-05-08 |
| efg_poisson_mms | passou em 2026-05-08 com backend esparso; potencial L2 5×5 = 0.00359683506821715, potencial L2 9×9 = 0.000827503569222836, potencial L∞ 5×5 = 0.00698519578344722, potencial L∞ 9×9 = 0.00169751665145062, campo L2 5×5 = 0.0330520262581074, campo L2 9×9 = 0.0136450944195131, campo L∞ 5×5 = 0.105393836778107, campo L∞ 9×9 = 0.0554928606259795 |
| partition unity | passou em 2026-05-08 (coberto por mls_shape_function) |
| linear reproduction | passou em 2026-05-08 (coberto por mls_shape_function) |
| charge conservation (PIDC) | passou em 2026-05-09 (test_charge_conservation, 15 subtestes) |
| cic_deposition_1d | passou em 2026-05-09 (test_cic_deposition_1d, 6 subtestes) |
| cic_deposition_reference_1d | passou em 2026-05-09 |
| manufactured_field_1d | passou em 2026-05-09 |
| poisson_solver_1d | passou em 2026-05-09 |
| field_interpolation_1d | passou em 2026-05-09 |
| leapfrog_1d | passou em 2026-05-09 |
| pic_periodic_motion_1d | passou em 2026-05-09 |
| Poisson MMS | passou em 2026-05-08 |
| Langmuir 1D | passou em 2026-05-09 (`test_langmuir_1d`, ω_obs=1.005) |

---

## Histórico

### 2026-05-09: T-039B (Codex — referências de deposição CIC 1D)
Codex criou `tests/test_cic_deposition_reference_1d.cpp`, registrado no CMake como `cic_deposition_reference_1d`. O teste cobre partícula no centro de uma célula (50/50), partícula no centro do domínio sobre nó, e um conjunto determinístico com semente fixa, comparando contra referência manual, conservação global e reprodutibilidade. CTest 23/23 passando. Não houve alteração de código de produção.

### 2026-05-09: T-043B (Codex — contorno periódico PIC 1D em movimento)
Codex criou `tests/test_pic_periodic_motion_1d.cpp`, registrado no CMake como `pic_periodic_motion_1d`. O teste usa `Grid1D` e `LeapFrog1D` com campo nulo para verificar cruzamentos pelas duas bordas, imagem exata de `xmax -> xmin`, posições sempre em `[xmin,xmax)`, conservação do número de partículas e posições analíticas embrulhadas em múltiplos passos. CTest 22/22 passando. Não houve implementação de Langmuir ou mudanças em PIDC/MLS/EFG.

### 2026-05-09: T-043 (Codex — leap-frog 1D isolado)
Codex criou `include/pidc/pic/LeapFrog1D.hpp` com `initialize_leapfrog_velocity_1d` e `leapfrog_advance_1d`. A inicialização converte `Particle::velocity.x` de `v(t=0)` para `v(t=-dt/2)`, e o avanço aplica `v_half += (q/m)E dt` seguido de `x = wrap(x + v_half dt)`. `tests/test_leapfrog_1d.cpp` cobre campo nulo, campo constante com solução analítica, reversibilidade drift-only com `dt` e `-dt`, wrap periódico e falhas de entrada. CTest 21/21 passando. Próximo passo sugerido: T-044, oscilação de Langmuir 1D.

### 2026-05-09: T-042 (Codex — interpolação CIC campo → partícula)
Codex criou `include/pidc/pic/FieldInterpolation1D.hpp` com duas sobrecargas de `interpolate_field_cic_1d`: uma para uma posição escalar e outra para `std::vector<pidc::Particle>`. A interpolação usa a mesma semântica periódica de `Grid1D` e os pesos CIC da deposição 1D, sem Eigen, MLS, EFG ou PIDC. `tests/test_field_interpolation_1d.cpp` cobre casos exatos em grid `[0,1)`, periodicidade, campo uniforme, campo manufaturado senoidal com tolerância `3e-3`, tamanho da sobrecarga vetorial e falhas para tamanho incorreto/valor não finito. CTest 20/20 passando. Próximo passo sugerido: T-043, leap-frog 1D isolado.

### 2026-05-09: T-041 (Codex — Poisson 1D periódico)
Codex criou `include/pidc/pic/PoissonSolver1D.hpp` com `PoissonResult1D` e `solve_poisson_periodic_1d`. O solver usa `std::vector<double>`, DFT manual com `std::complex<double>`, autovalores do operador de diferenças finitas periódico e gauge de potencial médio zero. O campo nodal é calculado por diferença central periódica. `tests/test_poisson_solver_1d.cpp` valida contra `SineManufacturedField1D`, verifica erro L∞ do potencial e do campo, média de potencial nula, densidade zero e falhas para tamanho incorreto/`epsilon0` inválido. CTest 19/19 passando. Não houve implementação de interpolação campo→partícula, leap-frog ou Langmuir.

### 2026-05-09: T-039 (Claude — plano Fase E + deposição CIC 1D)

Plano arquitetural da Fase E aprovado e gravado em `docs/ai/PHASE_E_PIC1D_PLAN.md`. `deposit_charge_cic_1d` implementada em `include/pidc/pic/ChargeDeposition1D.hpp` (header-only, sem Eigen). Algoritmo CIC: `Q[left] += q*(1-f); Q[right] += q*f` onde `f = fraction_in_cell`. Conservação pela partição da unidade. Teste `test_cic_deposition_1d` com 6 subtestes (50/50 centro, 100% nó, periodicidade, conservação, tamanho, throw). 17/17 testes passando.

### 2026-05-09: T-038C (Claude — revisão arquitetural PIC 1D)
Claude finalizou T-038C. Revisão arquitetural da Fase E (PIC 1D):
(1) `include/pidc/pic/` + namespace `pidc::pic` — correto.
(2) Reutilizar `Particle`/`Species` existentes no PIC 1D (usar apenas `.x`), sem duplicação prematura.
(3) Módulo PIC 1D deve ser **independente de `mls/`, `efg/` e Eigen** — R-018, DEC-0028.
(4) `PoissonSolver1D` será separado do `EFGPoissonSolver` — diferentes algoritmos/dimensões.
(5) Comparação PIC vs PIDC adiada para depois das validações individuais.
DEC-0027 aceita. DEC-0028 aceita. R-018–R-021 registrados. Sequência T-039→T-040→T-041→T-042→T-043.

### 2026-05-09: T-037 (Gemini + Claude — deposição conservativa de carga)
Gemini definiu DEC-0026 (interface `deposit_charge`). Claude implementou `include/pidc/pidc/ChargeDeposition.hpp`: função livre que itera partículas, chama `mls_evaluate` e acumula `q_p * phi_i(x_p)` nos índices nodais. Conservação garantida pela partição da unidade. Teste `test_charge_conservation` cobre: (1) conservação global multi-partícula 5×5; (2) partícula única; (3) lista vazia → vetor zero; (4) species_id inválido → `out_of_range`; (5) conservação em grade 9×9. 15/15 testes passando. DEC-0026 aceita. Fase E (PIC baseline, T-038) desbloqueada para Codex.

### 2026-05-09: T-036 (Claude — revisão arquitetural e fechamento Marco 3)
Claude finalizou T-036. Revisão arquitetural do `EFGPoissonSolver` após T-032/T-035:
(1) Implementação correta e alinhada com DEC-0024/DEC-0025; 14/14 testes passando.
(2) Acoplamento implícito em `impose_dirichlet_penalty` documentado — funciona corretamente mas depende de `stiffness_` populado antes da penalidade.
(3) `stiffness_matrix()` retorna cópia densa O(n²) — legítimo para testes, não deve ser chamado no ciclo temporal do PIDC.
(4) R-017 registrado: `solve()` recria `SimplicialLDLT` a cada chamada — não-bloqueante para Phase D, bloqueante para Phase F.
(5) R-006 atualizado: assemble/solve são interfaces separadas desde T-034; o resíduo é o cache da fatoração (R-017).
(6) Marco 3 do ROADMAP.md marcado como concluído; exportação CSV e script de plotagem diferidos como não-bloqueantes.
T-037 está em andamento: definir e implementar teste de deposição conservativa antes de Fase E.

### 2026-05-08: T-035 (Codex)
Codex concluiu T-035. `EFGPoissonSolver` agora armazena `K` como `Eigen::SparseMatrix<double>`, monta por `Eigen::Triplet<double>` e resolve com `Eigen::SimplicialLDLT`. A penalidade Dirichlet da DEC-0024 foi preservada como acréscimo diagonal, e `stiffness_matrix()` retorna uma cópia densa apenas para testes/diagnóstico. O teste MMS manteve as métricas de T-032 na escala de diferença esperada (< 1e-12 contra o baseline denso). Não houve implementação de PIC, PIDC, deposição, interpolação temporal ou caso grande.

### 2026-05-08: T-032 (Codex)
Codex concluiu T-032. O teste `efg_poisson_mms` agora mede erro
L2/L∞ do potencial e L2/L∞ do campo manufaturado, com campo avaliado como
`E = -grad(u_h)` a partir dos gradientes MLS. As métricas são finitas e refinam
de 5×5 para 9×9: potencial L2 0.00359684 → 0.000827504; potencial L∞
0.0069852 → 0.00169752; campo L2 0.033052 → 0.0136451; campo L∞
0.105394 → 0.0554929. Não houve implementação de PIC, PIDC, deposição ou caso grande.

---
### 2026-05-08: T-034, T-033 (Claude)
Claude concluiu T-034 e T-033. T-034: bug real identificado na eliminação sequencial de Dirichlet para caso não-homogêneo; `impose_dirichlet` substituído por `impose_dirichlet_penalty` (DEC-0024 aceita) — apenas diagonal de K é modificada, simetria preservada exatamente, homogêneo/não-homogêneo tratados identicamente. Tolerância do teste de contorno ajustada de `1e-12` para `1e-10`. 14/14 testes passando, L2 inalterado. T-033: DEC-0025 proposta para migração futura densa → esparsa via `SimplicialLDLT` (Codex, após Phase D validada).

---
### 2026-05-08: T-031 (Gemini)
Gemini concluiu T-031: Auditoria do `EFGPoissonSolver`. A implementação de Codex (T-Poisson) está matematicamente correta e consistente com as decisões `DEC-0005` (sinal) e `DEC-0006` (forma fraca). A imposição de Dirichlet por modificação da matriz é funcional, mas frágil. Para melhorar a robustez, foi proposta a `DEC-0024` para adotar o método de penalidade. Uma nova tarefa `T-034` foi criada para Claude revisar esta decisão e planejar a refatoração.

---
### 2026-05-08: T-Poisson (Codex)
Codex concluiu T-Poisson. Foram adicionados `MLSConfig`, `GaussCell2D` com quadratura Gauss 2×2, e `EFGPoissonSolver` denso inicial para o problema manufaturado Dirichlet. O teste `efg_poisson_mms` monta `K`, monta `b`, impõe Dirichlet homogêneo, resolve o sistema e valida erro L2: 5×5 = 0.00359684 (< 1e-2) e 9×9 = 0.000827504 (< 5×5). CTest passou com 14/14 testes. Não houve implementação de PIC, PIDC, deposição de carga, campo interpolado ou caso grande.

---

### 2026-05-08: T-023 (Gemini)
Gemini concluiu T-023 e Codex finalizou a remoção manual pendente: conforme `DEC-0019` (agora `aceita`), o campo `volume` foi removido da struct `Node`, e os testes/initializers foram ajustados. A justificativa é que a quadratura EFG (`DEC-0018`) usa células explícitas, tornando o campo redundante e uma fonte de ambiguidade (`R-009`).

---

### 2026-05-08: T-028, T-030 (Gemini)
Gemini concluiu T-028 e T-030. **Auditoria de validação e riscos concluída.**

1. `VALIDATION_PLAN.md` (T-028): As tolerâncias e o caso MMS estão corretos. Adicionada nota sobre a origem da tolerância de $10^{-10}$.
2. Riscos e Decisões (T-030): Confirmado que as implementações não-periódicas de MLS e busca de vizinhos (R-015, R-016) são corretas para a Fase D (Poisson Dirichlet). A estratégia para a Fase F periódica (DEC-0022) e o uso de ponteiro bruto em `NeighborSearchGrid` (R-014) para a Fase D são aceitáveis.

**Resultado:** A tarefa `T-Poisson` está totalmente desbloqueada e pronta para ser iniciada pelo Codex.

---
### 2026-05-08: T-029 (Claude)
Claude concluiu T-029: revisão dos contratos de `NeighborSearchGrid` e `PeriodicBoundary2D`. Implementações estão corretas para Phase D (não-periódica). Registrados R-014 (ponteiro bruto para NodeCloud), R-015 (mls_evaluate não periódico, bloqueante para Phase F) e R-016 (query_radius não periódico, bloqueante para Phase F). Proposta DEC-0022 (estratégia de sobrecargas periódicas para Phase F). T-Poisson permanece desbloqueada para Phase D. T-030 proposta para Gemini confirmar os riscos antes de T-Poisson.

---
### 2026-05-08: T-024, T-025, T-026 (Codex)
Codex concluiu T-024, T-025 e T-026. T-024 adicionou guarda explícita em `mls_evaluate` para raio de suporte positivo e finito, além do teste `mls_robustness` para ausência de NaN/Inf e condicionamento da matriz MLS. T-025 criou `NeighborSearchGrid` não periódico, com resultados ordenados e teste contra busca bruta. T-026 criou `PeriodicBoundary2D` com `wrap` e `minimum_image`, sem acoplar partículas, MLS ou PIDC. A Fase B geométrica mínima está completa para a próxima validação. CTest passou com 12/12 testes.

---
### 2026-05-08: Fases 0/A/B/C (Claude)
Claude concluiu auditoria das Fases 0/A/B/C. Resultados: (1) scripts/build.sh e run_tests.sh criados — Fase A completa; (2) DEC-0018 aceita — T-Poisson desbloqueada para Codex; (3) DEC-0019 proposta sobre semântica de Node::volume — aguarda decisão de Professor/Gemini (T-023); (4) docs/validation/VALIDATION_PLAN.md criado com tolerâncias quantitativas para Fases C e D; (5) Tarefas T-025 (NeighborSearchGrid), T-026 (PeriodicBoundary2D), T-028 (revisar VALIDATION_PLAN) e T-Poisson formalizadas no AI_BOARD. Fase B permanece incompleta (NeighborSearchGrid e PeriodicBoundary2D ausentes). 9/9 testes passando.

---

### 2026-05-08: T-021 (Claude)
Claude concluiu T-021: R-013 era falso positivo. A fórmula de gradiente em `mls_evaluate` foi verificada matematicamente correta via diferenciação implícita de `A c = p(x)`. O "termo extra" apontado por Gemini (`(∇p)ᵀ A⁻¹ pᵢ wᵢ`) faz parte do primeiro componente da derivada implícita; a confusão foi entre `A⁻¹ pᵢ` (vector por vizinho) e `c = A⁻¹ p(x)` (vector no ponto de consulta). Testes assimétricos adicionados para `{0.3, 0.7}`, `{0.6, 0.4}` e `{0.37, 0.61}` — 9/9 testes passam. **T-Poisson desbloqueada** (pendente DEC-0018 aceita e confirmação Gemini T-022).

---

### 2026-05-08: T-018 (Codex)
Codex revisou a T-018 após ela ter sido atribuída ao Gemini por engano. A implementação inicial de `RegularNodeCloud2D` usava caminhos e métodos inexistentes de `Domain2D`; foi corrigida para usar `pidc/Domain2D.hpp`, `lower()`, `width()` e `height()`, e o teste foi adaptado ao framework `tests/test_utils.hpp`. O duplicado de `R-013` em `docs/ai/RISKS.md` foi removido. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_test_regular_node_cloud`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 9/9 testes.

---

### 2026-05-08: T-020 (Gemini)
Gemini concluiu T-020: Auditoria matemática de `MLSShapeFunction`. **Foi encontrado um bug crítico na implementação do gradiente (T-017).** A fórmula implementada está incorreta, mas o teste passou devido a uma simetria no caso de teste que mascarou o erro. O risco foi registrado como R-013 e uma nova tarefa de correção, T-021, foi proposta para Claude. **A implementação do solver de Poisson (T-Poisson) está bloqueada até que T-021 seja concluída.**

---

### 2026-05-08: T-018 (Revisão)
T-018 foi integrada e revisada: `RegularNodeCloud2D` cria uma `NodeCloud` determinística a partir de `Domain2D`, incluindo domínios quadrados/retangulares, linha única, coluna única e erro controlado para dimensão zero. Com isso, a Fase B do `TODO.md` avança. O CTest agora executa 9/9 testes com sucesso.

---

### 2026-05-08: T-019 (Claude)
Claude concluiu T-019: revisão de arquitetura MLS → EFG. Registrados R-010 (raio de suporte sem propriedade), R-011 (busca O(N) acumulada em EFG) e R-012 (células de integração não definidas) em `docs/ai/RISKS.md`. Proposta DEC-0018 (quadratura por células retangulares Gauss 2×2) em `docs/ai/DECISIONS.md`. `CURRENT_CONTEXT.md` atualizado para refletir o estado real. `TODO.md` Fase C sincronizado. **Bloqueante:** DEC-0018 precisa ser aceita antes de implementar o assembler EFG Poisson.

---

### 2026-05-08: T-005 (Codex)
Codex concluiu T-005: framework de testes definido como CTest + executáveis C++ simples, com helper comum `tests/test_utils.hpp` para `require` e `approx_equal`. Todos os testes existentes foram migrados para o helper. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_smoke`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 8/8 testes.

---

### 2026-05-08: T-017 (Claude)
Claude concluiu T-017: `MLSShapeFunction` implementada em `include/pidc/mls/MLSShapeFunction.hpp` como função livre `mls_evaluate(Vec2, const NodeCloud&, double) -> ShapeFunctionData`. Monta a matriz de momento $A$ (3×3), resolve $A\mathbf{c} = \mathbf{p}(\mathbf{x})$ via `PartialPivLU`, e computa gradientes diferenciando implicitamente o sistema linear. Teste `test_mls_shape_function.cpp` cobre partição da unidade, reprodução linear, gradientes de PU e LR, segundo ponto de consulta e exceção com vizinhos insuficientes. 8/8 testes passando (DEC-0016).

---

### 2026-05-08: T-016 (Codex)
Codex concluiu T-016: Eigen foi adicionada ao CMake com `find_package(Eigen3 3.3 REQUIRED NO_MODULE)` e propagada por `pidc_core` via `Eigen3::Eigen`. O teste `eigen_dependency` foi registrado no CTest e passou. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_smoke`, `./build/pidc_test_eigen_dependency`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 7/7 testes.

---

### 2026-05-08: T-015 (Gemini)
Gemini concluiu T-015: Confirmação de que o repositório está sincronizado com as tarefas T-002, T-013 e T-014. Como próximo passo, foi proposta a tarefa T-016 para o Codex: adicionar a dependência da biblioteca Eigen via CMake, um pré-requisito para a Fase C (MLS/EFG).

---

### 2026-05-08: T-014 (Gemini)
Gemini concluiu T-014: Revisão e conclusão de tarefas pendentes. T-004 (convenção de unidades) foi formalmente concluída, e a entrada incorreta T-008 foi removida do quadro. Decisões implementadas (`DEC-0005`, `DEC-0008`, `DEC-0013`) foram atualizadas para o status `aceita`, sincronizando a documentação.

---

### 2026-05-08: T-013 (Gemini)
Gemini concluiu T-013: Sincronização do `TODO.md` com o `AI_BOARD.md`. As tarefas de implementação de `Species`, `NodeCloud`, `PolynomialBasis` e `WeightFunction` foram marcadas como concluídas, refletindo o trabalho já realizado por Claude e Codex.

---

### 2026-05-08: T-002 (Gemini)
Gemini concluiu T-002: Auditoria das equações em `docs/03_modelo_matematico_minimo.md`. O documento foi atualizado para refletir as decisões `DEC-0005` (unidades SI) e `DEC-0007` (estratégia de contorno), removendo ambiguidades.

---

### 2026-05-08: T-012 (Claude)
Claude concluiu T-012: `PolynomialBasis` linear 2D criada em
`include/pidc/mls/PolynomialBasis.hpp`. Funções `linear_basis(x)`,
`linear_basis_dx()`, `linear_basis_dy()`. 6/6 testes passando.
Todos os ingredientes sem Eigen para `MLSShapeFunction` estão prontos.

---
### 2026-05-08: T-010 (Claude)
Claude concluiu T-010 (resgate de T-008 / DEC-0011): `Species.hpp` criado em `include/pidc/`,
`Particle.hpp` refatorado para usar `species_id` (removidos `charge` e `mass`), `smoke.cpp`
atualizado, `test_species.cpp` criado usando padrão `require()`. 4/4 testes passando.
Nota: T-008 havia sido marcada como concluída por Gemini sem implementação real — violação de R-003.
