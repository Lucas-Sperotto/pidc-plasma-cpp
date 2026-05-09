# CURRENT_CONTEXT — Contexto mínimo para retomada

Atualizado em: 2026-05-09

Estamos reconstruindo em C/C++17 o método Particle-In-Diffuse-Cell (PIDC),
baseado na tese de Gleber Nelson Marques (INPE, 2008).

## Objetivo

Implementar e validar o PIDC de forma incremental:

1. MLS/EFG — **em progresso**;
2. Poisson EFG — validação MMS inicial concluída;
3. PIC baseline;
4. deposição PIDC;
5. interpolação PIDC;
6. leap-frog;
7. reprodução qualitativa da tese.

## Estado atual

**Fase B geométrica mínima concluída:**

- `Vec2`, `Particle`, `Species`, `Node` e `Domain2D` existem.
- `Domain2D` permanece geometria pura; contorno periódico básico via `wrapPeriodic(Vec2)`.
- `NodeCloud` é o proprietário canônico dos nós.
- `RegularNodeCloud2D` cria nuvens regulares determinísticas.
- `NeighborSearchGrid` faz busca não periódica por raio e retorna IDs ordenados.
- `PeriodicBoundary2D` fornece `wrap(Vec2)` e `minimum_image(Vec2)`.

**Fase C (MLS) concluída para a base linear inicial:**

- `WeightFunction` quártica implementada e testada.
- `PolynomialBasis` linear 2D implementada e testada.
- `MLSShapeFunction::evaluate` implementada (`mls_evaluate`).
  - Partição da unidade validada: $\sum\phi_i(\mathbf{x}) = 1$.
  - Reprodução linear validada: $\sum\phi_i(\mathbf{x})\,\mathbf{x}_i = \mathbf{x}$.
  - Gradientes de PU e LR validados com tolerância $10^{-10}$.
  - Raio de suporte inválido rejeitado por `std::invalid_argument`.
  - Ausência de `NaN`/`Inf` e condicionamento de $A$ testados em `mls_robustness`.
- CTest: **14/14 testes passando**.

**Fase D (Poisson EFG) validada e Marco 3 fechado (2026-05-09):**

- `GaussCell2D` gera células retangulares com quadratura Gauss 2×2.
- `MLSConfig` centraliza o raio de suporte usado por EFG.
- `EFGPoissonSolver` monta `K`, monta `b`, impõe Dirichlet por penalidade (DEC-0024) e resolve com backend esparso via `Eigen::SimplicialLDLT`.
- `stiffness_matrix()` retorna uma cópia densa apenas para testes e diagnóstico.
- Teste MMS Dirichlet para $u = \sin(\pi x)\sin(\pi y)$ passou:
  - potencial L2 5×5 = 0.00359684;
  - potencial L2 9×9 = 0.000827504;
  - potencial L∞ 5×5 = 0.0069852;
  - potencial L∞ 9×9 = 0.00169752;
  - campo L2 5×5 = 0.033052;
  - campo L2 9×9 = 0.0136451;
  - campo L∞ 5×5 = 0.105394;
  - campo L∞ 9×9 = 0.0554929.

**Deposição conservativa de carga (T-037, 2026-05-09):**

- `deposit_charge` em `include/pidc/pidc/ChargeDeposition.hpp` (DEC-0026 aceita).
- Itera partículas → `mls_evaluate` → acumula `q_p * phi_i(xp)`.
- Conservação `Σ Q_i = Σ q_p` garantida pela PU; verificada com tol 1e-12.
- `test_charge_conservation`: 5 subtestes; 15/15 CTest passando.

**Deposição CIC 1D implementada e testada (T-039, 2026-05-09):**

- `deposit_charge_cic_1d` em `include/pidc/pic/ChargeDeposition1D.hpp` (header-only, sem Eigen, DEC-0028).
- Algoritmo CIC: `Q[left] += q*(1-f)`, `Q[right] += q*f`, onde `f = fraction_in_cell(x_p)`.
- Conservação `|ΣQ_i − Σq_p| < 1e-12` garantida pela partição da unidade dos pesos CIC.
- `test_cic_deposition_1d`: 6 subtestes (centro 50/50, 100% sobre nó, periodicidade, conservação, tamanho, throw).
- `test_cic_deposition_reference_1d`: casos de referência com partícula central e conjunto determinístico de semente fixa (T-039B).
- Plano `docs/ai/PHASE_E_PIC1D_PLAN.md` criado e aprovado.
- CTest após T-039: **17/17 testes passando**; após T-039B e tarefas seguintes, suíte atual **23/23**.

**Campo manufaturado 1D implementado e testado (T-040, 2026-05-09):**

