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