# TEST_REPORT — Relatório de testes

## 2026-05-08 — Codex — T-005

### Ambiente

- CMake com C++17.
- Testes executáveis simples registrados no CTest.
- Helper comum: `tests/test_utils.hpp`.

### Comandos

```bash
cmake -S . -B build
cmake --build build -j
./build/pidc_smoke
/usr/bin/ctest --test-dir build --output-on-failure
git diff --check
```

### Resultado

- Build passou.
- Smoke passou.
- CTest passou com 8/8 testes.
- `git diff --check` passou.

### Observações

`ctest` deve ser invocado como `/usr/bin/ctest` neste ambiente, pois já foi
observado que o wrapper local em `$HOME/.local/bin/ctest` pode falhar por
falta do módulo Python `cmake`.

## Template para futuras execuções

```markdown
## YYYY-MM-DD — agente

### Ambiente

### Commit/branch

### Comandos

### Resultado

### Falhas

### Observações
```
