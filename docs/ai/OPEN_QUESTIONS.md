# OPEN_QUESTIONS — Questões em aberto

Este arquivo registra ambiguidades científicas, matemáticas ou de implementação que precisam ser resolvidas.

---

## Q-0001 — Sistema de unidades para constantes físicas

**Status:** aberto

**Contexto:** A equação de Poisson e as constantes físicas mudam de forma dependendo do sistema de unidades (e.g., SI vs. Gaussiano). A tese usa convenções que sugerem o sistema Gaussiano (`-4πρ`), enquanto código moderno geralmente prefere SI (`-ρ/ε₀`).

**Questão:** Qual sistema de unidades será o padrão para a implementação do núcleo físico do simulador?

**Proposta de Resolução:** Ver `DEC-0005`.

---

## Q-0002 — Gauge do potencial em contorno periódico

**Status:** aberto

**Contexto:** A equação de Poisson com condições de contorno periódicas tem uma solução para o potencial `u` que é definida a menos de uma constante. Para que o sistema linear `K u = b` tenha solução única, o potencial precisa ser "fixado" (gauge fixing).

**Questão:** Qual estratégia será usada para fixar o gauge do potencial? As opções comuns incluem: a) fixar o potencial em um nó (`u_k = 0`); b) exigir que a média do potencial seja nula (`sum(u_i) = 0`).

**Proposta de Resolução:** Ver `DEC-0007`.

---

## Q-0003 — Normalização do RHS PIDC a partir de carga nodal

**Status:** aberto

**Contexto:** O smoke PIDC 2D inicial usa `rhs = Q/epsilon0`, onde `Q` vem de
`deposit_charge_from_cells`. Para validar o fluxo do código isso é suficiente,
mas casos físicos podem exigir converter carga nodal em densidade/termo de fonte
compatível com a medida de integração EFG.

**Questão:** O RHS do Poisson PIDC deve usar `Q/epsilon0` diretamente ou
`Q/(epsilon0 * volume_nodal_equivalente)`?

**Proposta de Resolução:** Gemini deve auditar em
`docs/ai/GEMINI_TASKS/T_GEMINI_F_AUDIT_PIDC_RHS_DOMAINS.md` antes de comparação
PIC-PIDC ou reprodução física.
