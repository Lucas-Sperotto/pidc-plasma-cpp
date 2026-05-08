# 07 — Riscos técnicos iniciais

## Risco 1 — Mistura de unidades

A tese/publicação usa expressões com convenções que podem sugerir sistema gaussiano em alguns pontos, enquanto o código moderno tende a SI.

### Mitigação

Criar `docs/math/UNITS.md` antes de avançar para simulação física grande.

## Risco 2 — Sinal da equação de Poisson

Há risco de trocar o sinal entre `rho`, `-rho` e `-4*pi*rho`.

### Mitigação

Validar com solução manufaturada e documentar a convenção.

## Risco 3 — Matriz MLS mal condicionada

Se `n < m` ou os nós forem colineares, a matriz MLS falha.

### Mitigação

Monitorar:

- cardinalidade;
- número de condição;
- falhas locais;
- distribuição geométrica.

## Risco 4 — Deposição não conservativa

Se `sum(phi)` não for 1 numericamente, a carga não será conservada.

### Mitigação

Testar conservação por partícula e global.

## Risco 5 — Poisson periódico

Poisson periódico exige cuidado com o modo médio/gauge.

### Mitigação

Começar com Dirichlet no MMS e só depois implementar periódico.

## Risco 6 — O caso grande mascarar erro

Com muitas partículas, gráficos bonitos podem esconder erros.

### Mitigação

Nunca aceitar figura qualitativa como validação isolada.

## Risco 7 — Custo computacional alto

EFG/MLS e montagem podem ser caros.

### Mitigação

Começar pequeno, validar, depois esparsidade, cache e paralelização.

## Risco 8 — IAs sobrescrevendo decisões

Várias IAs podem alterar o mesmo arquivo de status.

### Mitigação

Uso de logs individuais e seções datadas.
