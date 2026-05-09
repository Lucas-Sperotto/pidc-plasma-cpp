# Log de Tarefa: T-002 — Auditar equações mínimas e sinais

**Agente:** Gemini
**Data:** 2026-05-08

## 1. Tarefa

Auditar o arquivo `docs/03_modelo_matematico_minimo.md` para garantir consistência com as decisões técnicas já tomadas, especialmente `DEC-0005` (unidades SI) e `DEC-0007` (contorno periódico).

## 2. Arquivos alterados

- `docs/03_modelo_matematico_minimo.md`: Atualizado para refletir a adoção do sistema SI para a equação de Poisson e para sumarizar as decisões sobre contorno periódico.
- `docs/ai/AI_BOARD.md`: Movida a tarefa T-002 para "Concluídas".
- `docs/ai/AI_STATUS.md`: Adicionada nota sobre a auditoria matemática.

## 3. Comandos executados

Nenhum comando de compilação ou teste foi necessário, pois a tarefa foi puramente de documentação e auditoria.

## 4. Testes executados

Nenhum.

## 5. Resultados

O documento `docs/03_modelo_matematico_minimo.md` agora está alinhado com as decisões do projeto. A ambiguidade sobre a forma da equação de Poisson foi removida, e a seção "Pontos de atenção" foi atualizada para refletir o estado atual das decisões, servindo como um guia mais claro para a implementação.

## 6. Hipóteses assumidas

- As decisões em `DECISIONS.md` são o estado canônico do projeto.
- A tarefa de auditoria (`T-002`) visava alinhar a documentação com essas decisões.

## 7. Pendências

Nenhuma.

## 8. Próximo passo sugerido

Com a base matemática mínima esclarecida, uma boa próxima tarefa para o **Codex** seria adicionar a dependência da biblioteca Eigen ao projeto (`CMakeLists.txt`), conforme pendência da "Fase A" no `TODO.md`. Isso desbloqueará a implementação do núcleo MLS/EFG, que depende de álgebra linear.