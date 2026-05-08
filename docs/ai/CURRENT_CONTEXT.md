# CURRENT_CONTEXT — Contexto mínimo para retomada

Atualizado em: 2026-05-08

Estamos reconstruindo em C/C++17 o método Particle-In-Diffuse-Cell (PIDC),
baseado na tese de Gleber Nelson Marques (INPE, 2008).

## Objetivo

Implementar e validar o PIDC de forma incremental:

1. MLS/EFG — **em progresso**;
2. Poisson EFG — próxima fase;
3. PIC baseline;
4. deposição PIDC;
5. interpolação PIDC;
6. leap-frog;
7. reprodução qualitativa da tese.

## Estado atual

**Fase B geométrica mínima concluída:**

- `Vec2`, `Particle`, `Species`, `Node` e `Domain2D` existem.
- `Domain2D` permanece geometria pura; contorno periódico básico via `wrapPeriodic(Vec2)`.
- `NodeCloud` é o proprietário canônico dos nós.
- `RegularNodeCloud2D` cria nuvens regulares determinísticas.
- `NeighborSearchGrid` faz busca não periódica por raio e retorna IDs ordenados.
- `PeriodicBoundary2D` fornece `wrap(Vec2)` e `minimum_image(Vec2)`.

**Fase C (MLS) concluída para a base linear inicial:**

- `WeightFunction` quártica implementada e testada.
- `PolynomialBasis` linear 2D implementada e testada.
- `MLSShapeFunction::evaluate` implementada (`mls_evaluate`).
  - Partição da unidade validada: $\sum\phi_i(\mathbf{x}) = 1$.
  - Reprodução linear validada: $\sum\phi_i(\mathbf{x})\,\mathbf{x}_i = \mathbf{x}$.
  - Gradientes de PU e LR validados com tolerância $10^{-10}$.
  - Raio de suporte inválido rejeitado por `std::invalid_argument`.
  - Ausência de `NaN`/`Inf` e condicionamento de $A$ testados em `mls_robustness`.
- CTest: **12/12 testes passando**.

**Infra:**

- CMake/C++17 funcional; Eigen3 integrado via `find_package`.
- `tests/test_utils.hpp` com `pidc::test::require` e `pidc::test::approx_equal`.
- `scripts/build.sh` e `scripts/run_tests.sh` existem.

## Próximos passos

| Tarefa | Responsável | Prioridade |
| --- | --- | --- |
| T-Poisson | Implementar assembler EFG Poisson (MMS) | Codex |

**Bloqueios antes de T-Poisson:**

- DEC-0018 está aceita: estratégia de quadratura — células retangulares Gauss 2×2.
- DEC-0019 está aceita: `Node::volume` foi removido.
- R-010: raio de suporte ainda precisa ser centralizado antes de usos extensos; T-Poisson deve introduzir `MLSConfig`.

## Decisões-chave vigentes

| DEC | Título | Status |
| --- | --- | --- |
| DEC-0005 | Unidades SI; Poisson = ∇²u = −ρ/ε₀ | aceita |
| DEC-0006 | Formulação variacional de Galerkin para EFG | proposta |
| DEC-0009 | `ShapeFunctionData` como contrato MLS/EFG/PIDC | aceita |
| DEC-0012 | `NodeCloud` como proprietário canônico de nós | aceita |
| DEC-0013 | Função peso: spline quártica | aceita (hipótese) |
| DEC-0014 | Base polinomial linear 2D, m=3 | aceita |
| DEC-0015 | Eigen via `find_package(Eigen3 3.3 REQUIRED)` | aceita |
| DEC-0016 | `mls_evaluate` como função livre, retorna `ShapeFunctionData` | aceita |
| DEC-0017 | Framework de testes: CTest + `test_utils.hpp` | aceita |
| DEC-0018 | Quadratura EFG: células retangulares Gauss 2×2 | aceita |
| DEC-0019 | Semântica de `Node::volume` | aceita |
| DEC-0020 | `NeighborSearchGrid` v1 não periódico | aceita |
| DEC-0021 | `PeriodicBoundary2D` como helper geométrico | aceita |

## Regras críticas

- Não avançar para o caso de 360 000 partículas antes das validações completas.
- Toda mudança em solver deve ter teste.
- Nunca declarar reprodução da tese sem evidência numérica ou visual.
