# T-041 — Poisson 1D periódico por FD/DFT manual

Agente: Codex  
Data: 2026-05-09

## Objetivo

Implementar um solver de Poisson 1D periódico para o PIC de referência,
resolvendo `d2phi/dx2 = -rho/epsilon0` em `Grid1D`, sem Eigen, sem
`EFGPoissonSolver`, sem MLS e sem PIDC.

## Arquivos alterados

- `include/pidc/pic/PoissonSolver1D.hpp`
- `tests/test_poisson_solver_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t041_poisson_solver_1d.md`

## Implementação

- Criado `pidc::pic::PoissonResult1D` com:
  - `std::vector<double> potential`;
  - `std::vector<double> electric_field`.
- Criada função:

```cpp
PoissonResult1D solve_poisson_periodic_1d(
    const Grid1D& grid,
    const std::vector<double>& charge_density,
    double epsilon0 = 1.0);
```

- Backend numérico:
  - DFT manual com `std::complex<double>`;
  - autovalores do operador de diferenças finitas periódico;
  - modo zero fixado como `phi_hat[0] = 0`;
  - remoção do resíduo de média numérica do potencial;
  - campo nodal por diferença central periódica.

## Testes

`tests/test_poisson_solver_1d.cpp` verifica:

- MMS periódico usando `SineManufacturedField1D`;
- erro L∞ do potencial contra `phi_exact`;
- erro L∞ do campo contra `E_exact`;
- gauge de potencial médio zero;
- densidade nula → potencial e campo nulos;
- falha para tamanho incorreto de `charge_density`;
- falha para `epsilon0 <= 0`.

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
- `/usr/bin/ctest --test-dir build --output-on-failure`: 19/19 testes passaram.
- `git diff --check`: passou.

## Hipóteses assumidas

- Para T-041, o MMS usa a densidade contínua de `SineManufacturedField1D`; por
  isso as tolerâncias do potencial e do campo aceitam erro de discretização de
  diferenças finitas.
- O gauge periódico é média zero para o potencial.

## Pendências

- T-042: interpolação CIC campo→partícula.
- T-043: leap-frog 1D isolado.
- T-044: oscilação de Langmuir 1D.
