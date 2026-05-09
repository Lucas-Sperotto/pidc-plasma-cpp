# Log de Tarefa: T-037 — Implementação de deposit_charge e teste de conservação

**Agente:** Claude
**Data:** 2026-05-09

## 1. Tarefa

Continuar T-037 a partir do plano do Gemini: revisar DEC-0026, implementar
`deposit_charge` em `include/pidc/pidc/ChargeDeposition.hpp` e criar
`tests/test_charge_conservation.cpp`.

## 2. Arquivos criados/alterados

- `include/pidc/pidc/ChargeDeposition.hpp`: função `deposit_charge` (novo, DEC-0026).
- `tests/test_charge_conservation.cpp`: 5 subtestes de conservação (novo).
- `CMakeLists.txt`: novo executável e teste `charge_conservation`.
- `docs/ai/DECISIONS.md`: DEC-0026 status `proposta` → `aceita`.
- `TODO.md`: conservação de carga marcada como concluída.
- `docs/ai/AI_BOARD.md`: T-037 movida para concluídas; T-038 na fila.
- `docs/ai/AI_STATUS.md`: entradas T-037 adicionadas ao histórico.
- `docs/ai/CURRENT_CONTEXT.md`: estado atualizado com T-037 e T-038.
- `ai_logs/2026-05-09_claude_charge-conservation-T037.md`: este arquivo.

## 3. Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Resultado: **15/15 testes passando** (0.04 s).

## 4. Decisões de implementação

### DEC-0026 — revisão e aceite

A interface proposta pelo Gemini está correta:

```cpp
Eigen::VectorXd deposit_charge(
    const NodeCloud& cloud,
    const std::vector<Particle>& particles,
    const std::vector<Species>& species_list,
    const MLSConfig& mls_config);
```

Única adição: `validate_mls_config` chamada no início para falha controlada
com `support_radius` inválido. Validação de `species_id` via bounds check
lança `std::out_of_range` para índice inválido.

### Por que a conservação é garantida

`sum_i phi_i(x) = 1` (PU, validada desde T-017). Logo:

```
sum_i Q_i = sum_p q_p * sum_i phi_i(x_p) = sum_p q_p * 1 = sum_p q_p
```

O teste é numérico (não apenas uma verificação de PU): ele confirma que
o loop de deposição acumula os valores nos índices corretos sem perda.

### Tolerância escolhida: 1e-12

O Gemini sugeriu 1e-14. Para 4 partículas com |q| ≤ 2, o erro acumulado
de PU (~1e-10 por partícula conforme testes MLS) pode ser da ordem de 4e-10.
Na prática, o erro observado nos testes foi ~1e-15 (bem abaixo de 1e-12 e 1e-14).
Escolhido 1e-12 como compromisso: exigente o suficiente para detectar erros
de lógica, robusto o suficiente para não ser frágil a variações de hardware.

## 5. Subtestes implementados

| Subteste | O que verifica |
| --- | --- |
| `test_global_conservation_multi_particle` | 4 partículas, 2 espécies, 5×5, `|ΣQ_i − Σq_p| < 1e-12` |
| `test_single_particle_conservation` | 1 partícula com q=3.0 em ponto assimétrico |
| `test_empty_particles_returns_zero` | lista vazia → vetor zero de tamanho 25 |
| `test_invalid_species_throws` | species_id=99 → `std::out_of_range` |
| `test_conservation_9x9` | 3 partículas em grade 9×9, independência da resolução |

## 6. Hipóteses assumidas

- `mls_evaluate` continua sendo busca linear; para Fase F com N grande,
  substituir por `NeighborSearchGrid` antes de integrar no ciclo PIDC.
- A função é stateless; o cache de `ShapeFunctionData` (reutilização entre
  deposição e interpolação) fica para quando o ciclo PIDC for implementado.

## 7. Riscos relevantes

- R-011: busca linear O(N) por partícula em `mls_evaluate` — aceitável para
  validação; deve ser resolvido antes do ciclo PIDC com N > 500.
- R-015/R-016/DEC-0022: `mls_evaluate` e `NeighborSearchGrid` não periódicos —
  bloqueante para Fase F, não para este teste.

## 8. Próximo passo sugerido

**T-038** (Codex): iniciar Fase E (PIC de referência):
- Deposição de carga em grade regular (nearest-grid-point ou CIC).
- Solver de Poisson por diferenças finitas ou FFT.
- Interpolação de campo elétrico.
- Leap-frog para avanço das partículas.
- Caso 1D: oscilação de Langmuir como critério de aceite.
