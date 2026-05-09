# Log — Codex — T-042 FieldInterpolation1D

Data: 2026-05-09

## Tarefa

Implementar a interpolação CIC do campo nodal para posições de partículas em `Grid1D`, mantendo o módulo PIC 1D independente de MLS, EFG, PIDC e Eigen.

## Arquivos alterados

- `include/pidc/pic/FieldInterpolation1D.hpp`
- `tests/test_field_interpolation_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t042_field_interpolation_1d.md`

## Implementação

- Criada a função escalar:
  `interpolate_field_cic_1d(const Grid1D&, const std::vector<double>&, double)`.
- Criada a sobrecarga para `std::vector<pidc::Particle>`.
- Regra usada:
  `E_p = E_left*(1-f) + E_right*f`, com índices e fração vindos de `Grid1D`.
- Validação explícita:
  - tamanho de `field_nodes` igual a `grid.size()`;
  - valores nodais finitos.

## Testes adicionados

- Casos exatos em `Grid1D{0,1,4}` com campo nodal `{0,4,8,12}`.
- Periodicidade para posições negativas e maiores que `xmax`.
- Sobrecarga vetorial para partículas.
- Campo uniforme.
- Campo manufaturado senoidal amostrado em `nx=128`, tolerância `3e-3`.
- Falhas para tamanho incorreto e valor não finito.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

Resultado final: 20/20 testes passando; `git diff --check` sem saída.

## Hipóteses assumidas

- `Particle::position.x` é a única coordenada usada nesta referência PIC 1D.
- A interpolação deve compartilhar a semântica periódica semiaberta de `Grid1D`.
- O teste MMS de interpolação usa erro de interpolação linear esperado, não exige coincidência analítica exata fora dos nós.

## Pendências

- Próxima tarefa sugerida: T-043 — leap-frog 1D isolado.
