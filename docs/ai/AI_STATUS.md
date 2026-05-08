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
| mls | `ShapeFunctionData` (contrato); `WeightFunction` quártica; `PolynomialBasis` linear 2D criadas |
| efg | não iniciado |
| pic | não iniciado |
| pidc | não iniciado |
| validation | não iniciado |
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
| partition unity | não iniciado |
| linear reproduction | não iniciado |
| charge conservation | não iniciado |
| Poisson MMS | não iniciado |
| Langmuir 1D | não iniciado |

## Último resumo

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
