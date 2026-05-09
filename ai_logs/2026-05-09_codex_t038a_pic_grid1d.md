# T-038A — Infraestrutura mínima para PIC 1D de referência

Agente: Codex  
Data: 2026-05-09

## Resumo

Criada a infraestrutura mínima de grade uniforme 1D para o PIC eletrostático de referência, sem avançar para deposição de carga, Poisson FD/FFT, campo elétrico, leap-frog, Langmuir, comparação com PIDC ou alteração no `EFGPoissonSolver`.

## Arquivos alterados

- `include/pidc/pic/Grid1D.hpp`
- `tests/test_pic_grid1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `ai_logs/2026-05-09_codex_t038a_pic_grid1d.md`

## Implementação

- `pidc::pic::Grid1D` com campos `xmin`, `xmax`, `nx`, `dx`.
- Semântica periódica e semiaberta `[xmin, xmax)`.
- `nx` interpretado como número de nós e células periódicas.
- Coordenadas nodais: `coordinate(i) = xmin + i*dx`, para `0 <= i < nx`.
- `cell_index(x)` opera sobre `wrap_position(x)`.
- `left_node_index(x) = cell_index(x)`.
- `right_node_index(x) = (left + 1) % nx`.
- `fraction_in_cell(x)` retorna a coordenada local dentro da célula periódica.

## Testes

`tests/test_pic_grid1d.cpp` verifica:

- `dx` correto;
- coordenadas nodais;
- wrap de posições negativas e maiores que `xmax`;
- índice de célula para pontos internos;
- comportamento em `xmax` e logo antes da borda direita;
- vizinhos esquerdo/direito com periodicidade;
- fração local dentro da célula;
- falha controlada para `nx = 0`;
- falha controlada para posição não finita.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## Resultados

- `cmake -S . -B build`: passou.
- `cmake --build build -j`: passou.
- `/usr/bin/ctest --test-dir build --output-on-failure`: 16/16 testes passaram, incluindo `pic_grid1d`.
- `git diff --check`: passou.

## Hipóteses assumidas

- Para a grade PIC 1D periódica, `nx` conta simultaneamente os nós e as células do domínio semiaberto `[xmin, xmax)`.
- O nó à direita da última célula é o nó `0`, por periodicidade.

## Pendências

- Implementar deposição de carga em grade regular 1D.
- Implementar solver de Poisson por diferenças finitas ou FFT.
- Implementar interpolação de campo, leap-frog e oscilação de Langmuir em tarefas separadas.

## Próximo passo sugerido

T-038B: implementar deposição de carga em grade regular 1D usando `Grid1D`, com teste de conservação de carga.
