# tests

Testes do projeto.

## Framework atual

O projeto usa executáveis C++ simples registrados no CTest. Cada arquivo
`tests/test_*.cpp` deve:

- incluir `tests/test_utils.hpp`;
- usar `pidc::test::require(...)` para asserções;
- usar `pidc::test::approx_equal(...)` para comparações de ponto flutuante;
- retornar `0` em sucesso;
- imprimir mensagens curtas de falha em `stderr`.

Frameworks externos como Catch2/doctest ficam adiados até haver necessidade
real de fixtures, parametrização ou relatórios mais ricos.

## Núcleo

```text
test_vec2.cpp
test_periodic_boundary.cpp
test_neighbor_search.cpp
```

## MLS/EFG

```text
test_partition_unity.cpp
test_constant_reproduction.cpp
test_linear_reproduction.cpp
test_gradient_linear.cpp
test_mls_conditioning.cpp
```

## PIDC

```text
test_charge_conservation_particle.cpp
test_charge_conservation_global.cpp
test_diffuse_cell_connectivity.cpp
test_field_interpolation.cpp
```

## Física

```text
test_leapfrog_simple.cpp
test_langmuir_frequency.cpp
```
