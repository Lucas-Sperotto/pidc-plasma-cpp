# Log — 2026-05-08 — Claude — T-007: ShapeFunctionData (DEC-0009)

## Objetivo

Criar `ShapeFunctionData` como contrato de interface entre os módulos MLS, EFG e
PIDC, conforme DEC-0009. Nenhum cálculo matemático — apenas a struct e o teste
do invariante de tamanho.

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/ShapeFunctionData.hpp` | criado |
| `tests/test_shape_function_data.cpp` | criado |
| `CMakeLists.txt` | adicionado novo executável e teste CTest |
| `docs/ai/AI_BOARD.md` | T-007 movida para Concluídas |
| `docs/ai/AI_STATUS.md` | módulo mls e tabela de testes atualizados |
| `docs/ai/DECISIONS.md` | DEC-0009 marcada como aceita |

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Resultado dos testes

```
1/2 Test #1: periodic_boundary ................   Passed    0.00 sec
2/2 Test #2: shape_function_data ..............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2
```

## Hipóteses assumidas

- A struct permanece flat em `include/pidc/` (não cria subdiretório `mls/`)
  enquanto o módulo não crescer. Consistente com o estilo atual do projeto.
- `is_valid` como função livre (não método) — sem acesso a estado privado
  e fácil de usar em `assert()` nos módulos consumidores.
- O teste de partição da unidade ($\sum_i \phi_i = 1$) fica para quando
  `MLSShapeFunction::evaluate` for implementada — não é testável agora.

## Problemas encontrados

Nenhum.

## Decisões propostas / aceitas

- DEC-0009 marcada como aceita (estava como proposta).

## Próximo passo sugerido

Escolher entre:
- **T-008**: criar `Species.hpp` e refatorar `Particle` para usar `species_id`
  (sem math, sem Eigen — apenas mudança de struct e atualização do teste existente).
- **T-009**: criar `NodeCloud` como proprietário canônico da lista de nós
  (sem math — apenas uma classe wrapper com `size()`, `operator[]`, `nodes()`).

T-008 e T-009 são independentes e podem ser feitas em qualquer ordem.
T-009 é recomendada antes de começar MLS pois `MLSShapeFunction::evaluate`
precisará de `const NodeCloud&`.
