# Log — Codex — T-039B CIC Deposition Reference 1D

Data: 2026-05-09

## Tarefa

Adicionar casos de referência para deposição CIC 1D, sem alterar código de produção e sem avançar para Langmuir, PIDC, MLS, EFG ou caso grande da tese.

## Arquivos alterados

- `tests/test_cic_deposition_reference_1d.cpp`
- `CMakeLists.txt`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/CURRENT_CONTEXT.md`
- `docs/validation/VALIDATION_PLAN.md`
- `ai_logs/2026-05-09_codex_t039b_cic_deposition_reference_1d.md`

## Testes adicionados

- Partícula no centro de uma célula: deposição 50/50 nos dois nós vizinhos.
- Partícula no centro do domínio sobre um nó: deposição 100% no nó.
- Conjunto determinístico com semente fixa:
  - referência manual CIC;
  - reprodutibilidade;
  - conservação global de carga.

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

Resultado final: 23/23 testes passando; `git diff --check` sem saída.

## Hipóteses assumidas

- A validação de "partícula no centro" cobre tanto o centro de célula quanto o centro do domínio sobre nó.
- Para evitar variação entre implementações, o conjunto de semente fixa usa um gerador linear congruente pequeno definido no próprio teste, não `std::uniform_real_distribution`.

## Pendências

- Próxima tarefa sugerida: T-044 — oscilação de Langmuir 1D, em rodada separada.
