# Plano Arquitetural — Fase E: PIC 1D de Referência

Aprovado em: 2026-05-09

**Contexto:** Antes de implementar a Fase F (PIDC completo), precisamos de um PIC 1D
clássico e validado independentemente. Esse código serve como referência para comparação
futura com o PIDC e valida a sequência física completa: deposição CIC → Poisson periódico
→ interpolação de campo → leap-frog → oscilação de Langmuir.

As fronteiras arquiteturais foram estabelecidas em T-038C (DEC-0027/0028, R-018–R-021).
O módulo `include/pidc/pic/` é proibido de incluir `mls/`, `efg/` ou `<Eigen/>`.

---

## Definição de "PIC 1D de referência" neste projeto

Um PIC 1D de referência é um código eletrostático 1D em grade uniforme periódica que:

1. Usa deposição CIC (bilinear) — não MLS;
2. Resolve Poisson com diferenças finitas ou DFT — não EFG;
3. Interpola campo via CIC reverso — não `mls_evaluate`;
4. Avança partículas com leap-frog de Störmer-Verlet;
5. Reproduz a frequência de plasma de Langmuir: `ω_obs ∈ [0.8, 1.2] × ω_pe`.

Nenhum critério de "referência" é satisfeito sem o item 5.

---

## Sequência de tarefas

| ID    | Módulo                    | Responsável | Arquivo principal                              |
|-------|---------------------------|-------------|------------------------------------------------|
| T-039 | Deposição CIC 1D          | Codex       | `include/pidc/pic/ChargeDeposition1D.hpp`      |
| T-040 | Poisson 1D periódico      | Codex       | `include/pidc/pic/PoissonSolver1D.hpp`         |
| T-041 | Interpolação campo CIC 1D | Codex       | `include/pidc/pic/FieldInterpolation1D.hpp`    |
| T-042 | Leap-frog 1D              | Codex       | `include/pidc/pic/LeapFrog1D.hpp`              |
| T-043 | Langmuir 1D (integração)  | Codex       | `apps/langmuir_1d.cpp`                         |

Cada tarefa desbloqueia a próxima. Nenhuma tarefa pode ser pulada.

---

## T-039 — Deposição CIC 1D

### Arquivo
`include/pidc/pic/ChargeDeposition1D.hpp`

### Interface
```cpp
namespace pidc::pic {

// DEC-0028: retorna std::vector<double>, sem Eigen
std::vector<double> deposit_charge_cic_1d(
    const Grid1D& grid,
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species_list);

} // namespace pidc::pic
```

### Algoritmo
Para cada partícula `p`:
- `f = grid.fraction_in_cell(x_p)` ∈ [0.0, 1.0)
- `left  = grid.left_node_index(x_p)`
- `right = grid.right_node_index(x_p)` = `(left+1) % nx`
- `Q[left] += q * (1.0 - f)`
- `Q[right] += q * f`

Onde `q = species_list[p.species_id].charge`.

### Testes obrigatórios — `tests/test_cic_deposition_1d.cpp`
1. Partícula no centro da célula → 50% em `left`, 50% em `right`.
2. Partícula sobre o nó `i` (f=0.0) → 100% em nó `i`.
3. Partícula próxima a `xmax` → periodicidade: nós `nx-1` e `0`.
4. Conservação global: `|Σ Q_i − Σ q_p| < 1e-12` com 10 partículas aleatórias.
5. Vetor de retorno tem tamanho `grid.size()`.

### Critério de aceite
Todos os 5 subtestes passam. CTest: 17/17.

---

## T-040 — Poisson 1D periódico (DFT/FD)

### Arquivo
`include/pidc/pic/PoissonSolver1D.hpp`

### Interface
```cpp
namespace pidc::pic {

struct PoissonResult1D {
    std::vector<double> potential;  // φ_i, tamanho nx
    std::vector<double> field;      // E_i centrado periódico, tamanho nx
};

// Resolve ∇²φ = -ρ/ε₀ com condição periódica.
// gauge: potencial médio = 0 (φ̂_0 = 0).
PoissonResult1D solve_poisson_periodic_1d(
    const Grid1D& grid,
    const std::vector<double>& charge_density,
    double epsilon0 = 8.854187817e-12);

} // namespace pidc::pic
```

