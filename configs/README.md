# configs

Arquivos de configuração planejados.

## Arquivos futuros

```text
poisson_mms_2d.json
pidc_small_periodic.json
pidc_medium_periodic.json
pidc_thesis_like.json
langmuir_1d.json
```

## Campos recomendados

```json
{
  "case_name": "poisson_mms_2d",
  "domain": {
    "xmin": 0.0,
    "xmax": 1.0,
    "ymin": 0.0,
    "ymax": 1.0
  },
  "nodes": {
    "type": "regular",
    "nx": 21,
    "ny": 21,
    "influence_scale": 2.0
  },
  "mls": {
    "basis": "linear_2d",
    "weight": "compact_schwarz",
    "support": "rectangular"
  },
  "poisson": {
    "boundary": "dirichlet",
    "solver": "dense_lu"
  },
  "output": {
    "directory": "data/output/poisson_mms_2d"
  }
}
```
