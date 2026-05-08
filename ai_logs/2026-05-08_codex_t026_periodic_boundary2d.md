# Log — Codex — T-026 PeriodicBoundary2D

Data: 2026-05-08

## Resumo

Criei `PeriodicBoundary2D` como helper geométrico separado de `Domain2D`,
oferecendo `wrap(Vec2)` e `minimum_image(Vec2)` sem acoplar partículas, MLS,
busca de vizinhos ou PIDC.

## Arquivos alterados

- `include/pidc/geometry/PeriodicBoundary2D.hpp`
- `tests/test_periodic_boundary2d.cpp`
- `CMakeLists.txt`
- `docs/ai/DECISIONS.md`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`

## Comandos executados

```bash
cmake --build build -j
./build/pidc_test_periodic_boundary2d
git diff --check
git commit -m "feat: add periodic boundary helper"
cmake -S . -B build
cmake --build build -j
./build/pidc_test_periodic_boundary2d
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## Testes e resultados

- `pidc_test_periodic_boundary2d`: passou.
- O teste cobre múltiplas voltas, deslocamentos positivos/negativos e empates
  exatos em meia-caixa.
- Suíte final: 12/12 testes passaram.

## Hipóteses assumidas

- Empates exatos em `+/-L/2` são preservados.
- `wrap` reutiliza a semântica já validada de `Domain2D::wrapPeriodic`.

## Pendências

- `NeighborSearchGrid` ainda não usa menor imagem periódica.
- Nenhum módulo PIDC foi implementado nesta tarefa.

## Próximo passo sugerido

Antes de usar `minimum_image` em busca de vizinhos ou deposição PIDC, revisar a
convenção de deslocamento com Claude/Gemini (T-029).
