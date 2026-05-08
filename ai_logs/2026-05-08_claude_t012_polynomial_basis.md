# Log — 2026-05-08 — Claude — T-012: PolynomialBasis linear 2D (Fase C / MLS)

## Objetivo

Criar a base polinomial linear 2D como segundo ingrediente do MLS.
Sem Eigen, sem matemática de matriz — apenas as funções de avaliação e
derivadas da base p(x,y) = [1, x, y].

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/mls/PolynomialBasis.hpp` | criado |
| `tests/test_polynomial_basis.cpp` | criado |
| `CMakeLists.txt` | adicionado executável e teste `polynomial_basis` |
| `docs/ai/AI_BOARD.md` | T-012 concluída |
| `docs/ai/AI_STATUS.md` | módulo `mls` e tabela de testes atualizados |
| `docs/ai/DECISIONS.md` | DEC-0014 adicionada |

## Comandos executados

```
cmake -S . -B build && cmake --build build -j && ctest --test-dir build
```

## Resultado dos testes

```
1/6 periodic_boundary   Passed
2/6 shape_function_data Passed
3/6 species             Passed
4/6 node_cloud          Passed
5/6 weight_function     Passed
6/6 polynomial_basis    Passed

100% tests passed, 0 tests failed out of 6
```

## Hipóteses assumidas

- Base linear (m=3) adotada como padrão inicial — base quadrática (m=6) pode
  ser adicionada depois se necessário para reproduzir a tese.
- Interface `std::array<double, 3>` em vez de `Eigen::Vector3d` — Eigen não
  está disponível ainda. Conversão trivial quando MLSShapeFunction for criada.
- Derivadas expostas como funções sem argumentos — deixa explícito que são
  constantes para a base linear.

## Próximo passo sugerido

T-013: adicionar Eigen via FetchContent e implementar `MLSShapeFunction::evaluate`.

Critério de aceite de T-013: teste de partição da unidade deve passar:
$\sum_i \phi_i(\mathbf{x}) = 1$ para qualquer $\mathbf{x}$ no interior da nuvem.
