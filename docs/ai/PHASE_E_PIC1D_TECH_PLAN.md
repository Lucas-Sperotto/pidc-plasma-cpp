# PHASE_E_PIC1D_TECH_PLAN — Plano técnico da Fase E

Data: 2026-05-09  
Status: plano técnico definitivo para a sequência T-038A–T-044

Este documento consolida a implementação incremental do PIC eletrostático 1D de
referência. Ele supersede a numeração divergente de
`docs/ai/PHASE_E_PIC1D_PLAN.md`, sem apagar esse plano anterior.

## Regras de arquitetura

- Todo código da Fase E fica em `include/pidc/pic/` e `tests/`.
- `include/pidc/pic/` não deve incluir `pidc/mls/`, `pidc/efg/`,
  `pidc/pidc/` nem `<Eigen/...>`.
- Vetores nodais PIC 1D usam `std::vector<double>`.
- Partículas usam `pidc::Particle` e `pidc::Species` já existentes, somente
  com as componentes `.position.x` e `.velocity.x`.
- Não usar `EFGPoissonSolver`, `mls_evaluate` ou `pidc::deposit_charge`.
- Cada tarefa deve ser validada isoladamente antes da próxima.

## Comandos obrigatórios de validação

Executar ao concluir cada tarefa:

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

## T-038A — Grid1D periódico

Estado: implementada por Codex e validada.

Header existente:

- `include/pidc/pic/Grid1D.hpp`

Tipo/API:

```cpp
namespace pidc::pic {

struct Grid1D {
    double xmin;
    double xmax;
    std::size_t nx;
    double dx;

    Grid1D(double xmin_value, double xmax_value, std::size_t nx_value);

    std::size_t size() const noexcept;
    double coordinate(std::size_t i) const;
    double wrap_position(double x) const;
    std::size_t cell_index(double x) const;
    std::size_t left_node_index(double x) const;
    std::size_t right_node_index(double x) const;
    double fraction_in_cell(double x) const;
};

} // namespace pidc::pic
```

Semântica:

- Domínio periódico semiaberto `[xmin, xmax)`.
- `nx` nós = `nx` células.
- `dx = (xmax - xmin) / nx`.
- O nó `nx` é a imagem periódica do nó `0`.

Teste existente:

- `tests/test_pic_grid1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 4`, `dx = 0.25`.
- Nós em `0.0`, `0.25`, `0.5`, `0.75`.
- Pontos de wrap: `-0.25`, `-1.125`, `1.0`, `1.25`, `2.5`.
- Ponto de borda: `std::nextafter(1.0, 0.0)`.

Tolerância:

- Comparações geométricas: `1e-12`.

## T-039 — Deposição CIC 1D conservativa

Estado: implementada por Claude e validada.

Header existente:

- `include/pidc/pic/ChargeDeposition1D.hpp`

Função/API:

```cpp
namespace pidc::pic {

std::vector<double> deposit_charge_cic_1d(
    const Grid1D& grid,
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species_list);

} // namespace pidc::pic
```

Algoritmo:

- Para cada partícula `p`, usar `x = p.position.x`.
- Obter `f = grid.fraction_in_cell(x)`.
- Obter `left = grid.left_node_index(x)`.
- Obter `right = grid.right_node_index(x)`.
- Obter `q = species_list[p.species_id].charge`.
- Acumular `Q[left] += q * (1.0 - f)`.
- Acumular `Q[right] += q * f`.
- Lançar `std::out_of_range` se `species_id` for inválido.

Teste existente:

- `tests/test_cic_deposition_1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 4`.
- Partícula em `0.125`: 50% no nó `0`, 50% no nó `1`.
- Partícula em `0.25`: 100% no nó `1`.
- Partícula em `0.875`: 50% no nó `3`, 50% no nó `0`.
- Dez partículas em posições fixas para conservação global.
- `species_id = 99` para falha controlada.

Tolerância:

- Conservação global: `1e-12`.
- Pesos nodais determinísticos: `1e-12`.

## T-040 — Campo elétrico manufaturado em Grid1D

Criar header:

- `include/pidc/pic/ManufacturedField1D.hpp`

Funções/API:

```cpp
namespace pidc::pic {

double sinusoidal_potential_1d(
    const Grid1D& grid,
    double x,
    int mode = 1,
    double amplitude = 1.0);

double sinusoidal_electric_field_1d(
    const Grid1D& grid,
    double x,
    int mode = 1,
    double amplitude = 1.0);

double sinusoidal_charge_density_1d(
    const Grid1D& grid,
    double x,
    double epsilon0 = 1.0,
    int mode = 1,
    double amplitude = 1.0);

} // namespace pidc::pic
```

Definições:

- `L = grid.xmax - grid.xmin`.
- `k = 2*pi*mode/L`.
- `s = grid.wrap_position(x) - grid.xmin`.
- `phi(x) = A sin(k s)`.
- `E(x) = -A k cos(k s)`.
- `rho(x) = epsilon0 A k^2 sin(k s)`.

Validações de entrada:

