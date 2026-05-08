# OPEN_QUESTIONS — Perguntas abertas

## Q-001 — Qual sistema de unidades será adotado no código?

Opções:

1. SI;
2. CGS/Gaussiano;
3. sistema normalizado.

Impacto:
Afeta Poisson, constantes físicas, frequência e escala dos resultados.

---

## Q-002 — Como tratar Poisson com contorno periódico?

Opções:

1. remover modo médio;
2. fixar potencial médio zero;
3. fixar um nó como gauge;
4. usar solver especializado.

Impacto:
Afeta simulação de plasma com contorno periódico.

---

## Q-003 — Qual função peso inicial?

Opções:

1. Schwarz truncada singular/interpolante;
2. spline cúbica compacta;
3. função suave regular para depuração.

Impacto:
Afeta Kronecker, Dirichlet, condicionamento e continuidade.

---

## Q-004 — Começar com domínio de influência circular ou retangular?

Opções:

1. circular: mais natural fisicamente;
2. retangular: busca mais simples e próxima da tese/publicação.

Impacto:
Afeta célula difusa, busca de vizinhança e desempenho.

---

## Q-005 — Qual framework de testes?

Opções:

1. doctest;
2. Catch2;
3. testes simples com executáveis e asserts;
4. GoogleTest.

Impacto:
Afeta dependências e facilidade de uso.
