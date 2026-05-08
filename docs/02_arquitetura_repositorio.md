# 02 — Arquitetura planejada do repositório

## Diretórios

```text
include/pidc/
```

Cabeçalhos públicos do projeto.

```text
src/
```

Implementações C++.

```text
apps/
```

Executáveis de reprodução, validação e exploração.

```text
tests/
```

Testes unitários e de regressão.

```text
configs/
```

Arquivos JSON/YAML com parâmetros de simulação.

```text
scripts/
```

Scripts de build, execução, plotagem e validação.

```text
docs/
```

Documentação matemática, técnica e científica.

```text
data/input/
```

Entradas fixas.

```text
data/reference/
```

Soluções de referência, CSVs autorizados e dados digitizados.

```text
data/output/
```

Saídas geradas. Deve ficar fora do versionamento, exceto exemplos pequenos.

```text
ai_logs/
```

Registros das intervenções das IAs.

---

## Módulos C++ planejados

### `core`

- `Vec2`
- tipos numéricos;
- constantes físicas;
- utilidades.

### `particles`

- `Particle`
- `Species`
- inicialização de partículas;
- amostragem Maxwell-Boltzmann.

### `geometry`

- domínio 2D;
- contorno periódico;
- nuvem de nós;
- células de integração;
- busca de vizinhança.

### `mls`

- base polinomial;
- função peso;
- matriz MLS;
- funções de forma;
- gradientes.

### `efg`

- montagem Galerkin;
- quadratura;
- condições de contorno;
- solver de Poisson.

### `pic`

- deposição regular;
- interpolação regular;
- solver PIC de referência.

### `pidc`

- célula difusa;
- deposição PIDC;
- interpolação PIDC;
- ciclo temporal PIDC.

### `io`

- CSV;
- JSON;
- logs;
- metadados de simulação.

### `validation`

- métricas L2 e L∞;
- comparação entre arquivos;
- resumo de energia/carga/frequência.

---

## Dependências recomendadas

### Inicial

- C++17;
- CMake;
- Eigen.
- CTest com executáveis C++ simples e `tests/test_utils.hpp`.

### Depois

- nlohmann/json;
- Catch2 ou doctest, apenas se os testes simples ficarem insuficientes;
- OpenMP;
- FFTW;
- Gmsh.

## Estratégia de matrizes

### Fase inicial

Matriz densa para facilitar depuração.

### Fase validada

Migrar para `Eigen::SparseMatrix`.

### Fase grande

Estudar:

- pré-condicionadores;
- CG/GMRES;
- reuso de fatoração se `K` não muda;
- paralelização.
