# AI_STATUS — Estado técnico consolidado

Atualizado em: 2026-05-08

## Estado geral

Bootstrap mínimo criado e validado. O projeto compila com CMake/C++17, possui biblioteca `pidc_core`, executável smoke e teste simples de contorno periódico registrado no CTest.

## Módulos

| Módulo | Estado |
| --- | --- |
| core | bootstrap mínimo compilando |
| particles | `Vec2`, `Particle` (refatorada) e `Species` implementadas (DEC-0011) |
| geometry | `Domain2D` inicial com contorno periódico via `wrapPeriodic(Vec2)`; `NodeCloud`, `RegularNodeCloud2D`, `NeighborSearchGrid` e `PeriodicBoundary2D` criados |
| mls | `ShapeFunctionData` (contrato); `WeightFunction` quártica; `PolynomialBasis` linear 2D; `MLSShapeFunction` com `mls_evaluate` (PU + LR validados, guarda de raio de suporte e robustez finita testadas) |
| efg | `GaussCell2D` e `EFGPoissonSolver` denso inicial implementados; Poisson MMS Dirichlet passou em 5×5 e 9×9 com métricas de potencial e campo |
| pic | não iniciado |
| pidc | não iniciado |
| validation | CTest com 14 testes; gradiente MLS verificado em 4 pontos incluindo 3 assimétricos; R-013 fechado como falso positivo; `VALIDATION_PLAN.md` criado; robustez MLS, busca de vizinhança, fronteira periódica, quadratura e Poisson MMS com campo testados |
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
| efg_poisson_mms | passou em 2026-05-08; potencial L2 5×5 = 0.00359684, potencial L2 9×9 = 0.000827504, potencial L∞ 5×5 = 0.0069852, potencial L∞ 9×9 = 0.00169752, campo L2 5×5 = 0.033052, campo L2 9×9 = 0.0136451, campo L∞ 5×5 = 0.105394, campo L∞ 9×9 = 0.0554929 |
| partition unity | passou em 2026-05-08 (coberto por mls_shape_function) |
| linear reproduction | passou em 2026-05-08 (coberto por mls_shape_function) |
| charge conservation | não iniciado |
| Poisson MMS | passou em 2026-05-08 |
| Langmuir 1D | não iniciado |

## Último resumo

Codex concluiu T-032 (2026-05-08). O teste `efg_poisson_mms` agora mede erro
L2/L∞ do potencial e L2/L∞ do campo manufaturado, com campo avaliado como
`E = -grad(u_h)` a partir dos gradientes MLS. As métricas são finitas e refinam
de 5×5 para 9×9: potencial L2 0.00359684 → 0.000827504; potencial L∞
0.0069852 → 0.00169752; campo L2 0.033052 → 0.0136451; campo L∞
0.105394 → 0.0554929. Não houve implementação de PIC, PIDC, deposição ou caso grande.

---

### Histórico anterior

Claude concluiu T-034 e T-033 (2026-05-08). T-034: bug real identificado na eliminação sequencial de Dirichlet para caso não-homogêneo; `impose_dirichlet` substituído por `impose_dirichlet_penalty` (DEC-0024 aceita) — apenas diagonal de K é modificada, simetria preservada exatamente, homogêneo/não-homogêneo tratados identicamente. Tolerância do teste de contorno ajustada de `1e-12` para `1e-10`. 14/14 testes passando, L2 inalterado. T-033: DEC-0025 proposta para migração futura densa → esparsa via `SimplicialLDLT` (Codex, após Phase D validada).

---

### Histórico anterior

Gemini concluiu T-031: Auditoria do `EFGPoissonSolver`. A implementação de Codex (T-Poisson) está matematicamente correta e consistente com as decisões `DEC-0005` (sinal) e `DEC-0006` (forma fraca). A imposição de Dirichlet por modificação da matriz é funcional, mas frágil. Para melhorar a robustez, foi proposta a `DEC-0024` para adotar o método de penalidade. Uma nova tarefa `T-034` foi criada para Claude revisar esta decisão e planejar a refatoração.

---

### Histórico anterior

Codex concluiu T-Poisson (2026-05-08). Foram adicionados `MLSConfig`, `GaussCell2D` com quadratura Gauss 2×2, e `EFGPoissonSolver` denso inicial para o problema manufaturado Dirichlet. O teste `efg_poisson_mms` monta `K`, monta `b`, impõe Dirichlet homogêneo, resolve o sistema e valida erro L2: 5×5 = 0.00359684 (< 1e-2) e 9×9 = 0.000827504 (< 5×5). CTest passou com 14/14 testes. Não houve implementação de PIC, PIDC, deposição de carga, campo interpolado ou caso grande.

---

### Histórico anterior