### Algoritmo (DFT manual via `std::complex<double>`)
1. DFT forward: `ρ̂_k = Σ_j ρ_j · exp(-2πi·j·k/n)`
2. Autovalores FD: `μ_k = 2·(cos(2πk/n) - 1) / dx²`
3. `φ̂_k = -ρ̂_k / (ε₀ · μ_k)`, com `φ̂_0 = 0`
4. DFT inverse
5. Campo: `E_i = -(φ_{(i+1)%n} - φ_{(i-1+n)%n}) / (2·dx)`

Sem Eigen, sem FFT externa. Apenas `<complex>`, `<cmath>`, `<vector>`.

### Testes obrigatórios — `tests/test_poisson_1d.cpp`
MMS: `ρ(x) = ε₀ · (2π/L)² · sin(2πx/L)` → `φ(x) = sin(2πx/L)`.
1. `||φ_computed − φ_exact||_∞ < 5e-3` para `nx=32`.
2. `||E_computed − E_exact||_∞ < 5e-3` para `nx=32`.
3. Potencial médio ≈ 0 (gauge verificado).
4. Tamanho de retorno = `nx`.

### Critério de aceite
CTest: 18/18.

---

## T-041 — Interpolação de campo CIC 1D

### Arquivo
`include/pidc/pic/FieldInterpolation1D.hpp`

### Interface
```cpp
namespace pidc::pic {

// CIC reverso: E_p = E[left]*(1-f) + E[right]*f
double interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& E_nodes,
    double x);

} // namespace pidc::pic
```

### Testes obrigatórios — `tests/test_field_interpolation_1d.cpp`
1. Centro de célula → média dos dois nós.
2. Sobre nó `i` (f=0) → `E_nodes[i]` exato.
3. Campo uniforme → retorna a constante em qualquer posição.
4. Campo linear → interpolação precisa em ponto interior.
5. Periodicidade: perto de `xmax` interpola `nx-1` e `0`.

### Critério de aceite
CTest: 19/19.

---

## T-042 — Leap-frog 1D (Störmer-Verlet)

### Arquivo
`include/pidc/pic/LeapFrog1D.hpp`

### Interface
```cpp
namespace pidc::pic {

void leapfrog_advance_1d(
    pidc::Particle& particle,
    const Grid1D& grid,
    double E_at_particle,
    double charge_over_mass,
    double dt);

} // namespace pidc::pic
```

### Testes obrigatórios — `tests/test_leapfrog_1d.cpp`
1. Campo zero → movimento uniforme.
2. Campo uniforme → aceleração verificada em 2 passos.
3. Reversibilidade: 100 forward + 100 backward → erro < 1e-10.
4. Periodicidade: posição além de `xmax` retorna para `[xmin, xmax)`.

### Critério de aceite
CTest: 20/20.

---

## T-043 — Langmuir 1D (integração completa)

### Arquivo
`apps/langmuir_1d.cpp`

### Setup — unidades normalizadas
- `ε₀ = 1`, `e = 1`, `m_e = 1`, `n₀ = 1` → `ω_pe = 1`
- `L = 1.0`, `nx = 32`, `N_e = 64`, `dt = 0.1`, `A = 0.01`
- Perturbação: `x_p = p*L/N + A*sin(2π*x_p/L)`

### Sequência por passo
1. `deposit_charge_cic_1d` → `rho`
2. `solve_poisson_periodic_1d` → `phi`, `E_nodes`
3. `interpolate_field_cic_1d` para cada partícula
4. `leapfrog_advance_1d` para cada partícula

### Critério de aceite (Marco 4)
`ω_obs ∈ [0.8, 1.2] × ω_pe` após `T_sim = 20 / ω_pe`.

---

## Proibições explícitas até T-044

1. Nenhum `#include "pidc/mls/..."` ou `#include "pidc/efg/..."` em `include/pidc/pic/`.
2. Nenhum `#include <Eigen/...>` em `include/pidc/pic/`.
3. Não usar `EFGPoissonSolver` nem `mls_evaluate` no código PIC 1D.
4. Não avançar para T-043 sem T-039–T-042 individualmente validados.
5. Não declarar "PIC 1D validado" sem o critério de Langmuir.
6. Não comparar PIC vs PIDC antes de T-043 completo e Fase F completa.

---

## Estado ao iniciar (pré-T-039)

- CTest: 16/16 passando
- `Grid1D` pronto (`include/pidc/pic/Grid1D.hpp`, T-038A)
- DEC-0027, DEC-0028 aceitas; R-018–R-021 registrados
