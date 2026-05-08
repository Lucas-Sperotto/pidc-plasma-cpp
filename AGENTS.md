# AGENTS.md — Contrato de cooperação entre IAs

Este repositório será desenvolvido com apoio de **Gemini**, **Codex** e **Claude**. Este arquivo é a instrução compartilhada central.

## 1. Missão

Reconstruir em C/C++ o método **Particle-In-Diffuse-Cell (PIDC)** para simulação eletrostática de plasmas, com validação matemática, física e computacional.

A prioridade não é “fazer rodar rápido” de início. A prioridade é:

1. entender;
2. validar;
3. reproduzir;
4. otimizar.

## 2. Fontes primárias

As fontes primárias são:

- tese: *O Método Particle-In-Diffuse-Cell: uma abordagem Meshfree para simulação de plasmas*, Gleber Nelson Marques, INPE, 2008;
- publicação: *Uma abordagem meshfree para simulação de plasmas não-colisionais*, CMNE/CILAMCE 2007.

Nenhuma IA deve inventar detalhes do método sem registrar como hipótese.

## 3. Arquivos que toda IA deve ler antes de trabalhar

Antes de alterar qualquer arquivo, leia:

1. `README.md`
2. `TODO.md`
3. `ROADMAP.md`
4. `docs/00_visao_geral.md`
5. `docs/01_mapa_tese_para_codigo.md`
6. `docs/ai/CURRENT_CONTEXT.md`
7. `docs/ai/AI_BOARD.md`
8. `docs/ai/DECISIONS.md`

## 4. Regra de comunicação entre IAs

As IAs não conversam diretamente em tempo real. Elas conversam pelo repositório.

Arquivos de comunicação:

- `docs/ai/AI_BOARD.md`: quadro vivo de tarefas;
- `docs/ai/AI_STATUS.md`: estado técnico atual;
- `docs/ai/DECISIONS.md`: decisões técnicas aceitas ou propostas;
- `docs/ai/CURRENT_CONTEXT.md`: contexto mínimo para retomar o projeto;
- `ai_logs/`: registro individual de cada intervenção.

## 5. Protocolo antes de iniciar uma tarefa

Antes de trabalhar:

1. Leia os arquivos centrais.
2. Escolha uma tarefa pequena do `TODO.md`.
3. Registre a intenção em `docs/ai/AI_BOARD.md`.
4. Não edite simultaneamente arquivos que outra IA marcou como “em andamento”.
5. Se houver ambiguidade científica, registre em `docs/ai/OPEN_QUESTIONS.md`.

## 6. Protocolo ao concluir uma tarefa

Ao concluir:

1. Atualize `docs/ai/AI_STATUS.md`.
2. Atualize o item correspondente em `TODO.md`, se realmente concluído.
3. Crie um log em `ai_logs/YYYY-MM-DD_<agente>_<tarefa>.md`.
4. Informe:
   - arquivos alterados;
   - comandos executados;
   - testes executados;
   - resultados;
   - hipóteses assumidas;
   - pendências;
   - próximo passo sugerido.

## 7. Papéis sugeridos

### Codex

Foco principal:

- implementação C/C++;
- CMake;
- testes;
- scripts;
- organização de código;
- refatoração incremental.

Não deve alterar a formulação matemática sem registrar decisão.

### Gemini

Foco principal:

- auditoria matemática;
- documentação;
- checagem de equações;
- comparação tese ↔ código;
- revisão de coerência física;
- leitura ampla de contexto.

Não deve fazer grandes reescritas de código sem testes.

### Claude

Foco principal:

- revisão crítica;
- arquitetura;
- riscos;
- refatoração;
- clareza de interfaces;
- detecção de acoplamentos frágeis;
- planejamento de validação.

Não deve substituir código validado sem apresentar motivo e teste equivalente.

### ChatGPT / Professor

Foco principal:

- arbitrar decisões científicas;
- priorizar tarefas;
- revisar resultados;
- integrar contribuições;
- manter a linha metodológica.

## 8. Regras de implementação

- C++17.
- Código claro antes de código esperto.
- Começar denso e pequeno; migrar para esparso depois.
- Todo resultado numérico relevante deve ser exportável para CSV.
- Toda figura deve ser reproduzível por script.
- Toda alteração em solver deve vir acompanhada de teste.
- Não aceitar `NaN`, `Inf` ou condicionamento ruim sem diagnóstico.
- Não otimizar antes de validar.

## 9. Regras de documentação

- Matemática em Markdown com `$...$` e `$$...$$`.
- Sempre mapear equação → módulo → teste.
- Registrar hipóteses.
- Separar “validado” de “experimental”.
- Nunca declarar reprodução da tese sem evidência numérica ou visual.

## 10. Política para decisões técnicas

Use `docs/ai/DECISIONS.md`.

Formato:

```markdown
## DEC-000X — Título

Status: proposta | aceita | rejeitada | substituída

Contexto:
Decisão:
Justificativa:
Impacto no código:
Impacto na validação:
```

## 11. Política de branches

Sugestões:

```text
feat/mls-shape-functions
feat/efg-poisson-mms
feat/pidc-charge-deposition
test/charge-conservation
docs/thesis-map
audit/math-mls
```

## 12. Proibição importante

Não pular diretamente para o caso de 360.000 partículas.

O caso grande só pode vir depois de:

1. MLS validado;
2. Poisson validado;
3. deposição conservativa;
4. interpolação de campo testada;
5. leap-frog testado;
6. caso pequeno estável.
