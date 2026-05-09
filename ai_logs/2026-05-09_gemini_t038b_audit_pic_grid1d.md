# Log de Tarefa: T-038B — Auditoria matemática do contrato PIC 1D

**Agente:** Gemini
**Data:** 2026-05-09

## 1. Tarefa

Auditar matematicamente o contrato da grade 1D periódica (`pic::Grid1D`) implementada em T-038A e propor o próximo passo para a Fase E (PIC de referência).

## 2. Arquivos alterados

- `docs/ai/DECISIONS.md`: Adicionada a `DEC-0027` para formalizar a convenção da grade periódica 1D.
- `docs/ai/AI_BOARD.md`: Marcada T-038B como concluída e adicionada a T-039.
- `docs/ai/AI_STATUS.md`: Adicionado o resumo da auditoria e do plano.
- `ai_logs/2026-05-09_gemini_t038b_audit_pic_grid1d.md`: Este arquivo.

## 3. Resultados da Auditoria

1.  **Semântica Periódica**: A convenção `[xmin, xmax)` com `nx` nós e `nx` células é a padrão e correta para PIC periódico.
2.  **Suficiência para CIC**: As informações da grade (índice da célula, posição fracionária) são suficientes para a deposição de carga linear (CIC).
3.  **Convenção**: A convenção `dx = L/nx` com nós em `xmin + i*dx` para `i=0..nx-1` é robusta e evita nós duplicados.

**Conclusão:** O contrato da grade 1D está matematicamente correto e pronto para ser usado na deposição de carga. A `DEC-0027` foi criada para registrar formalmente esta convenção.

## 4. Próximo passo sugerido (Instruções para Codex)

O agente **Codex** deve iniciar a tarefa `T-039`:
1.  Implemente a função `deposit_charge_cic_1d` em um novo header `include/pidc/pic/ChargeDeposition1D.hpp`. A função deve seguir a `DEC-0027`.
2.  Implemente o teste `tests/test_pic_charge_conservation_1d.cpp`.
3.  O teste deve incluir os seguintes casos:
    - **Partícula no centro da célula**: verificar se a carga é dividida igualmente (50/50) entre os dois nós vizinhos.
    - **Partícula sobre um nó**: verificar se 100% da carga é depositada naquele nó.
    - **Partícula na última célula (perto de `xmax`)**: verificar se a carga é corretamente depositada nos nós `nx-1` e `0`.
    - **Conservação global**: verificar se a soma de todas as cargas nodais é igual à soma de todas as cargas das partículas, para múltiplas partículas em posições arbitrárias.
4.  Adicione o novo teste ao `CMakeLists.txt` e verifique se todos os 17 testes passam.
5.  Atualize o `TODO.md` e o `AI_STATUS.md` para refletir a conclusão da tarefa.
6.  Mova a T-039 para "Concluídas" no `AI_BOARD.md`.
7.  Crie seu próprio log de conclusão.