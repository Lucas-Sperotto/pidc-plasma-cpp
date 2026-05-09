# Log de Tarefa: T-039 — Plano Fase E + Deposição CIC 1D

**Agente:** Claude
**Data:** 2026-05-09

## 1. Tarefa

Aprovação e registro do plano arquitetural da Fase E (PIC 1D de referência),
seguida da implementação de T-039: `deposit_charge_cic_1d`.

## 2. Arquivos alterados

- `docs/ai/PHASE_E_PIC1D_PLAN.md`: plano arquitetural completo (novo).
- `include/pidc/pic/ChargeDeposition1D.hpp`: implementação CIC (novo).
- `tests/test_cic_deposition_1d.cpp`: 6 subtestes (novo).
- `CMakeLists.txt`: `pidc_test_cic_deposition_1d` adicionado.
- `docs/ai/AI_BOARD.md`: T-039 concluída; T-040/T-041 na fila.
- `docs/ai/AI_STATUS.md`: histórico T-039; contagem de testes atualizada.
- `docs/ai/CURRENT_CONTEXT.md`: seção CIC 1D adicionada; CTest 17/17.

## 3. Comandos executados

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)
ctest --output-on-failure
# Resultado: 17/17 testes passando
```

## 4. Implementação

### ChargeDeposition1D.hpp

Header-only. Sem Eigen (DEC-0028). Depende apenas de `Grid1D`, `Particle`, `Species`.

Algoritmo CIC para cada partícula `p`:
```
f     = grid.fraction_in_cell(p.position.x)
left  = grid.left_node_index(p.position.x)
right = grid.right_node_index(p.position.x)   // = (left+1) % nx
Q[left]  += q * (1.0 - f)
Q[right] += q * f
```

Conservação: `(1-f) + f = 1` → `ΣQ_i = Σq_p` exatamente.

### Testes (test_cic_deposition_1d.cpp)

| # | Condição | Verificação |
|---|----------|-------------|
| 1 | x = 0.125 (centro célula 0) | Q[0]=Q[1]=−0.5, Q[2]=Q[3]=0 |
| 2 | x = 0.25 (nó 1 exato, f=0) | Q[1]=−1.0; demais=0 |
| 3 | x = 0.875 (meio última célula) | Q[3]=−0.5; Q[0]=−0.5 (periodicidade) |
| 4 | 10 partículas aleatórias | |ΣQ_i − Σq_p| < 1e-12 |
| 5 | lista vazia | vetor de tamanho grid.size() |
| 6 | species_id = 99 | lança `std::out_of_range` |

## 5. Próximo passo sugerido

**T-040 (Codex):** implementar `PoissonSolver1D` periódico em
`include/pidc/pic/PoissonSolver1D.hpp`:

- DFT manual via `std::complex<double>`: O(n²), sem dependência externa.
- Equação: `∇²φ = -ρ/ε₀` periódica; gauge `φ̂_0 = 0`.
- Autovalores FD: `μ_k = 2·(cos(2πk/n) − 1) / dx²`.
- Campo: `E_i = −(φ_{i+1} − φ_{i-1}) / (2·dx)` (centrado periódico).
- Teste MMS: `ρ = ε₀·(2π/L)²·sin(2πx/L)` → `φ_exact = sin(2πx/L)`.
- Tolerâncias: `||φ − φ_exact||_∞ < 5e-3` e `||E − E_exact||_∞ < 5e-3` para nx=32.
- CTest esperado após T-040: 18/18.

Ver `docs/ai/PHASE_E_PIC1D_PLAN.md` para a especificação completa.
