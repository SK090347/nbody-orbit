# nbody-orbit


**Live demo:** https://sk090347.github.io/nbody-orbit/
N-body gravity lab: C++17 Verlet integrator, Python energy drift analysis, and a canvas viz with a JS port of the same physics. I care more about bounded energy error than pretty spirals that secretly dissipate.

[![CI](https://github.com/SK090347/nbody-orbit/actions/workflows/ci.yml/badge.svg)](https://github.com/SK090347/nbody-orbit/actions/workflows/ci.yml)
[![License: MIT OR Apache-2.0](https://img.shields.io/badge/license-MIT%20OR%20Apache--2.0-blue.svg)](LICENSE)

[Sumit Kumar Ta](https://github.com/SK090347) · dual MIT / Apache-2.0

## How it works

Pairwise gravity with Plummer softening \(\varepsilon^2\):

\[
\mathbf{a}_i = -G \sum_{j \neq i} m_j \frac{\mathbf{r}_i - \mathbf{r}_j}{\bigl(|\mathbf{r}_i - \mathbf{r}_j|^2 + \varepsilon^2\bigr)^{3/2}}
\]

Velocity Verlet (symplectic leapfrog):

\[
\begin{aligned}
\mathbf{x} &\leftarrow \mathbf{x} + \mathbf{v}\,\Delta t + \tfrac12\mathbf{a}\,\Delta t^2 \\
\mathbf{a}' &\leftarrow \mathrm{accel}(\mathbf{x}) \\
\mathbf{v} &\leftarrow \mathbf{v} + \tfrac12(\mathbf{a}+\mathbf{a}')\,\Delta t
\end{aligned}
\]

Energy tracked as \(E = \sum_i \tfrac12 m_i \|\mathbf{v}_i\|^2 - \sum_{i<j} G m_i m_j / \sqrt{r_{ij}^2+\varepsilon^2}\). Direct force is \(O(n^2)\); Barnes–Hut is a documented upgrade path.

Longer notes: [docs/MATH.md](docs/MATH.md).

## Quick start

```bash
make -j
make run          # figure-8 → data/trajectory.json
make demo

python3 -m pip install -r python/requirements.txt
python3 python/analyze_energy.py data/trajectory.json -o data/energy_drift.csv
python3 -m pytest -q python/tests

python3 -m http.server 8080 --directory web
# http://localhost:8080
```

```text
./nbody-orbit --scenario scenarios/figure8.json --out data/out.json \
  --steps 20000 --dt 0.001 --frame-every 20
```

## Layout

```
cpp/           Verlet + O(n²) forces + JSON I/O
scenarios/     figure8.json, solar_lite.json
python/        energy CSV + pytest
web/           canvas orbits (JS Verlet)
docs/MATH.md
```

CI builds with `g++ -std=c++17`, runs a short scenario, then pytest (figure-8 relative energy drift \(< 10^{-3}\)).

## License

**MIT** OR **Apache-2.0**.