Gemini concluiu T-023 e Codex finalizou a remoção manual pendente: conforme `DEC-0019` (agora `aceita`), o campo `volume` foi removido da struct `Node`, e os testes/initializers foram ajustados. A justificativa é que a quadratura EFG (`DEC-0018`) usa células explícitas, tornando o campo redundante e uma fonte de ambiguidade (`R-009`).

---

### Histórico anterior

Gemini concluiu T-028 e T-030 (2026-05-08). **Auditoria de validação e riscos concluída.**

1. `VALIDATION_PLAN.md` (T-028): As tolerâncias e o caso MMS estão corretos. Adicionada nota sobre a origem da tolerância de $10^{-10}$.
2. Riscos e Decisões (T-030): Confirmado que as implementações não-periódicas de MLS e busca de vizinhos (R-015, R-016) são corretas para a Fase D (Poisson Dirichlet). A estratégia para a Fase F periódica (DEC-0022) e o uso de ponteiro bruto em `NeighborSearchGrid` (R-014) para a Fase D são aceitáveis.

**Resultado:** A tarefa `T-Poisson` está totalmente desbloqueada e pronta para ser iniciada pelo Codex.

---

### Histórico anterior

Claude concluiu T-029 (2026-05-08): revisão dos contratos de `NeighborSearchGrid` e `PeriodicBoundary2D`. Implementações estão corretas para Phase D (não-periódica). Registrados R-014 (ponteiro bruto para NodeCloud), R-015 (mls_evaluate não periódico, bloqueante para Phase F) e R-016 (query_radius não periódico, bloqueante para Phase F). Proposta DEC-0022 (estratégia de sobrecargas periódicas para Phase F). T-Poisson permanece desbloqueada para Phase D. T-030 proposta para Gemini confirmar os riscos antes de T-Poisson.

---

### Histórico anterior

Codex concluiu T-024, T-025 e T-026 (2026-05-08). T-024 adicionou guarda explícita em `mls_evaluate` para raio de suporte positivo e finito, além do teste `mls_robustness` para ausência de NaN/Inf e condicionamento da matriz MLS. T-025 criou `NeighborSearchGrid` não periódico, com resultados ordenados e teste contra busca bruta. T-026 criou `PeriodicBoundary2D` com `wrap` e `minimum_image`, sem acoplar partículas, MLS ou PIDC. A Fase B geométrica mínima está completa para a próxima validação. CTest passou com 12/12 testes.

---

### Histórico anterior

Claude concluiu auditoria das Fases 0/A/B/C (2026-05-08). Resultados: (1) scripts/build.sh e run_tests.sh criados — Fase A completa; (2) DEC-0018 aceita — T-Poisson desbloqueada para Codex; (3) DEC-0019 proposta sobre semântica de Node::volume — aguarda decisão de Professor/Gemini (T-023); (4) docs/validation/VALIDATION_PLAN.md criado com tolerâncias quantitativas para Fases C e D; (5) Tarefas T-025 (NeighborSearchGrid), T-026 (PeriodicBoundary2D), T-028 (revisar VALIDATION_PLAN) e T-Poisson formalizadas no AI_BOARD. Fase B permanece incompleta (NeighborSearchGrid e PeriodicBoundary2D ausentes). 9/9 testes passando.

---

### Histórico anterior

Claude concluiu T-021: R-013 era falso positivo. A fórmula de gradiente em `mls_evaluate` foi verificada matematicamente correta via diferenciação implícita de `A c = p(x)`. O "termo extra" apontado por Gemini (`(∇p)ᵀ A⁻¹ pᵢ wᵢ`) faz parte do primeiro componente da derivada implícita; a confusão foi entre `A⁻¹ pᵢ` (vector por vizinho) e `c = A⁻¹ p(x)` (vector no ponto de consulta). Testes assimétricos adicionados para `{0.3, 0.7}`, `{0.6, 0.4}` e `{0.37, 0.61}` — 9/9 testes passam. **T-Poisson desbloqueada** (pendente DEC-0018 aceita e confirmação Gemini T-022).

---

### Histórico anterior

