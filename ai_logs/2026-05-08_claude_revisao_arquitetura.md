# Log — 2026-05-08 — Claude — Revisão crítica de arquitetura (T-003)

## Objetivo

Revisar criticamente a arquitetura planejada do repositório pidc-plasma-cpp
antes de qualquer implementação do núcleo matemático (MLS/EFG/PIDC).
Identificar acoplamentos frágeis, propor interfaces mínimas e congelar
decisões de design.

## Arquivos lidos

- AGENTS.md, CLAUDE.md, TODO.md, ROADMAP.md
- docs/02_arquitetura_repositorio.md
- docs/05_fluxo_de_trabalho_ias.md
- docs/07_riscos_tecnicos.md
- docs/ai/AI_BOARD.md, DECISIONS.md, RISKS.md, CURRENT_CONTEXT.md
- include/pidc/Vec2.hpp, Particle.hpp, Node.hpp, Domain2D.hpp
- src/Domain2D.cpp
- tests/test_periodic_boundary.cpp

## Arquivos alterados

- `docs/ai/RISKS.md` — adicionados R-004 a R-009
- `docs/ai/DECISIONS.md` — adicionadas DEC-0008 a DEC-0012
- `docs/ai/AI_BOARD.md` — T-003 movida para Concluídas; T-006 a T-009 propostas

## Comandos executados

Nenhum comando de build ou teste. Revisão puramente documental/arquitetural.

## Resultado dos testes

Não aplicável nesta intervenção.

## Hipóteses assumidas

Nenhuma hipótese matemática nova. Revisão baseada estritamente no código
existente e na documentação já presente no repositório.

## Riscos encontrados

| ID    | Descrição curta                                   | Severidade |
| ----- | ------------------------------------------------- | ---------- |
| R-004 | Domain2D depende de Particle (acoplamento concreto) | Alta — existe agora |
| R-005 | ShapeFunctionData sem struct definida             | Alta — bloqueia MLS/EFG/PIDC |
| R-006 | K/b sem separação no solver                       | Média — custo por passo |
| R-007 | charge/mass por instância de Particle             | Baixa — memória e inconsistência |
| R-008 | Posse do vetor de nós indefinida                  | Média — tempo de vida ambíguo |
| R-009 | Node::volume com semântica ambígua                | Média — afeta quadratura e deposição |

## Decisões propostas

| ID       | Resumo                                               | Dependência |
| -------- | ---------------------------------------------------- | ----------- |
| DEC-0008 | Remover applyPeriodic(Particle&) de Domain2D         | Nenhuma |
| DEC-0009 | Definir ShapeFunctionData antes de implementar MLS   | Nenhuma |
| DEC-0010 | EFGPoissonSolver: assemble() separado de solve()     | DEC-0009, DEC-0012 |
| DEC-0011 | Introduzir Species; Particle usa species_id          | Nenhuma |
| DEC-0012 | NodeCloud como proprietário canônico da lista de nós | Nenhuma |

## Pendências

- DEC-0008 a DEC-0012 ainda são propostas; requerem aceite do Professor/ChatGPT.
- R-009 (semântica de Node::volume) requer decisão antes da Fase C (MLS).
- T-002 (auditoria matemática por Gemini) ainda não iniciada — relevante antes
  de implementar MLS.

## Próximo passo sugerido

1. Professor/ChatGPT: arbitrar DEC-0008, DEC-0009, DEC-0011 e DEC-0012
   (as de menor risco e maior urgência).
2. Codex: implementar DEC-0008 (remoção de applyPeriodic) — mudança pequena,
   sem impacto matemático, já tem teste cobrindo o comportamento.
3. Codex: criar ShapeFunctionData.hpp (DEC-0009) como arquivo de cabeçalho
   vazio com a struct — sem lógica, apenas o contrato.
4. Gemini: auditar equações (T-002) em paralelo com as implementações acima.
