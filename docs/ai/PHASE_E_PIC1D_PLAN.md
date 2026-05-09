# Plano Arquitetural — Fase E: PIC 1D de Referência

Aprovado em: 2026-05-09
Atualizado em: 2026-05-09 (T-040C — sincronização de numeração)

**Nota:** A numeração canônica desta Fase E foi definida definitivamente em
`docs/ai/PHASE_E_PIC1D_TECH_PLAN.md`. Este documento usa a mesma numeração.

**Contexto:** Antes de implementar a Fase F (PIDC completo), precisamos de um PIC 1D
clássico e validado independentemente. Esse código serve como referência para comparação
futura com o PIDC e valida a sequência física completa: deposição CIC → campo manufaturado
→ Poisson periódico → interpolação de campo → leap-frog → oscilação de Langmuir.

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

| ID     | Módulo                        | Estado  | Arquivo principal                                |
|--------|-------------------------------|---------|--------------------------------------------------|
| T-038A | Grid1D periódico              | ✓ pronto | `include/pidc/pic/Grid1D.hpp`                   |
| T-039  | Deposição CIC 1D              | ✓ pronto | `include/pidc/pic/ChargeDeposition1D.hpp`       |
| T-040  | Campo elétrico manufaturado   | ✓ pronto | `include/pidc/pic/ManufacturedField1D.hpp`      |
| T-041  | Poisson 1D periódico          | pendente | `include/pidc/pic/PoissonSolver1D.hpp`          |
| T-042  | Interpolação campo CIC 1D     | pendente | `include/pidc/pic/FieldInterpolation1D.hpp`     |
| T-043  | Leap-frog 1D                  | pendente | `include/pidc/pic/LeapFrog1D.hpp`               |
| T-044  | Langmuir 1D (integração)      | pendente | `include/pidc/pic/Langmuir1D.hpp`               |

Cada tarefa desbloqueia a próxima. Nenhuma tarefa pode ser pulada.

---

## T-038A — Grid1D periódico (concluída)

`include/pidc/pic/Grid1D.hpp` — semântica semiaberta `[xmin, xmax)`, `nx` nós = `nx`
células, `dx = L/nx` (DEC-0027). Teste `test_pic_grid1d` passando.

---

## T-039 — Deposição CIC 1D (concluída)

### Arquivo
`include/pidc/pic/ChargeDeposition1D.hpp`

### Interface
```cpp
namespace pidc::pic {
std::vector<double> deposit_charge_cic_1d(
    const Grid1D& grid,
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species_list);
} // namespace pidc::pic
```

Algoritmo CIC: `Q[left] += q*(1-f)`, `Q[right] += q*f`.
Conservação: `|ΣQ_i − Σq_p| < 1e-12`.
Teste `test_cic_deposition_1d`: 6 subtestes passando.

---

## T-040 — Campo elétrico manufaturado (concluída)

### Arquivo
`include/pidc/pic/ManufacturedField1D.hpp`

Fornece soluções analíticas exatas (φ, E, ρ) para validação do solver Poisson (T-041)
e do interpolador (T-042). Sem dependência de Eigen.

Teste `test_manufactured_field_1d` passando. CTest: 18/18.

---

## T-041 — Poisson 1D periódico (pendente)

### Arquivo
`include/pidc/pic/PoissonSolver1D.hpp`

### Interface
```cpp
namespace pidc::pic {

struct PoissonResult1D {
    std::vector<double> potential;    // φ_i, tamanho nx
    std::vector<double> electric_field; // E_i centrado periódico, tamanho nx
};

// Resolve ∇²φ = -ρ/ε₀ com condição periódica.
// gauge: potencial médio = 0 (φ̂_0 = 0).
PoissonResult1D solve_poisson_periodic_1d(
    const Grid1D& grid,
    const std::vector<double>& charge_density,
    double epsilon0 = 1.0);

} // namespace pidc::pic
```

### Algoritmo (DFT manual via `std::complex<double>`)
1. DFT forward de `ρ_i`
2. Autovalores FD: `λ_k = 4·sin²(πk/n)/dx²`
3. `φ̂_k = ρ̂_k / (ε₀·λ_k)`, com `φ̂_0 = 0` (gauge)
4. DFT inverse
5. Campo: `E_i = -(φ_{(i+1)%n} - φ_{(i-1+n)%n}) / (2·dx)`

Sem Eigen, sem FFT externa.

### Testes obrigatórios — `tests/test_poisson_1d.cpp`
MMS discreto: construir `ρ_i` a partir do laplaciano FD de `φ_i = sin(2πx_i/L)`.
1. `||φ_computed − φ_exact||_∞ < 1e-10` para nx=32.
2. `||E_computed − E_exact||_∞ < 1e-10` para nx=32.
3. Potencial médio `< 1e-12` (gauge).
4. Densidade zero → potencial e campo zero.
5. Tamanho de retorno = nx.

### Critério de aceite
CTest: 19/19.

---

