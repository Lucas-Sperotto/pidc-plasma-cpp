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

**Fase C (MLS) concluída:**

- `WeightFunction` quártica implementada e testada.
- `PolynomialBasis` linear 2D implementada e testada.
- `MLSShapeFunction::evaluate` implementada (`mls_evaluate`).
  - Partição da unidade validada: $\sum\phi_i(\mathbf{x}) = 1$.
  - Reprodução linear validada: $\sum\phi_i(\mathbf{x})\,\mathbf{x}_i = \mathbf{x}$.
  - Gradientes de PU e LR validados com tolerância $10^{-10}$.
- CTest: **8/8 testes passando**.

**Infra:**

- CMake/C++17 funcional; Eigen3 integrado via `find_package`.
- `tests/test_utils.hpp` com `pidc::test::require` e `pidc::test::approx_equal`.

**Pendente (Fase B):**

- `RegularNodeCloud2D` (T-018, Codex).
- `NeighborSearchGrid` (antes de simulação em escala).

## Próximos passos

| Tarefa | Responsável | Prioridade |
| --- | --- | --- |
| T-018 | Implementar `RegularNodeCloud2D` | Codex |
| T-020 | Auditar matematicamente `MLSShapeFunction` e gradientes | Gemini |
| T-Poisson | Implementar assembler EFG Poisson (MMS) | Codex |

**Bloqueios antes de T-Poisson:**

- DEC-0018 (proposta): estratégia de quadratura — células retangulares Gauss 2×2.
- R-009: semântica de `Node::volume` precisa de decisão.
- R-010: raio de suporte precisa ser centralizado.

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
| DEC-0018 | Quadratura EFG: células retangulares Gauss 2×2 | proposta |

## Regras críticas

- Não avançar para o caso de 360 000 partículas antes das validações completas.
- Toda mudança em solver deve ter teste.
- Nunca declarar reprodução da tese sem evidência numérica ou visual.
