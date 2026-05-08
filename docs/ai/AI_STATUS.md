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
| mls | não iniciado |
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
| partition unity | não iniciado |
| linear reproduction | não iniciado |
| charge conservation | não iniciado |
| Poisson MMS | não iniciado |
| Langmuir 1D | não iniciado |

## Último resumo

Codex concluiu `T-006`: `Domain2D` agora é geometria pura e não inclui nem menciona `Particle`; chamadores usam `wrapPeriodic(Vec2)` diretamente. `cmake -S . -B build`, `cmake --build build -j`, `./build/pidc_smoke`, `/usr/bin/ctest --test-dir build --output-on-failure` e `git diff --check` passaram.
