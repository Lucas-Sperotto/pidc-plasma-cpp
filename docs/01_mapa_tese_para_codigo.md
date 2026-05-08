# 01 — Mapa tese/publicação para código

Este arquivo registra como a tese e a publicação devem ser transformadas em módulos de software.

## Capítulo 2 / Seção PIC eletrostático

### Conceitos

- partículas carregadas;
- densidade de carga;
- equação de Poisson;
- interpolação do campo;
- avanço temporal;
- contorno periódico;
- frequência plasma-elétron;
- comprimento de Debye.

### Código

```text
include/pidc/particles/
include/pidc/pic/
src/particles/
src/pic/
apps/run_pic_1d_langmuir.cpp
tests/test_periodic_boundary.cpp
tests/test_leapfrog.cpp
```

### Testes

- conservação de número de partículas;
- avanço temporal simples;
- oscilação de Langmuir;
- conservação de carga em deposição regular.

---

## Capítulo 3 / MEFG e MLS

### Conceitos

- base polinomial local;
- funções peso;
- domínios de influência;
- MLS;
- matriz `A`;
- funções de forma `phi_i`;
- gradientes das funções de forma;
- partição da unidade;
- condição `n >= m`;
- padrões patológicos de distribuição nodal.

### Código

```text
include/pidc/mls/
include/pidc/efg/
src/mls/
src/efg/
tests/test_partition_unity.cpp
tests/test_linear_reproduction.cpp
```

### Testes

- `sum(phi_i)=1`;
- reprodução de `u=a+bx+cy`;
- gradiente correto;
- diagnóstico de condicionamento.

---

## Capítulo 4 / PIDC

### Conceitos

- elemento difuso;
- célula difusa;
- domínios de influência como células funcionais;
- particionamento de carga por funções de forma;
- interpolação do campo nos mesmos nós da célula difusa.

### Código

```text
include/pidc/pidc/DiffuseCell.hpp
include/pidc/pidc/PIDCChargeDepositor.hpp
include/pidc/pidc/PIDCFieldInterpolator.hpp
include/pidc/pidc/PIDCLoop.hpp
src/pidc/
tests/test_charge_conservation_pidc.cpp
tests/test_field_interpolation_pidc.cpp
```

### Testes

- conservação de carga por partícula;
- conservação de carga global;
- mesma célula difusa usada em deposição e interpolação;
- falha controlada quando a célula difusa tem conectividade insuficiente.

---

## Capítulo 5 / Metodologias computacionais

### Conceitos

- uso de nuvem de pontos;
- Delaunay como fonte de pontos e células de integração;
- quadratura de Gauss;
- otimização de cálculo de funções de forma;
- reaproveitamento de derivadas;
- modelo orientado a objetos;
- paralelização futura.

### Código

```text
include/pidc/geometry/
include/pidc/integration/
include/pidc/io/
include/pidc/parallel/   # futuro
src/geometry/
src/integration/
src/io/
```

### Testes

- quadratura em domínio simples;
- montagem de matriz;
- leitura/escrita de CSV;
- comparação de resultados.

---

## Capítulo 6 / Resultados

### Conceitos

- análise de desempenho PIC-FEM;
- análise de campo elétrico em meios heterogêneos;
- análise qualitativa do campo autoconsistente do plasma.

### Código

```text
apps/reproduce_poisson_mms.cpp
apps/reproduce_thesis_small.cpp
apps/reproduce_thesis_medium.cpp
apps/reproduce_thesis_large.cpp
scripts/plot_field.py
scripts/plot_particles.py
scripts/plot_potential.py
```

### Saídas

```text
data/output/
data/reference/
docs/reproduction/
```

## Estratégia

Não reproduzir diretamente a Figura 6.32/6.33 da tese antes de validar:

1. MLS;
2. Poisson;
3. carga;
4. campo;
5. leap-frog;
6. contorno periódico.