- `SineManufacturedField1D` em `include/pidc/pic/ManufacturedField1D.hpp` (header-only, sem Eigen).
- Fórmulas:
  - $\phi(x) = A\sin(2\pi x/L)$;
  - $E(x) = -A(2\pi/L)\cos(2\pi x/L)$;
  - $\rho(x) = \epsilon_0 A(2\pi/L)^2\sin(2\pi x/L)$.
- Funções de amostragem nodal: `sample_potential`, `sample_electric_field`, `sample_charge_density`.
- `test_manufactured_field_1d` cobre valores analíticos, periodicidade, tamanhos dos vetores e soma de `rho` aproximadamente zero.
- Na T-040 não houve implementação de Poisson 1D, interpolação campo→partícula, leap-frog ou Langmuir.
- CTest: **18/18 testes passando**.

**Poisson 1D periódico implementado e testado (T-041, 2026-05-09):**

- `PoissonResult1D` e `solve_poisson_periodic_1d` em `include/pidc/pic/PoissonSolver1D.hpp`.
- Resolve $\partial^2\phi/\partial x^2 = -\rho/\epsilon_0$ em `Grid1D` periódico.
- Backend: DFT manual com `std::complex<double>` e autovalores do operador de diferenças finitas periódico.
- Gauge: potencial médio zero (`phi_hat[0] = 0` e remoção do resíduo médio numérico).
- Campo nodal: diferença central periódica `E_i = -(phi_{i+1} - phi_{i-1})/(2 dx)`.
- `test_poisson_solver_1d` valida MMS periódico contra `SineManufacturedField1D`, gauge, densidade zero e falhas de entrada.
- Não houve implementação de leap-frog ou Langmuir.
- CTest: **19/19 testes passando**.

**Interpolação CIC campo→partícula implementada e testada (T-042, 2026-05-09):**

- `interpolate_field_cic_1d` em `include/pidc/pic/FieldInterpolation1D.hpp` (header-only, sem Eigen).
- Sobrecargas:
  - posição escalar `double x`;
  - vetor de `pidc::Particle`, usando apenas `particle.position.x`.
- Regra CIC periódica: `E_p = E_left*(1-f) + E_right*f`, com índices/fração vindos de `Grid1D`.
- `test_field_interpolation_1d` cobre casos exatos, wrap periódico, campo uniforme, campo manufaturado senoidal e falhas de entrada.
- Não houve implementação de leap-frog, Langmuir, MLS, EFG ou PIDC.
- CTest: **20/20 testes passando**.

**Leap-frog 1D isolado implementado e testado (T-043, 2026-05-09):**

- `initialize_leapfrog_velocity_1d` e `leapfrog_advance_1d` em `include/pidc/pic/LeapFrog1D.hpp` (header-only, sem Eigen).
- Convenção:
  - antes da inicialização, `Particle::velocity.x` representa `v(t=0)`;
  - após a inicialização, representa `v(t=-dt/2)`;
  - após cada avanço, continua sendo velocidade de meio passo.
- Atualização: `v_half += (q/m)E dt`; `x = grid.wrap_position(x + v_half dt)`.
- `test_leapfrog_1d` cobre movimento uniforme, aceleração constante, reversibilidade drift-only, wrap periódico e falhas de entrada.
- Não houve implementação de Langmuir, MLS, EFG ou PIDC.
- CTest: **21/21 testes passando**.

**Contorno periódico PIC 1D em movimento validado (T-043B, 2026-05-09):**

- `tests/test_pic_periodic_motion_1d.cpp` usa `Grid1D` + `LeapFrog1D` com campo nulo.
- Valida cruzamentos pela direita e pela esquerda, imagem exata `xmax -> xmin`, permanência em `[xmin,xmax)`, conservação do número de partículas e posições analíticas embrulhadas após múltiplos passos.
- Não houve implementação de Langmuir, MLS, EFG ou PIDC.
- CTest: **22/22 testes passando**.

**Oscilação de Langmuir 1D implementada e testada (T-044, 2026-05-09):**

- `run_langmuir_1d` em `include/pidc/pic/Langmuir1D.hpp` (header-only, sem Eigen).
- Integra a cadeia completa PIC 1D: deposição CIC → Poisson periódico (DFT) → interpolação CIC → leap-frog.
- Unidades normalizadas: ε₀=1, q_mac=-L/N, m_mac=L/N, q/m=-1 → ω_pe=1.
- Perturbação modo-1 senoidal (A=1e-3); leapfrog inicializado por meio passo antes do laço.
- Diagnóstico: série temporal Re[Ê_{k=1}] → DFT O(n²/2) → ω_obs = 1.005 ∈ [0.8, 1.2]·ω_pe.
- `test_langmuir_1d` cobre: T1 frequência, T2 deriva de energia, T3 conservação de carga, T4 finitude.
- **Fase E concluída. CTest: 24/24 testes passando.**