- `mode > 0`.
- `amplitude`, `epsilon0` e `x` finitos.
- `epsilon0 > 0`.

Criar teste:

- `tests/test_manufactured_field_1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 8`.
- `mode = 1`, `amplitude = 2.0`, `epsilon0 = 3.0`.
- Avaliar em `x = 0.0`, `0.25`, `0.5`, `0.75`.
- Verificar periodicidade com `x + 1.0` e `x - 1.0`.
- Testar `mode = 2` em pelo menos um ponto.
- Testar falhas para `mode = 0` e `epsilon0 <= 0`.

Tolerâncias:

- Valores analíticos: `1e-12`.
- Periodicidade: `1e-12`.

## T-041 — Poisson 1D por diferenças finitas periódico

Escolha de contorno:

- Usar Poisson periódico porque `Grid1D`, deposição CIC e Langmuir 1D são
  periódicos. Dirichlet não será usado na Fase E.

Criar header:

- `include/pidc/pic/PoissonSolver1D.hpp`

Tipos/funções/API:

```cpp
namespace pidc::pic {

struct PoissonResult1D {
    std::vector<double> potential;
    std::vector<double> electric_field;
};

PoissonResult1D solve_poisson_periodic_1d(
    const Grid1D& grid,
    const std::vector<double>& charge_density,
    double epsilon0 = 1.0);

} // namespace pidc::pic
```

Algoritmo:

- Receber `charge_density` com tamanho `grid.size()`.
- Exigir `epsilon0 > 0` e finito.
- Fazer DFT manual de `rho_i` com `std::complex<double>`.
- Subtrair implicitamente o modo médio via `phi_hat[0] = 0`.
- Para `k = 1..nx-1`, usar autovalor do operador `-Laplaciano_FD`:
  `lambda_k = 4 * sin(pi*k/nx)^2 / dx^2`.
- Resolver em Fourier: `phi_hat[k] = rho_hat[k] / (epsilon0 * lambda_k)`.
- Fazer DFT inversa com normalização `1/nx`.
- Calcular campo nodal periódico:
  `E_i = -(phi_{(i+1)%nx} - phi_{(i-1+nx)%nx}) / (2*dx)`.

Criar teste:

- `tests/test_poisson_1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 32`.
- `epsilon0 = 2.5`.
- MMS discreto:
  - `phi_i = sin(2*pi*x_i)`.
  - `rho_i = -epsilon0 * laplacian_fd(phi)_i`, com laplaciano periódico
    `(phi_{i+1} - 2*phi_i + phi_{i-1})/dx^2`.
  - `E_exact_i` calculado pela mesma diferença central usada pelo solver.
- Testar também vetor de densidade zero.
- Testar falhas para tamanho errado e `epsilon0 <= 0`.

Tolerâncias:

- Potencial `L_inf`: `1e-10`.
- Campo nodal contra derivada discreta: `1e-10`.
- Média do potencial: `1e-12`.
- Densidade zero: potencial e campo `1e-12`.

## T-042 — Interpolação campo → partícula

Criar header:

- `include/pidc/pic/FieldInterpolation1D.hpp`

Funções/API:

```cpp
namespace pidc::pic {

double interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    double x);

std::vector<double> interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    const std::vector<pidc::Particle>& particles);

} // namespace pidc::pic
```

Algoritmo:

- Exigir `field_nodes.size() == grid.size()`.
- Para um ponto `x`, usar a mesma geometria da deposição CIC:
  - `f = grid.fraction_in_cell(x)`;
  - `left = grid.left_node_index(x)`;
  - `right = grid.right_node_index(x)`;
  - `E_p = E_left*(1-f) + E_right*f`.
- A sobrecarga por partículas retorna um vetor do mesmo tamanho de `particles`,
  usando apenas `particle.position.x`.

Criar teste:

- `tests/test_field_interpolation_1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 4`, `field_nodes = {0.0, 4.0, 8.0, 12.0}`.
- `x = 0.125`: média `2.0`.
- `x = 0.25`: valor nodal `4.0`.
- Campo uniforme `{3.5, 3.5, 3.5, 3.5}` em múltiplas posições.
- Borda periódica `x = 0.875`: média entre nós `3` e `0`, valor `6.0`.
- Sobrecarga vetorial com três partículas em posições fixas.
- Campo manufaturado com `Grid1D{0,1,128}`, usando
  `sinusoidal_electric_field_1d`.
- Testar falha para vetor nodal com tamanho errado.

Tolerâncias:

- Casos exatos: `1e-12`.
- Campo manufaturado interpolado: `3e-3`.

## T-043 — Leap-frog isolado

Criar header:

- `include/pidc/pic/LeapFrog1D.hpp`

Funções/API:

```cpp
namespace pidc::pic {

void initialize_leapfrog_velocity_1d(
    pidc::Particle& particle,
    double electric_field,
    double charge_over_mass,
    double dt);

void leapfrog_advance_1d(
    pidc::Particle& particle,
    const Grid1D& grid,
    double electric_field,
    double charge_over_mass,
    double dt);

} // namespace pidc::pic
```

Convenção:

