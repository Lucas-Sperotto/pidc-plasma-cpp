# Log — 2026-05-09 — Gemini — T-GEMINI-F-AUDIT-PIDC-RHS-DOMAINS

## Objetivo

Auditar a relação matemática entre o vetor fonte de Poisson (`rhs`) e as cargas nodais (`Q_i`), a definição dos domínios de influência e outras hipóteses que conectam a tese de referência à implementação atual do PIDC.

## Contexto lido

- [x] AGENTS.md
- [x] TODO.md
- [x] ROADMAP.md
- [x] docs/ai/CURRENT_CONTEXT.md
- [x] docs/ai/AI_BOARD.md
- [x] docs/ai/DECISIONS.md
- [x] docs/03_modelo_matematico_minimo.md

## Arquivos alterados

- `docs/ai/AI_BOARD.md`: Movi a tarefa para "em andamento" e depois para "concluída".
- `docs/ai/AI_STATUS.md`: Adicionei o resumo da auditoria.
- `docs/ai/DECISIONS.md`: Adicionei as propostas `DEC-0032` e `DEC-0033`.

## Comandos executados

Nenhum comando de compilação ou teste foi necessário para esta tarefa de auditoria.

## Resultado dos testes

N/A.

## Hipóteses assumidas

1.  A forma fraca de Galerkin (`DEC-0006`) é a base para derivar o sistema linear.
2.  A equação de Poisson na forma SI (`DEC-0005`) é `∇²u = -ρ/ε₀`.
3.  A densidade de carga `ρ` pode ser representada como uma soma de funções delta de Dirac nas posições das partículas.

## Problemas encontrados

Nenhum problema ou bug foi encontrado. A auditoria revelou que a lógica implementada é consistente com a teoria, mas que as relações matemáticas chave não estavam formalizadas como decisões, criando um risco de ambiguidade futura.

## Decisões propostas

- **DEC-0032:** Formaliza a relação `b_i = Q_i / ε₀` para o vetor fonte de Poisson.
- **DEC-0033:** Formaliza o uso de um domínio de influência circular, definido por `support_radius`, como o padrão para o ciclo PIDC.

## Próximo passo sugerido

Claude deve iniciar a tarefa `T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP` para revisar a arquitetura do ciclo PIDC, agora que as fases de implementação inicial estão concluídas. Em paralelo, Gemini e Claude podem começar a definir o caso de comparação para o Marco 6 (`T-046`).