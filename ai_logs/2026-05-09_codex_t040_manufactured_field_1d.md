# T-040 — Campo elétrico manufaturado em Grid1D

Agente: Codex  
Data: 2026-05-09

## Objetivo

Implementar funções manufaturadas 1D para potencial, campo elétrico e densidade
de carga em grade periódica, sem resolver Poisson e sem avançar para
interpolação campo→partícula, leap-frog, Langmuir, PIDC, MLS ou EFG.

## Arquivos alterados

- `include/pidc/pic/ManufacturedField1D.hpp`
- `tests/test_manufactured_field_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t040_manufactured_field_1d.md`

## Implementação

- Criada `pidc::pic::SineManufacturedField1D`.
- Fórmulas:
  - `phi(x) = A sin(2*pi*x/L)`;
  - `E(x) = -A * (2*pi/L) * cos(2*pi*x/L)`;
  - `rho(x) = epsilon0 * (2*pi/L)^2 * A * sin(2*pi*x/L)`.
- Criadas funções de amostragem nodal:
  - `sample_potential`;
  - `sample_electric_field`;
  - `sample_charge_density`.
- Todos os vetores nodais usam `std::vector<double>`.
- O header não inclui Eigen, MLS, EFG ou PIDC.

## Testes

`tests/test_manufactured_field_1d.cpp` verifica:

- valores analíticos em `0`, `L/4`, `L/2`, `3L/4`;
- tamanho dos vetores retornados por `sample_potential`,
  `sample_electric_field` e `sample_charge_density`;
- periodicidade entre `x` e `x ± L`;
- soma nodal de `rho_i` aproximadamente zero;
- falha explícita para `length <= 0` e `epsilon0 <= 0`.

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
- `/usr/bin/ctest --test-dir build --output-on-failure`: 18/18 testes passaram.
- `git diff --check`: passou.

## Hipóteses assumidas

- O campo manufaturado usa origem analítica em `x = 0`, conforme especificado
  pelo professor.
- O comprimento `L` da solução manufaturada é armazenado em
  `SineManufacturedField1D::length`.

## Pendências

- T-041: implementar Poisson 1D periódico por diferenças finitas/DFT manual.
- T-042: implementar interpolação CIC campo→partícula.
- T-043: implementar leap-frog 1D isolado.
- T-044: implementar oscilação de Langmuir 1D.
