# PROMPTS — Prompts úteis para operar as IAs

Veja também `docs/08_prompt_inicial_para_cada_ia.md`.

## Prompt curto para retomar

```text
Leia AGENTS.md, TODO.md, ROADMAP.md, docs/ai/CURRENT_CONTEXT.md e docs/ai/AI_BOARD.md. Depois me diga qual é a menor próxima tarefa segura para avançar no projeto PIDC sem pular validações.
```

## Prompt para revisar uma implementação

```text
Revise os arquivos alterados nesta branch. Verifique se a mudança respeita AGENTS.md, TODO.md e DECISIONS.md. Aponte riscos matemáticos, riscos de arquitetura e testes faltantes. Não reescreva tudo; sugira correções pequenas.
```

## Prompt para gerar log

```text
Com base nas alterações feitas, crie um log seguindo docs/ai/HANDOFF_TEMPLATE.md. Inclua arquivos alterados, comandos executados, testes, hipóteses, problemas e próximo passo.
```

## Prompt para checar validação

```text
Leia docs/04_validacoes_referencia.md e compare com os testes existentes. Liste quais validações já estão cobertas, quais estão parcialmente cobertas e quais ainda faltam.
```
