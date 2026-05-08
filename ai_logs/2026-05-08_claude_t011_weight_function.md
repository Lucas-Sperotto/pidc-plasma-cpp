# Log — 2026-05-08 — Claude — T-011: WeightFunction spline quártica (Fase C / MLS)

## Objetivo

Iniciar a Fase C (MLS) com a função peso compacta. Sem Eigen, sem matemática
de matriz — apenas a função $w(r)$ e sua derivada $w'(r)$ com testes exatos.

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/mls/WeightFunction.hpp` | criado (novo diretório `mls/`) |
| `tests/test_weight_function.cpp` | criado |
| `CMakeLists.txt` | adicionado executável e teste `weight_function` |
| `docs/ai/DECISIONS.md` | DEC-0013 adicionada |
| `docs/ai/AI_BOARD.md` | T-011 concluída; bloqueio de função peso resolvido |
| `docs/ai/AI_STATUS.md` | módulo `mls` e tabela de testes atualizados |

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Resultado dos testes

```
1/5 Test #1: periodic_boundary ................   Passed    0.00 sec
2/5 Test #2: shape_function_data ..............   Passed    0.00 sec
3/5 Test #3: species ..........................   Passed    0.00 sec
4/5 Test #4: node_cloud .......................   Passed    0.00 sec
5/5 Test #5: weight_function ..................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 5
```

## Hipóteses assumidas

- Spline quártica $w(r) = 1 - 6r^2 + 8r^3 - 3r^4$ adotada como função peso
  padrão (DEC-0013 com status "hipótese"). A tese de Marques precisa ser
  consultada para confirmar ou substituir.
- Funções livres inline em `namespace pidc` — sem struct ou classe, seguindo
  o estilo do projeto.

## Problemas encontrados

Nenhum.

## Próximo passo sugerido

T-012: base polinomial linear 2D — `p(x,y) = [1, x, y]` como
`std::array<double, 3>`. Header-only, sem Eigen, teste com valores exatos.

Depois de T-012: adicionar Eigen via FetchContent e implementar o núcleo de
`MLSShapeFunction::evaluate` (fase mais delicada — requer montagem de $A$,
resolução de sistema 3×3, e retorno de `ShapeFunctionData` válido).

O primeiro teste matemático real do MLS é a **partição da unidade**:
$\sum_i \phi_i(\mathbf{x}) = 1$ para qualquer $\mathbf{x}$ no interior da
nuvem.
