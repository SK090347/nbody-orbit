# nbody-orbit

**High-performance N-body gravity lab** — C++17 core, Python energy analysis, HTML/JS canvas visualization.

[![CI](https://github.com/SK090347/nbody-orbit/actions/workflows/ci.yml/badge.svg)](https://github.com/SK090347/nbody-orbit/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![License: Apache-2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE-APACHE)

> Built by [Sumit Kumar Ta](https://github.com/SK090347) — portfolio project showcasing **numerics**, **systems C++**, and **interactive visualization**.

Dual-licensed **MIT OR Apache-2.0**. See [LICENSE](LICENSE), [LICENSE-APACHE](LICENSE-APACHE), and [NOTICE](NOTICE).

---

## Why this exists

Classical N-body gravity is the simplest physics that still demands careful **numerical methods**:

- Naive pairwise forces are **O(n²)** — fine for labs, a baseline for tree codes.
- Time integration must respect **symplecticity** so orbits don’t spiral from fake dissipation.
- Diagnostics (energy drift) separate “looks pretty” demos from trustworthy simulation.

`nbody-orbit` ships a small but complete stack: integrate → export JSON → analyze → visualize.

---

## Physics notes

### Force model

Newtonian gravity with Plummer-style softening:

\[
\mathbf{a}_i = -G \sum_{j \neq i} m_j \frac{\mathbf{r}_i - \mathbf{r}_j}{\bigl(|\mathbf{r}_i - \mathbf{r}_j|^2 + \varepsilon^2\bigr)^{3/2}}
\]

Softening \(\varepsilon^2\) (`soft2`) keeps close encounters finite without changing far-field \(1/r^2\) behavior.

### Integrator — velocity Verlet (leapfrog family)

1. \(\mathbf{x} \leftarrow \mathbf{x} + \mathbf{v}\,\Delta t + \tfrac12\mathbf{a}\,\Delta t^2\)
2. Recompute \(\mathbf{a}\) from new positions
3. \(\mathbf{v} \leftarrow \mathbf{v} + \tfrac12(\mathbf{a}_\text{old}+\mathbf{a}_\text{new})\,\Delta t\)

This is a second-order **symplectic** method: long-term energy oscillates with bounded error instead of secular drift (for Hamiltonian systems with fixed \(\Delta t\)).

### Barnes–Hut (documented stub)

For large \(n\), an octree multipole approximation reduces force cost toward **O(n log n)**. The force header documents the upgrade path (`theta` opening angle, monopole CoM); the CLI currently uses the exact **direct O(n²)** sum so conservation tests stay clean and debuggable.

### Featured IC — figure-8 choreography

Equal-mass three-body periodic orbit of Chenciner & Montgomery (2000). Ideal for energy tests: delicate periodic structure amplifies integrator mistakes.

---

## Architecture

```
nbody-orbit/
├── cpp/                 # C++17 Verlet + O(n²) forces + JSON I/O
│   ├── include/         # body, force, integrator, json_io
│   └── src/             # CLI: nbody-orbit
├── scenarios/           # figure8.json, solar_lite.json
├── python/              # energy drift CSV + pytest
├── web/                 # canvas viz + JS port of the integrator
├── data/                # generated trajectories / CSV
└── .github/workflows/   # CI: build, run sim, pytest
```

```mermaid
flowchart LR
  SC[scenarios/*.json] --> CLI[nbody-orbit CLI]
  CLI --> JSON[trajectory JSON]
  JSON --> PY[analyze_energy.py]
  PY --> CSV[energy_drift.csv]
  JS[web JS Verlet port] --> VIZ[canvas orbits]
  JSON -.-> VIZ
```

---

## Quick start

### Build & run (C++)

```bash
git clone https://github.com/SK090347/nbody-orbit.git
cd nbody-orbit
make -j
make run          # default figure-8 → data/trajectory.json
make demo         # figure-8 + solar_lite
```

CLI flags:

```text
./nbody-orbit --scenario scenarios/figure8.json --out data/out.json \
  --steps 20000 --dt 0.001 --frame-every 20
```

### Python analysis

```bash
python3 -m pip install -r python/requirements.txt
python3 python/analyze_energy.py data/trajectory.json -o data/energy_drift.csv
python3 -m pytest -q python/tests
```

### Browser visualization

Serve the `web/` folder (ES modules need HTTP, not `file://`):

```bash
python3 -m http.server 8080 --directory web
# open http://localhost:8080
```

Live demos: figure-8, binary+planet, Sun–Earth–Jupiter. HUD shows \(t\), \(E\), and \(|\Delta E/E_0|\).

---

## Tests

| Check | What it proves |
|-------|----------------|
| `test_energy_relative_drift_small` | Figure-8 relative energy drift \(\lt 10^{-3}\) over thousands of steps |
| `test_frame_count_matches_energy` | JSON schema / frame ↔ energy alignment |
| `test_soft_potential_finite_at_zero` | Softening remains finite at \(r=0\) |

CI builds the binary with `g++ -std=c++17`, runs a short scenario, then pytest.

---

## Recruiter skills map

| Skill | Where it shows up |
|-------|-------------------|
| **Numerical methods** | Symplectic Verlet, softening, energy diagnostics |
| **Systems / C++17** | Zero-dependency CLI, header/src split, Makefile, JSON writer |
| **Performance awareness** | Explicit O(n²) baseline + Barnes–Hut upgrade notes |
| **Python tooling** | Trajectory analysis, CSV export, pytest gating |
| **Front-end viz** | Canvas trails, glow, live JS port of the same physics |
| **Engineering hygiene** | Dual license, CI, scenarios-as-data, readable README |

---

## Topics

`n-body` · `physics` · `cpp` · `python` · `javascript` · `simulation` · `portfolio`

---

## License

Copyright © 2026 Sumit Kumar Ta (SK090347).

Licensed under **MIT** or **Apache-2.0**, at your option.
