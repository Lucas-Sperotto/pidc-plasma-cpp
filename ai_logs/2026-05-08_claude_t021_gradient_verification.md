# Log — 2026-05-08 — Claude — T-021: Verificação do gradiente MLS (R-013)

## Objetivo

Investigar R-013 (Gemini, T-020): suposto bug na fórmula do gradiente de
`mls_evaluate` — "termo extra `(∇p)ᵀ c wᵢ`". Corrigir se necessário; senão,
adicionar testes assimétricos que detectariam qualquer bug real.

## Análise matemática

A fórmula implementada deriva implicitamente `A c = p(x)`:

```text
A ∂c/∂xₖ = ∂p/∂xₖ − ∂A/∂xₖ c          [diferenciando A c = p(x)]
∂φᵢ/∂xₖ = (∂c/∂xₖ)ᵀ pᵢ wᵢ + cᵀ pᵢ ∂wᵢ/∂xₖ
```

Expandindo `(∂c/∂xₖ)ᵀ pᵢ`:

```text
= (A⁻¹(∂p/∂xₖ − ∂A/∂xₖ c))ᵀ pᵢ
= (∂p/∂xₖ)ᵀ A⁻¹ pᵢ − cᵀ ∂A/∂xₖ A⁻¹ pᵢ
```

Em código: `dc_dx = lu.solve(dpx − dA_dx * c)` → `dc_dx.dot(pki)` calcula
exatamente essa expressão via simetria de `A⁻¹` e `∂A/∂xₖ`.

**Conclusão:** A fórmula está correta. Nenhuma correção necessária.

## Causa raiz do R-013

Gemini identificou o termo `(∂p/∂x)ᵀ A⁻¹ pᵢ wᵢ` como "extra" e o confundiu
com `(∂p/∂x)ᵀ c wᵢ = (∂p/∂x)ᵀ A⁻¹ p(x) wᵢ`. Esses dois termos só coincidem
quando `pᵢ = p(x)`, i.e., quando o vizinho `i` está exatamente no ponto de
consulta. O primeiro é o componente correto da derivada implícita; o segundo
não faz parte da fórmula.

## Problema real resolvido: teste frágil

O teste original tinha gradientes verificados APENAS para o ponto simétrico
`{0.5, 0.5}`. Mesmo que a fórmula fosse correta, um teste assim não é robusto
como regra geral. T-021 corrige isso.

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `tests/test_mls_shape_function.cpp` | adicionados testes assimétricos completos |
| `docs/ai/RISKS.md` | R-013 fechado como falso positivo com análise |
| `docs/ai/AI_BOARD.md` | T-021 concluída; T-Poisson desbloqueada |
| `docs/ai/AI_STATUS.md` | resumo atualizado |

## Testes assimétricos adicionados

Lambda `check_all` verifica as 9 propriedades (PU, LR-x, LR-y, ∇PU-x, ∇PU-y,
∇LR-xx, ∇LR-xy, ∇LR-yx, ∇LR-yy) para cada ponto de consulta:

| Ponto | Simétrico? | PU | LR | Gradientes |
| --- | --- | --- | --- | --- |
| {0.5, 0.5} | sim | ✓ | ✓ | ✓ |
| {0.3, 0.7} | não | ✓ | ✓ | ✓ (novo) |
| {0.6, 0.4} | não | ✓ | ✓ | ✓ (novo) |
| {0.37, 0.61} | não | ✓ | ✓ | ✓ (novo) |

## Resultado

```
9/9 tests passed, 0 tests failed
```

## Próximo passo sugerido

**T-022 (Gemini)** — confirmar a resolução de R-013 e dar sinal verde para
T-Poisson. Além disso, **DEC-0018** (quadratura Gauss 2×2) precisa ser aceita
antes de Codex iniciar o assembler EFG.
