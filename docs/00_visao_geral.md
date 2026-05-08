# 00 — Visão geral científica do projeto

## Problema

Modelos Particle-In-Cell simulam plasmas representando o sistema por macropartículas carregadas. O campo elétrico autoconsistente é obtido por uma equação de Poisson e depois interpolado de volta para as partículas.

O método PIDC surge quando se substitui a célula geométrica tradicional do PIC por uma **célula difusa**, definida pelos domínios de influência e pelas funções de forma do método Element-Free Galerkin.

## Motivação

Em regiões com forte acúmulo de carga, o PIC baseado em malha pode exigir remalhamento local. Além disso, aproximações de primeira ordem podem produzir campo elétrico constante por célula, o que é frágil em regiões estreitas com forte variação de campo.

O EFG/MEFG oferece:

- construção meshfree;
- funções de forma suaves;
- alta continuidade;
- adaptatividade potencialmente mais simples;
- conexão natural com o conceito de elemento/célula difusa.

## Núcleo do método

O ciclo PIDC contém:

1. particionamento de carga nas células difusas;
2. resolução da equação de Poisson;
3. interpolação do campo elétrico na posição das partículas;
4. avanço temporal por leap-frog.

## Premissa do repositório

A tese apresentou a formulação e resultados preliminares/qualitativos. Este repositório busca transformar isso em uma implementação moderna e verificável.

## Não objetivo inicial

Não é objetivo inicial fazer uma simulação industrial completa de plasma.

O objetivo inicial é construir uma referência limpa:

- pequena;
- rastreável;
- documentada;
- testável;
- reprodutível.
