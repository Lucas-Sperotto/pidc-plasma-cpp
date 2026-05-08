# 2026-05-08 — Codex — remover applyPeriodic de Domain2D

## Objetivo

Implementar a `T-006`/`DEC-0008`: remover `applyPeriodic(Particle&)` de `Domain2D` para deixar o domínio como geometria pura, dependente apenas de `Vec2`.

## Arquivos alterados

- `include/pidc/Domain2D.hpp`
- `src/Domain2D.cpp`
- `apps/smoke.cpp`
- `tests/test_periodic_boundary.cpp`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `ai_logs/2026-05-08_codex_remove_apply_periodic.md`

## Comandos executados

- `git status --short`
- `sed -n '1,220p' docs/ai/AI_BOARD.md`
- `sed -n '1,220p' docs/ai/AI_STATUS.md`
- `rg -n "applyPeriodic|Particle" include/pidc src apps tests docs/ai/DECISIONS.md`
- `rg -n "applyPeriodic|Particle" include/pidc/Domain2D.hpp src/Domain2D.cpp tests/test_periodic_boundary.cpp apps/smoke.cpp`
- `sed -n '1,180p' include/pidc/Domain2D.hpp`
- `sed -n '1,180p' tests/test_periodic_boundary.cpp`
- `cmake -S . -B build`
- `cmake --build build -j`
- `./build/pidc_smoke`
- `/usr/bin/ctest --test-dir build --output-on-failure`
- `git diff --check`
- `rg -n "applyPeriodic|Particle" include/pidc/Domain2D.hpp src/Domain2D.cpp tests/test_periodic_boundary.cpp`
- `git diff --stat`
- `git status --short`

## Testes executados

- `cmake -S . -B build`: passou.
- `cmake --build build -j`: passou.
- `./build/pidc_smoke`: passou.
- `/usr/bin/ctest --test-dir build --output-on-failure`: passou, 1/1 teste.
- `git diff --check`: passou.
- `rg -n "applyPeriodic|Particle" include/pidc/Domain2D.hpp src/Domain2D.cpp tests/test_periodic_boundary.cpp`: não encontrou ocorrências.

## Resultados

- `Domain2D.hpp` não inclui mais `Particle.hpp`.
- `Domain2D` não declara nem implementa mais `applyPeriodic(Particle&)`.
- `apps/smoke.cpp` preserva o comportamento usando `particle.position = domain.wrapPeriodic(particle.position)`.
- `tests/test_periodic_boundary.cpp` testa diretamente `wrapPeriodic(Vec2)`.

## Hipóteses assumidas

- A `DEC-0008`, embora ainda marcada como proposta em `DECISIONS.md`, foi considerada pronta para implementação porque está listada como `T-006` no `AI_BOARD.md`.
- O domínio periódico continua sendo tratado como semiaberto: `[lower, upper)`.

## Pendências

- Decidir se `DEC-0008` deve ser promovida de `proposta` para `aceita` em uma revisão posterior.
- Definir framework de testes (`T-005`).
- Implementar as próximas estruturas pequenas: `Species`, `ShapeFunctionData` ou `NodeCloud`.

## Próximo passo sugerido

Implementar `T-008` (`Species`) ou `T-009` (`NodeCloud`) antes de avançar para busca de vizinhança e MLS.