**PIC baseline 1D — grade criada e auditada (T-038A/B/C, 2026-05-09):**

- `pic::Grid1D` em `include/pidc/pic/Grid1D.hpp` (namespace `pidc::pic`).
- Semântica periódica semiaberta `[xmin, xmax)`, `nx` nós = `nx` células, `dx = L/nx` (DEC-0027 aceita).
- Métodos: `size`, `coordinate`, `wrap_position`, `cell_index`, `left_node_index`, `right_node_index`, `fraction_in_cell`.
- `test_pic_grid1d` passa; após T-039B, a suíte atual está em 23/23 CTest passando.
- **Fronteiras arquiteturais definidas (T-038C):** módulo `pic/` independente de `mls/`, `efg/` e Eigen.
  - Vetores nodais PIC 1D: `std::vector<double>` (DEC-0028).
  - Reutilizar `Particle`/`Species` existentes, usando apenas componente `.x`.
  - `PoissonSolver1D` será separado do `EFGPoissonSolver` (diferentes algoritmos/dimensões).
  - Comparação PIC vs PIDC adiada para depois das validações individuais.
- Riscos R-018–R-021 registrados.
- Sequência correta de implementação: T-039 (CIC) → T-040 (campo manufaturado) → T-041 (Poisson1D) → T-042 (interpolação) → T-043 (leap-frog) → T-043B (contorno periódico) → T-044 (Langmuir).

**Infra:**

- CMake/C++17 funcional; Eigen3 integrado via `find_package`.
- `tests/test_utils.hpp` com `pidc::test::require` e `pidc::test::approx_equal`.
- `scripts/build.sh` e `scripts/run_tests.sh` existem.
- CTest atual: **24/24 testes passando**.

## Próximos passos

| Tarefa | Descrição | Responsável | Prioridade |
| --- | --- | --- | --- |
| T-045 | Iniciar Fase F — PIDC completo: resolver R-017 (cache LDLT), revisar R-015/R-016/DEC-0022 | Professor / Claude | proposta |

**Pendências antes de avançar para PIDC (Fase F):**

- Fase E concluída (T-044 ✓).
- R-017: cache LDLT em `EFGPoissonSolver` — resolver antes de Phase F.
- R-015, R-016, DEC-0022: periodicidade MLS/busca — bloqueantes para Phase F.

## Decisões-chave vigentes

| DEC | Título | Status |
| --- | --- | --- |
| DEC-0005 | Unidades SI; Poisson = ∇²u = −ρ/ε₀ | aceita |
| DEC-0006 | Formulação variacional de Galerkin para EFG | aceita |
| DEC-0009 | `ShapeFunctionData` como contrato MLS/EFG/PIDC | aceita |
| DEC-0012 | `NodeCloud` como proprietário canônico de nós | aceita |
| DEC-0013 | Função peso: spline quártica | aceita (hipótese) |
| DEC-0014 | Base polinomial linear 2D, m=3 | aceita |
| DEC-0015 | Eigen via `find_package(Eigen3 3.3 REQUIRED)` | aceita |
| DEC-0016 | `mls_evaluate` como função livre, retorna `ShapeFunctionData` | aceita |
| DEC-0017 | Framework de testes: CTest + `test_utils.hpp` | aceita |
| DEC-0018 | Quadratura EFG: células retangulares Gauss 2×2 | aceita |
| DEC-0019 | Semântica de `Node::volume` | aceita |
| DEC-0020 | `NeighborSearchGrid` v1 não periódico | aceita |
| DEC-0021 | `PeriodicBoundary2D` como helper geométrico | aceita |
| DEC-0023 | Raio de suporte MMS inicial: `1.8*h_g` via `MLSConfig` | aceita |
| DEC-0024 | Penalidade para Dirichlet no EFG Poisson | aceita |
| DEC-0025 | Migração do backend de K: denso → esparso | aceita |
| DEC-0026 | `deposit_charge`: função livre para deposição conservativa de carga | aceita |
| DEC-0027 | Grade periódica 1D: `nx` nós = `nx` células, `dx = L/nx` | aceita |
| DEC-0028 | Vetores nodais PIC 1D como `std::vector<double>`, sem Eigen | aceita |
| DEC-0029 | Validação separada para componentes do PIC 1D | proposta |
| DEC-0030 | Convenção para MMS 1D senoidal | proposta |

## Regras críticas

- Não avançar para o caso de 360 000 partículas antes das validações completas.
- Toda mudança em solver deve ter teste.
- Nunca declarar reprodução da tese sem evidência numérica ou visual.
