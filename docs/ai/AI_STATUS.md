# AI_STATUS — Estado técnico consolidado

Atualizado em: 2026-05-08

## Estado geral

Bootstrap mínimo criado e validado. O projeto compila com CMake/C++17, possui biblioteca `pidc_core`, executável smoke e teste simples de contorno periódico registrado no CTest.

## Módulos

| Módulo | Estado |
| --- | --- |
| core | bootstrap mínimo compilando |
| particles | `Vec2`, `Particle` (refatorada) e `Species` implementadas (DEC-0011) |
| geometry | `Domain2D` inicial com contorno periódico via `wrapPeriodic(Vec2)`; `NodeCloud` criado como proprietário canônico de nós |
| mls | `ShapeFunctionData` (contrato); `WeightFunction` quártica; `PolynomialBasis` linear 2D; `MLSShapeFunction` com `mls_evaluate` (PU + LR validados) |
| efg | não iniciado |
| pic | não iniciado |
| pidc | não iniciado |
| validation | CTest com 8 testes; `tests/test_utils.hpp`; partição da unidade e reprodução linear validadas |
| scripts | não iniciado |

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
| partition unity | passou em 2026-05-08 (coberto por mls_shape_function) |
| linear reproduction | passou em 2026-05-08 (coberto por mls_shape_function) |
| charge conservation | não iniciado |
| Poisson MMS | não iniciado |
| Langmuir 1D | não iniciado |

## Último resumo

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
