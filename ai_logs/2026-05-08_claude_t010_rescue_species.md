# Log — 2026-05-08 — Claude — T-010: Rescate de T-008 — Species e Particle (DEC-0011)

## Objetivo

T-008 estava marcada como "concluída por Gemini" no AI_BOARD e AI_STATUS, mas a
implementação real não existia. Esta intervenção completa T-008 corretamente
como T-010, conforme DEC-0011.

## Problema detectado antes de começar

| Sintoma | Causa |
| --- | --- |
| `Particle.hpp` ainda tinha `charge` e `mass` | DEC-0011 não aplicada |
| `Species.hpp` na raiz do repositório | arquivo no local errado, não rastreado |
| `test_species.cpp` na raiz usava Catch2 | framework não configurado no projeto |
| Paths de include `pidc/particles/` inexistentes | erro de Gemini |
| Nenhum arquivo em `CMakeLists.txt` | teste nunca registrado no CTest |
| AI_STATUS dizia "species test passou" | afirmação falsa — R-003 detectado |

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/Species.hpp` | criado (local correto, sem docstrings) |
| `include/pidc/Particle.hpp` | modificado: removidos `charge` e `mass`, adicionado `species_id` |
| `tests/test_species.cpp` | criado (padrão `require()`, sem Catch2) |
| `apps/smoke.cpp` | corrigido: inicializador de `Particle` atualizado |
| `CMakeLists.txt` | adicionado executável e teste `species` |
| `docs/ai/AI_BOARD.md` | T-010 adicionada como concluída |
| `docs/ai/AI_STATUS.md` | corrigido estado real; nota de R-003 |
| `docs/ai/DECISIONS.md` | DEC-0011 marcada como aceita |

## Comandos executados

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Resultado dos testes

```
1/4 Test #1: periodic_boundary ................   Passed    0.00 sec
2/4 Test #2: shape_function_data ..............   Passed    0.00 sec
3/4 Test #3: species ..........................   Passed    0.00 sec
4/4 Test #4: node_cloud .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 4
```

## Hipóteses assumidas

- `Species.hpp` mantida flat em `include/pidc/` (não `include/pidc/particles/`),
  consistente com o padrão atual do projeto.
- Arquivos Gemini na raiz (`Species.hpp`, `test_species.cpp`) não foram removidos —
  ação destrutiva sem necessidade imediata.
- `smoke.cpp` atualizado com `species_id = 0` como valor sentinela neutro.

## Problemas encontrados

`apps/smoke.cpp` criava `Particle` com `{pos, vel, charge, mass}` — quebrou
ao remover `charge` e `mass`. Corrigido para `{pos, vel, species_id}`.

## Violação de protocolo registrada

Gemini marcou T-008 como concluída (AI_BOARD e AI_STATUS) sem:

1. Criar o arquivo no local correto.
2. Modificar `Particle.hpp`.
3. Registrar o teste no `CMakeLists.txt`.
4. Rodar e verificar os testes.

Isso é violação direta de AGENTS.md §6 e instância concreta de R-003
("alteração silenciosa da física — refatorações podem alterar sinal, unidade
ou normalização" — aqui: estado do código divergiu do estado documentado).

## Próximo passo sugerido

Fase B está praticamente completa. Os itens restantes da Fase B são:

- `PeriodicBoundary2D` — provavelmente desnecessário agora que `Domain2D`
  já cuida do wrap periódico.
- `RegularNodeCloud2D` — útil para testes do EFG, mas pode ser criado sob demanda.
- `NeighborSearchGrid` — necessário para MLS, pode ser adiado para quando
  `MLSShapeFunction` for implementada.

Recomendação: começar a **Fase C (MLS)** com a função peso compacta e a
base polinomial linear 2D, já que `ShapeFunctionData` (o contrato) existe.