## T-042 — Interpolação campo CIC 1D (pendente)

### Arquivo
`include/pidc/pic/FieldInterpolation1D.hpp`

### Interface
```cpp
namespace pidc::pic {

// CIC reverso: E_p = E[left]*(1-f) + E[right]*f
double interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    double x);

// Sobrecarga vetorial por partícula
std::vector<double> interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    const std::vector<pidc::Particle>& particles);

} // namespace pidc::pic
```

### Testes obrigatórios — `tests/test_field_interpolation_1d.cpp`
1. Centro de célula → média dos dois nós.
2. Sobre nó `i` (f=0) → `E_nodes[i]` exato.
3. Campo uniforme → retorna a constante em qualquer posição.
4. Campo manufaturado (`sinusoidal_electric_field_1d`, nx=128): erro `< 3e-3`.
5. Periodicidade: perto de `xmax` interpola `nx-1` e `0`.
6. Tamanho errado → throw.

### Critério de aceite
CTest: 20/20.

---

## T-043 — Leap-frog 1D (pendente)

### Arquivo
`include/pidc/pic/LeapFrog1D.hpp`

### Interface
```cpp
namespace pidc::pic {

// Recua velocidade de dt/2 a partir de v(t=0) para inicializar o stagger
void initialize_leapfrog_velocity_1d(
    pidc::Particle& particle,
    double electric_field,
    double charge_over_mass,
    double dt);

// Avança: v_{n+1/2} = v_{n-1/2} + (q/m)*E*dt; x_{n+1} = x_n + v_{n+1/2}*dt
void leapfrog_advance_1d(
    pidc::Particle& particle,
    const Grid1D& grid,
    double electric_field,
    double charge_over_mass,
    double dt);

} // namespace pidc::pic
```

### Testes obrigatórios — `tests/test_leapfrog_1d.cpp`
1. Campo zero → movimento uniforme, posição linear.
2. Campo constante → posição e velocidade analíticas em 2 passos.
3. Reversibilidade: 100 passos forward + 100 passos backward → erro `< 1e-10`.
4. Periodicidade: posição além de `xmax` retorna para `[xmin, xmax)`.

### Critério de aceite
CTest: 21/21.

---

## T-044 — Langmuir 1D (pendente)

### Arquivo
`include/pidc/pic/Langmuir1D.hpp` + teste `tests/test_langmuir_1d.cpp`

### Unidades normalizadas
- `ε₀ = 1`, `e = 1`, `m_e = 1`, `n₀ = 1` → `ω_pe = 1`
- `L = 1.0`, `nx = 64`, `N_e = 128`, `dt = 0.05`, perturbação `A = 1e-3`

### Sequência por passo
1. `deposit_charge_cic_1d` → `Q[nx]`
2. Somar fundo iônico: `ρ_i = Q_i/dx + 1.0`
3. `solve_poisson_periodic_1d` → `phi[nx]`, `E_nodes[nx]`
4. `interpolate_field_cic_1d` para cada partícula
5. `leapfrog_advance_1d` para cada partícula

### Critério de aceite (Marco 4)
`ω_obs ∈ [0.8, 1.2] × ω_pe` após `T_sim = 2500 × dt`.

---

## Arquivos esperados por tarefa

| Tarefa | Header | Teste |
|--------|--------|-------|
| T-038A | `Grid1D.hpp` | `test_pic_grid1d.cpp` ✓ |
| T-039  | `ChargeDeposition1D.hpp` | `test_cic_deposition_1d.cpp` ✓ |
| T-040  | `ManufacturedField1D.hpp` | `test_manufactured_field_1d.cpp` ✓ |
| T-041  | `PoissonSolver1D.hpp` | `test_poisson_1d.cpp` |
| T-042  | `FieldInterpolation1D.hpp` | `test_field_interpolation_1d.cpp` |
| T-043  | `LeapFrog1D.hpp` | `test_leapfrog_1d.cpp` |
| T-044  | `Langmuir1D.hpp` | `test_langmuir_1d.cpp` |

---

## Proibições explícitas até T-044

1. Nenhum `#include "pidc/mls/..."` ou `#include "pidc/efg/..."` em `include/pidc/pic/`.
2. Nenhum `#include <Eigen/...>` em `include/pidc/pic/`.
3. Não usar `EFGPoissonSolver` nem `mls_evaluate` no código PIC 1D.
4. Não avançar para T-044 sem T-039–T-043 individualmente validados.
5. Não declarar "PIC 1D validado" sem o critério de Langmuir.
6. Não comparar PIC vs PIDC antes de T-044 completo e Fase F completa.

---

## Estado ao corrigir a numeração (T-040C)

- CTest: **18/18 passando**
- Concluídas: T-038A, T-039, T-040
- Próxima tarefa: **T-041** (Codex) — `PoissonSolver1D` periódico
- Plano técnico detalhado: `docs/ai/PHASE_E_PIC1D_TECH_PLAN.md`
