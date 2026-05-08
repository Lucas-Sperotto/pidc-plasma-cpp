# Log — Codex — T-025 NeighborSearchGrid

Data: 2026-05-08

## Resumo

Criei `NeighborSearchGrid` como uma grade 2D não periódica para busca de nós por
raio. A busca retorna IDs globais ordenados e filtra os candidatos por distância
euclidiana exata. A implementação ainda não foi integrada ao MLS.

## Arquivos alterados

- `include/pidc/geometry/NeighborSearchGrid.hpp`
- `tests/test_neighbor_search_grid.cpp`
- `CMakeLists.txt`
- `docs/ai/DECISIONS.md`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`

## Comandos executados

```bash
cmake --build build -j
./build/pidc_test_neighbor_search_grid
git diff --check
git commit -m "feat: add neighbor search grid"
cmake -S . -B build
cmake --build build -j
./build/pidc_test_neighbor_search_grid
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## Testes e resultados

- `pidc_test_neighbor_search_grid`: passou.
- O teste compara a grade contra uma busca bruta em pontos internos, de borda e
  externos.
- Suíte final: 12/12 testes passaram.

## Hipóteses assumidas

- A versão v1 é não periódica.
- `NodeCloud` deve viver mais que `NeighborSearchGrid`.
- Nós na borda superior do domínio são aceitos por clamp para a última célula.

## Pendências

- A integração com `mls_evaluate` não foi feita nesta tarefa.
- A busca periódica deve ser especificada separadamente, possivelmente usando
  `PeriodicBoundary2D`.

## Próximo passo sugerido

Claude + Gemini devem revisar os contratos de `NeighborSearchGrid` e
`PeriodicBoundary2D` antes de qualquer acoplamento com MLS/PIDC (T-029).