- Antes da inicialização, `particle.velocity.x` representa `v(t=0)`.
- Após `initialize_leapfrog_velocity_1d`, `particle.velocity.x` representa
  `v(t=-dt/2)`.
- Após cada `leapfrog_advance_1d`, `particle.velocity.x` representa velocidade
  de meio passo e `particle.position.x` representa posição inteira.

Algoritmo:

- `initialize`: `v_half = v0 - 0.5 * (q/m) * E * dt`.
- `advance`:
  - `v_half += (q/m) * E * dt`;
  - `x += v_half * dt`;
  - `x = grid.wrap_position(x)`.
- Exigir entradas finitas.

Criar teste:

- `tests/test_leapfrog_1d.cpp`

Dados determinísticos:

- Grid `[0, 1)`, `nx = 16`.
- Campo zero: `x0 = 0.2`, `v = 0.1`, `dt = 0.05`, dez passos.
- Campo constante: `E = 2.0`, `q/m = -0.5`, `dt = 0.01`, comparar com
  `x(t) = x0 + v0*t + 0.5*a*t^2`, `v(t+dt/2) = v0 + a*(t+dt/2)`.
- Reversibilidade: avançar 100 passos com `dt`, depois 100 passos com `-dt`,
  usando campo constante e domínio amplo o suficiente para não cruzar a borda.
- Periodicidade: posição que cruza `xmax` deve voltar para `[xmin, xmax)`.

Tolerâncias:

- Posição e velocidade em casos analíticos: `1e-10`.
- Wrap periódico: `1e-12`.

## T-044 — Oscilação de Langmuir 1D

Criar header:

- `include/pidc/pic/Langmuir1D.hpp`

Criar teste:

- `tests/test_langmuir_1d.cpp`

Tipos/função/API:

```cpp
namespace pidc::pic {

struct LangmuirConfig1D {
    double length;
    std::size_t nx;
    std::size_t particles;
    double dt;
    std::size_t steps;
    double perturbation;
};

struct LangmuirResult1D {
    double omega_observed;
    double relative_energy_drift;
    double max_charge_error;
};

LangmuirResult1D run_langmuir_1d(const LangmuirConfig1D& config);

} // namespace pidc::pic
```

Unidades normalizadas:

- `epsilon0 = 1`.
- `n0 = 1`.
- `q/m = -1`.
- `omega_pe = 1`.

Dados determinísticos:

- `length = 1.0`.
- `nx = 64`.
- `particles = 128`.
- `dt = 0.05`.
- `steps = 2500`.
- `perturbation = 1e-3`.
- Posições iniciais:
  - `x0_p = (p + 0.5) * length / particles`;
  - `x_p = x0_p + perturbation * sin(2*pi*x0_p/length)`;
  - aplicar `Grid1D::wrap_position`.
- Velocidades iniciais: zero.
- Elétrons com macrocarga `q_p = -length / particles`.
- Fundo iônico uniforme: adicionar `rho_ion = +1.0` à densidade nodal.
- Densidade eletrônica: `charge_density_i = Q_i / dx`.

Sequência por passo:

1. Depositar elétrons com `deposit_charge_cic_1d`.
2. Converter carga nodal para densidade `rho_e = Q_i / dx`.
3. Somar fundo iônico `rho_i = +1.0`.
4. Resolver Poisson periódico.
5. Interpolar campo para cada partícula.
6. Avançar partículas com leap-frog.
7. Registrar amplitude temporal do modo fundamental da densidade ou do campo.

Métrica de frequência:

- Calcular DFT temporal discreta da série do modo fundamental.
- Ignorar frequência zero.
- Selecionar frequência de maior amplitude.
- Converter para frequência angular `omega_observed = 2*pi*f_peak`.

Critérios de aceite:

- `omega_observed` em `[0.8, 1.2]`.
- Todos os valores finitos.
- Todas as partículas permanecem em `[xmin, xmax)`.
- `max_charge_error <= 1e-10`.
- `relative_energy_drift <= 0.25`.

Tolerâncias iniciais:

- Frequência: 20% relativa.
- Carga: `1e-10`.
- Energia qualitativa: deriva relativa até `0.25`.

## Atualizações de CMake esperadas

Para cada novo teste, adicionar um executável e um `add_test` em `CMakeLists.txt`:

- T-040: `pidc_test_manufactured_field_1d` / `manufactured_field_1d`.
- T-041: `pidc_test_poisson_1d` / `poisson_1d`.
- T-042: `pidc_test_field_interpolation_1d` / `field_interpolation_1d`.
- T-043: `pidc_test_leapfrog_1d` / `leapfrog_1d`.
- T-044: `pidc_test_langmuir_1d` / `langmuir_1d`.

## Critério de aceite da Fase E

A Fase E só deve ser considerada concluída quando:

- T-038A a T-043 tiverem testes unitários próprios passando.
- T-044 passar com frequência de Langmuir dentro do intervalo aceito.
- Nenhum header em `include/pidc/pic/` depender de EFG, MLS, PIDC ou Eigen.
- Todos os comandos obrigatórios de validação passarem.
