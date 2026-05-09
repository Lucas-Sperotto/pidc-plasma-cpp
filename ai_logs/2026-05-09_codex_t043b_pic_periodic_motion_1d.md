# Log — Codex — T-043B Periodic Motion 1D

Data: 2026-05-09

## Tarefa

Validar o contorno periódico PIC 1D em movimento, sem avançar para Langmuir, PIDC, MLS, EFG ou caso grande da tese.

## Arquivos alterados

- `tests/test_pic_periodic_motion_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t043b_pic_periodic_motion_1d.md`

## Implementação

- Adicionado teste dedicado `pic_periodic_motion_1d`.
- O teste usa apenas `Grid1D` e `LeapFrog1D`, com campo elétrico nulo.
- Não foi criado novo código de produção.

## Testes adicionados

- Cruzamento pela direita.
- Cruzamento pela esquerda.
- Imagem exata `xmax -> xmin`.
- Movimento de múltiplas partículas por múltiplos passos.
- Conservação do número de partículas.
- Posições finais contra `grid.wrap_position(x0 + v*t)`.
- Velocidades constantes em campo nulo.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

Resultado final: 22/22 testes passando; `git diff --check` sem saída.

## Hipóteses assumidas

- A validação de contorno periódico da Fase E pode ser feita como teste de movimento com campo nulo, sem introduzir ainda a física de Langmuir.
- `Grid1D` continua sendo a fonte canônica da semântica periódica semiaberta `[xmin,xmax)`.

## Pendências

- Próxima tarefa sugerida: T-044 — oscilação de Langmuir 1D.
