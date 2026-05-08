# 05 — Fluxo de trabalho entre Gemini, Codex e Claude

## Problema

As IAs não têm uma memória compartilhada confiável fora do repositório. Portanto, o repositório precisa funcionar como um quadro de comunicação.

## Arquivos centrais

### `AGENTS.md`

Contrato geral.

### `GEMINI.md`

Contexto específico do Gemini.

### `CLAUDE.md`

Contexto específico do Claude.

### `CODEX.md`

Contexto específico do Codex.

### `docs/ai/AI_BOARD.md`

Quadro de tarefas em andamento.

### `docs/ai/AI_STATUS.md`

Estado técnico consolidado.

### `docs/ai/DECISIONS.md`

Decisões técnicas.

### `docs/ai/CURRENT_CONTEXT.md`

Resumo curto para retomada.

### `ai_logs/`

Logs de cada intervenção.

---

## Procedimento padrão

### Antes

```text
1. Ler AGENTS.md.
2. Ler TODO.md.
3. Ler ROADMAP.md.
4. Ler CURRENT_CONTEXT.md.
5. Ler AI_BOARD.md.
6. Escolher tarefa pequena.
7. Registrar intenção no AI_BOARD.md.
```

### Durante

```text
1. Fazer uma mudança pequena.
2. Rodar teste.
3. Registrar hipótese se necessário.
4. Não esconder falhas.
```

### Depois

```text
1. Atualizar AI_STATUS.md.
2. Atualizar AI_BOARD.md.
3. Criar log em ai_logs/.
4. Propor próximo passo.
```

---

## Formato de log

```markdown
# Log — YYYY-MM-DD — agente — tarefa

## Objetivo

## Arquivos alterados

## Comandos executados

## Resultado dos testes

## Hipóteses assumidas

## Problemas encontrados

## Decisões propostas

## Próximo passo sugerido
```

---

## Regras de convivência

- Não apagar registro de outra IA.
- Preferir anexar nova seção datada.
- Não editar grandes arquivos centrais sem necessidade.
- Não marcar tarefa como concluída sem teste.
- Se discordar de uma decisão, criar uma proposta em `DECISIONS.md`.
- Não misturar refatoração com mudança matemática.
