# Log — 2026-05-08 — Claude — T-019: Revisão de arquitetura MLS → EFG

## Objetivo

Revisar a arquitetura atual antes de implementar o assembler EFG Poisson.
Identificar riscos, acoplamentos frágeis e decisões pendentes que devem ser
resolvidas antes de avançar para a Fase D.

## Contexto verificado

- `MLSShapeFunction::evaluate` funcional (T-017, 8/8 testes passando).
- `test_utils.hpp` criado pelo Codex (T-005), todos os testes migrados.
- `CURRENT_CONTEXT.md` estava desatualizado ("em fase de planejamento").
- `TODO.md` Fase C com itens concluídos ainda desmarcados.

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `docs/ai/RISKS.md` | adicionados R-010, R-011, R-012; atualização do R-009 |
| `docs/ai/DECISIONS.md` | DEC-0018 proposta (quadratura Gauss 2×2) |
| `docs/ai/CURRENT_CONTEXT.md` | reescrito para refletir o estado real |
| `TODO.md` | Fase C sincronizada com o que foi implementado |
| `docs/ai/AI_BOARD.md` | T-019 concluída; T-Poisson bloqueada |
| `docs/ai/AI_STATUS.md` | último resumo atualizado |

## Riscos identificados

### R-010 — Raio de suporte sem propriedade definida
`support_radius` é passado como `double` sem dono. Se assembler EFG e loop PIDC
calcularem valores diferentes, $A(\mathbf{x})$ será inconsistente entre montagem
de K e deposição. Mitigação: centralizar antes de implementar EFG.

### R-011 — Busca O(N) acumulada em EFG
`mls_evaluate` percorre todos os nós por ponto de Gauss. Para validação com
N=25: OK. Para N=1024 e ~4000 pontos de Gauss: ~4×10⁶ comparações por montagem.
Mitigação: aceitar para validação; `NeighborSearchGrid` antes de simulação.

### R-012 — Células de integração para EFG não definidas
EFG exige integração numérica de $K_{ij} = \int \nabla\phi_i \cdot \nabla\phi_j$.
Sem estrutura `GaussCell2D`, o assembler não pode ser implementado.
Mitigação: DEC-0018 deve ser aceita antes de T-Poisson.

### R-009 (atualização)
Semântica de `Node::volume` é BLOQUEANTE para EFG. A abordagem de células de
background (DEC-0018) desacopla a quadratura de `volume` — o campo pode ser
removido ou documentado como auxiliar sem uso em K.

## Decisão proposta

**DEC-0018**: quadratura por células retangulares de background com Gauss 2×2.
Para nuvem $N \times N$: $(N-1)^2$ células de lado $h_g = 1/(N-1)$.
Estrutura `GaussCell2D` com `origin`, `size` e vetor de `{point, weight}`.

## Bloqueante antes de T-Poisson

DEC-0018 (proposta) precisa ser aceita ou substituída pelo Professor/Gemini
antes de Codex implementar o assembler EFG.

## Próximo passo sugerido

**T-018 (Codex)** — `RegularNodeCloud2D`: gera nuvem regular $N \times N$ em
$[0,1]^2$ com teste determinístico. Pré-requisito para o teste MMS de Poisson.
