# AI_STATUS — Estado técnico consolidado

Atualizado em: 2026-05-08

## Estado geral

Bootstrap mínimo criado e validado. O projeto compila com CMake/C++17, possui biblioteca `pidc_core`, executável smoke e teste simples de contorno periódico registrado no CTest.

## Módulos

| Módulo | Estado |
|---|---|
| core | bootstrap mínimo compilando |
| particles | estruturas `Particle` e `Vec2` iniciais |
| geometry | `Domain2D` inicial com contorno periódico via `wrapPeriodic(Vec2)`; sem dependência de `Particle` |
| mls | `ShapeFunctionData` (contrato de interface) criada; sem implementação de cálculo |
| efg | não iniciado |
| pic | não iniciado |
| pidc | não iniciado |
| validation | não iniciado |
| scripts | não iniciado |

## Testes

| Teste | Estado |
|---|---|
| smoke build | passou em 2026-05-08 |
| periodic boundary | passou em 2026-05-08 |
| shape_function_data | passou em 2026-05-08 |
| partition unity | não iniciado |
| linear reproduction | não iniciado |
| charge conservation | não iniciado |
| Poisson MMS | não iniciado |
| Langmuir 1D | não iniciado |

## Último resumo

Claude concluiu `T-007` (DEC-0009): `ShapeFunctionData` criada em `include/pidc/ShapeFunctionData.hpp` como contrato entre MLS, EFG e PIDC. Struct header-only com `neighbor_ids`, `phi`, `grad_phi` e função livre `is_valid()`. Teste `shape_function_data` registrado no CTest e passando. 2/2 testes passando.