Codex revisou a T-018 após ela ter sido atribuída ao Gemini por engano. A implementação inicial de `RegularNodeCloud2D` usava caminhos e métodos inexistentes de `Domain2D`; foi corrigida para usar `pidc/Domain2D.hpp`, `lower()`, `width()` e `height()`, e o teste foi adaptado ao framework `tests/test_utils.hpp`. O duplicado de `R-013` em `docs/ai/RISKS.md` foi removido. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_test_regular_node_cloud`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 9/9 testes.

---

### Histórico anterior

Gemini concluiu T-020: Auditoria matemática de `MLSShapeFunction`. **Foi encontrado um bug crítico na implementação do gradiente (T-017).** A fórmula implementada está incorreta, mas o teste passou devido a uma simetria no caso de teste que mascarou o erro. O risco foi registrado como R-013 e uma nova tarefa de correção, T-021, foi proposta para Claude. **A implementação do solver de Poisson (T-Poisson) está bloqueada até que T-021 seja concluída.**

---

### Histórico anterior

T-018 foi integrada e revisada: `RegularNodeCloud2D` cria uma `NodeCloud` determinística a partir de `Domain2D`, incluindo domínios quadrados/retangulares, linha única, coluna única e erro controlado para dimensão zero. Com isso, a Fase B do `TODO.md` avança. O CTest agora executa 9/9 testes com sucesso.

---

### Histórico anterior

Claude concluiu T-019: revisão de arquitetura MLS → EFG. Registrados R-010 (raio de suporte sem propriedade), R-011 (busca O(N) acumulada em EFG) e R-012 (células de integração não definidas) em `docs/ai/RISKS.md`. Proposta DEC-0018 (quadratura por células retangulares Gauss 2×2) em `docs/ai/DECISIONS.md`. `CURRENT_CONTEXT.md` atualizado para refletir o estado real. `TODO.md` Fase C sincronizado. **Bloqueante:** DEC-0018 precisa ser aceita antes de implementar o assembler EFG Poisson.

---

### Histórico anterior

Codex concluiu T-005: framework de testes definido como CTest + executáveis C++ simples, com helper comum `tests/test_utils.hpp` para `require` e `approx_equal`. Todos os testes existentes foram migrados para o helper. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_smoke`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 8/8 testes.

---

### Histórico anterior

Claude concluiu T-017: `MLSShapeFunction` implementada em `include/pidc/mls/MLSShapeFunction.hpp` como função livre `mls_evaluate(Vec2, const NodeCloud&, double) -> ShapeFunctionData`. Monta a matriz de momento $A$ (3×3), resolve $A\mathbf{c} = \mathbf{p}(\mathbf{x})$ via `PartialPivLU`, e computa gradientes diferenciando implicitamente o sistema linear. Teste `test_mls_shape_function.cpp` cobre partição da unidade, reprodução linear, gradientes de PU e LR, segundo ponto de consulta e exceção com vizinhos insuficientes. 8/8 testes passando (DEC-0016).

---

### Histórico anterior

Codex concluiu T-016: Eigen foi adicionada ao CMake com `find_package(Eigen3 3.3 REQUIRED NO_MODULE)` e propagada por `pidc_core` via `Eigen3::Eigen`. O teste `eigen_dependency` foi registrado no CTest e passou. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_smoke`, `./build/pidc_test_eigen_dependency`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram com 7/7 testes.

---

### Histórico anterior

Gemini concluiu T-015: Confirmação de que o repositório está sincronizado com as tarefas T-002, T-013 e T-014. Como próximo passo, foi proposta a tarefa T-016 para o Codex: adicionar a dependência da biblioteca Eigen via CMake, um pré-requisito para a Fase C (MLS/EFG).

---

### Histórico anterior

Gemini concluiu T-014: Revisão e conclusão de tarefas pendentes. T-004 (convenção de unidades) foi formalmente concluída, e a entrada incorreta T-008 foi removida do quadro. Decisões implementadas (`DEC-0005`, `DEC-0008`, `DEC-0013`) foram atualizadas para o status `aceita`, sincronizando a documentação.

---

### Histórico anterior

Gemini concluiu T-013: Sincronização do `TODO.md` com o `AI_BOARD.md`. As tarefas de implementação de `Species`, `NodeCloud`, `PolynomialBasis` e `WeightFunction` foram marcadas como concluídas, refletindo o trabalho já realizado por Claude e Codex.

---

### Histórico anterior

Gemini concluiu T-002: Auditoria das equações em `docs/03_modelo_matematico_minimo.md`. O documento foi atualizado para refletir as decisões `DEC-0005` (unidades SI) e `DEC-0007` (estratégia de contorno), removendo ambiguidades.

---

### Histórico anterior

Claude concluiu T-012: `PolynomialBasis` linear 2D criada em
`include/pidc/mls/PolynomialBasis.hpp`. Funções `linear_basis(x)`,
`linear_basis_dx()`, `linear_basis_dy()`. 6/6 testes passando.
Todos os ingredientes sem Eigen para `MLSShapeFunction` estão prontos.

---
Claude concluiu T-010 (rescate de T-008 / DEC-0011): `Species.hpp` criado em `include/pidc/`,
`Particle.hpp` refatorado para usar `species_id` (removidos `charge` e `mass`), `smoke.cpp`
atualizado, `test_species.cpp` criado usando padrão `require()`. 4/4 testes passando.
Nota: T-008 havia sido marcada como concluída por Gemini sem implementação real — violação de R-003.
