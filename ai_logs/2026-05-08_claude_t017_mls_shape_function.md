# Log — 2026-05-08 — Claude — T-017: MLSShapeFunction::evaluate (Fase C / MLS)

## Objetivo

Implementar a avaliação completa das funções de forma MLS: montar a matriz de
momento $A$, resolver $A\mathbf{c} = \mathbf{p}(\mathbf{x})$ e produzir
$\phi_i(\mathbf{x})$ e $\nabla\phi_i(\mathbf{x})$ para todos os vizinhos
dentro do suporte. Critério de aceite: partição da unidade e reprodução linear.

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/mls/MLSShapeFunction.hpp` | criado |
| `tests/test_mls_shape_function.cpp` | criado |
| `CMakeLists.txt` | adicionado executável e teste `mls_shape_function` |
| `docs/ai/AI_BOARD.md` | T-017 concluída |
| `docs/ai/AI_STATUS.md` | módulo `mls` e tabela de testes atualizados |
| `docs/ai/DECISIONS.md` | DEC-0016 adicionada |

## Comandos executados

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Resultado dos testes

```
1/8 periodic_boundary     Passed
2/8 shape_function_data   Passed
3/8 species               Passed
4/8 node_cloud            Passed
5/8 weight_function       Passed
6/8 polynomial_basis      Passed
7/8 eigen_dependency      Passed
8/8 mls_shape_function    Passed

100% tests passed, 0 tests failed out of 8
```

## Hipóteses assumidas

- Busca linear O(n) sobre todos os nós — suficiente para nuvens de validação.
  Estrutura espacial (k-d tree, cell lists) necessária para simulações grandes.
- `PartialPivLU` para A (3×3) — não exige simetria positiva definida, mais
  robusto que Cholesky para casos degenerados. Custo negligível para m=3.
- Suporte uniforme `h` para todos os nós — suporte variável virá quando
  necessário para adaptividade.
- Gradiente calculado por diferenciação implícita: $A\,\partial_k\mathbf{c} =
  \partial_k\mathbf{p} - \partial_k A\cdot\mathbf{c}$, reutilizando a mesma
  fatoração LU. Mais estável que diferença finita.

## Propriedades verificadas numericamente

Com nuvem regular 5×5, $h = 0.6$, ponto $(0.5, 0.5)$:

| Propriedade | Valor | Tolerância |
| --- | --- | --- |
| $\sum\phi_i$ | $1.0$ | $10^{-10}$ |
| $\sum\phi_i x_i$ | $0.5$ | $10^{-10}$ |
| $\sum\phi_i y_i$ | $0.5$ | $10^{-10}$ |
| $\sum\partial_x\phi_i$ | $0.0$ | $10^{-10}$ |
| $\sum\partial_y\phi_i$ | $0.0$ | $10^{-10}$ |
| $\sum\partial_x\phi_i\,x_i$ | $1.0$ | $10^{-10}$ |
| $\sum\partial_x\phi_i\,y_i$ | $0.0$ | $10^{-10}$ |
| $\sum\partial_y\phi_i\,x_i$ | $0.0$ | $10^{-10}$ |
| $\sum\partial_y\phi_i\,y_i$ | $1.0$ | $10^{-10}$ |

## Próximo passo sugerido

**T-018 (Codex)** — esqueleto do solver EFG Poisson:

- Estrutura de dados para matriz de rigidez densa (Eigen).
- Integração por pontos de Gauss sobre o domínio $[0,1]^2$.
- Montagem da RHS e condições de Dirichlet.
- Critério de aceite: teste MMS com $u(x,y) = \sin(\pi x)\sin(\pi y)$
  → $-\nabla^2 u = 2\pi^2 u$.
