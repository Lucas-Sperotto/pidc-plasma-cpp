# 06 — Plano da primeira semana de desenvolvimento

## Dia 1 — Bootstrap

Responsável sugerido: Codex

- Criar CMake funcional.
- Criar `Vec2`.
- Criar `Particle`.
- Criar `Node`.
- Criar `Domain2D`.
- Criar smoke test.
- Rodar build.

## Dia 2 — Busca de vizinhança

Responsável sugerido: Codex + Claude

- Implementar busca simples `O(N)`.
- Implementar versão grid/hash simples.
- Testar domínio de influência retangular.
- Revisar interface.

## Dia 3 — Função peso e MLS

Responsável sugerido: Codex

- Implementar função peso compacta.
- Implementar base `[1,x,y]`.
- Implementar cálculo de `phi`.
- Testar `sum(phi)=1`.

## Dia 4 — Auditoria matemática

Responsável sugerido: Gemini

- Conferir notação MLS.
- Conferir sinais.
- Conferir condição `n >= m`.
- Escrever relatório em `docs/math/`.

## Dia 5 — Gradientes

Responsável sugerido: Codex + Claude

- Implementar `grad_phi`.
- Testar função linear.
- Validar campo manufaturado.

## Dia 6 — Poisson MMS

Responsável sugerido: Codex

- Montar matriz densa.
- Impor Dirichlet.
- Resolver MMS pequeno.
- Exportar CSV.

## Dia 7 — Revisão

Responsável sugerido: todos

- Gemini audita equações.
- Claude audita arquitetura.
- Codex corrige build/testes.
- Professor decide próximo marco.
