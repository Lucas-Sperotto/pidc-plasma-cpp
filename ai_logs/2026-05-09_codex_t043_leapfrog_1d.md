# Log — Codex — T-043 LeapFrog1D

Data: 2026-05-09

## Tarefa

Implementar o leap-frog 1D isolado para a referência PIC eletrostática, sem avançar para Langmuir, PIDC, MLS ou EFG.

## Arquivos alterados

- `include/pidc/pic/LeapFrog1D.hpp`
- `tests/test_leapfrog_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t043_leapfrog_1d.md`

## Implementação

- Criada `initialize_leapfrog_velocity_1d`.
- Criada `leapfrog_advance_1d`.
- Convenção:
  - antes da inicialização, `Particle::velocity.x` representa `v(t=0)`;
  - depois da inicialização, representa `v(t=-dt/2)`;
  - após cada avanço, segue representando velocidade de meio passo.
- Atualização:
  - `v_half += (q/m) E dt`;
  - `x = grid.wrap_position(x + v_half dt)`.
- Entradas não finitas são rejeitadas com `std::invalid_argument`.

## Testes adicionados

- Campo nulo e movimento uniforme.
- Campo constante contra solução analítica de aceleração constante.
- Reversibilidade drift-only com `dt` e `-dt`.
- Wrap periódico ao cruzar `xmax`.
- Falhas para `dt` e campo elétrico não finitos.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

Resultado final: 21/21 testes passando; `git diff --check` sem saída.

## Hipóteses assumidas

- A referência PIC 1D usa apenas `Particle::position.x` e `Particle::velocity.x`.
- A reversibilidade com a API `leapfrog_advance_1d(..., dt)` foi validada no caso drift-only (`E = 0`), enquanto o caso de campo constante foi validado por solução analítica.
- A atualização periódica de posição pertence ao avanço isolado desta referência 1D.

## Pendências

- Próxima tarefa sugerida: T-044 — oscilação de Langmuir 1D.
