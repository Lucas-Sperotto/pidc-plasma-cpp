# T-044 — Oscilação de Langmuir 1D

**Agente:** Claude  
**Data:** 2026-05-09  
**Status:** concluída

---

## Objetivo

Implementar a simulação de oscilação de Langmuir 1D como Marco 4 da Fase E,
validando a cadeia completa PIC 1D (deposição CIC → Poisson periódico → interpolação
CIC → leap-frog) por meio do critério físico: frequência de plasma observada
ω_obs ∈ [0.8, 1.2]·ω_pe.

---

## Arquivos alterados

| Arquivo | Ação |
| --- | --- |
| `include/pidc/pic/Langmuir1D.hpp` | criado |
| `tests/test_langmuir_1d.cpp` | criado |
| `CMakeLists.txt` | adicionado target `pidc_test_langmuir_1d` |
| `docs/ai/AI_BOARD.md` | T-044 adicionado a Concluídas; T-045 em Próximas |
| `docs/ai/AI_STATUS.md` | entrada T-044 adicionada |
| `docs/ai/CURRENT_CONTEXT.md` | seção T-044 adicionada; CTest 24/24; próximos passos → T-045 |
| `TODO.md` | Langmuir marcada como `[x]` |

---

## Implementação

### `Langmuir1D.hpp`

Estruturas: `LangmuirConfig1D` (length, nx, particles, dt, steps, perturbation) e
`LangmuirResult1D` (omega_observed, relative_energy_drift, max_charge_error).

Função principal: `run_langmuir_1d(config)`.

**Unidades normalizadas:** ε₀=1, q_e=-1, m_e=1, n₀=1 → ω_pe=1.  
**Macropartícula:** q_mac=-L/N, m_mac=L/N, q_mac/m_mac=-1=q_over_m.

**Inicialização:**
- Posições uniformes com deslocamento senoidal modo-1: x_p = x0_p + A·sin(2πx0_p/L)
- Velocidades inicialmente zero
- Leapfrog inicializado: v(-dt/2) = v(0) - 0.5·(q/m)·E₀·dt

**Laço principal:**
1. `poisson_step()`: deposita carga CIC → adiciona fundo iônico ρ_ion=+1 → resolve Poisson
2. Registra `mode1_signal` = Re[Ê_{k=1}] = Σ_j E_j·cos(-2πj/nx)
3. Registra energia total = E_cinética + E_eletrostática
4. Verifica erro de carga a cada 50 passos
5. Avança posições e velocidades via `leapfrog_advance_1d`

**Extração de frequência:**
- DFT O(n²/2) da série temporal de `mode1_signal`
- Pico no bin k_peak → ω_obs = 2π·k_peak / (steps·dt)
- Usar Re[Ê_{k=1}] em vez de |Ê_{k=1}| evita dobramento de frequência

**Deriva de energia:**
- Descarta os primeiros 10% como transiente
- max |E_tot − E_mean| / |E_mean| sobre o restante

---

## Resultado dos testes

```
Running Langmuir 1D simulation (2500 steps)...
  omega_observed     = 1.005...
  energy_drift       = 0.024...
  max_charge_error   = 0.0
langmuir_1d test passed
```

- T1: ω_obs = 1.005 ∈ [0.8, 1.2]·ω_pe ✓
- T2: deriva de energia ≤ 0.25 ✓
- T3: erro de carga ≤ 1e-10 ✓
- T4: todos os valores são finitos ✓

Tempo de execução: ~0.41 s (dominado pelo DFT O(n²/2) com n=2500).

---

## Testes executados

```bash
cmake --build build -j && ctest --test-dir build --output-on-failure
```

**24/24 testes passando.**

---

## Hipóteses assumidas

- Leapfrog com meio passo de inicialização: v(-dt/2) = v(0) - 0.5·a·dt — convenção padrão Störmer-Verlet.
- Fundo iônico uniforme fixo ρ_ion = +1 em unidades normalizadas.
- Gauge do potencial: φ médio = 0 (já garantido por `PoissonSolver1D`).
- Critério de frequência ±20%: suficiente para validação de PIC em grade grosseira (nx=64, 128 partículas).

---

## Pendências

Nenhuma. Fase E concluída.

## Próximo passo sugerido

T-045: Iniciar Fase F (PIDC completo).  
Pré-requisitos: resolver R-017 (cache LDLT em EFGPoissonSolver) e revisar R-015/R-016/DEC-0022
(periodicidade MLS/busca de vizinhos).
