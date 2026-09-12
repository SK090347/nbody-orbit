"""Energy conservation smoke tests for the C++ Verlet integrator."""
from __future__ import annotations

import json
import math
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / "nbody-orbit"
SCENARIO = ROOT / "scenarios" / "figure8.json"
OUT = ROOT / "data" / "pytest_traj.json"


def _run_sim(steps: int = 8000, dt: float = 1e-3) -> dict:
    assert BIN.exists(), f"build the CLI first: make -C {ROOT}"
    OUT.parent.mkdir(parents=True, exist_ok=True)
    cmd = [
        str(BIN),
        "--scenario",
        str(SCENARIO),
        "--out",
        str(OUT),
        "--steps",
        str(steps),
        "--dt",
        str(dt),
        "--frame-every",
        "40",
    ]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    assert r.returncode == 0, r.stderr + r.stdout
    with OUT.open() as f:
        return json.load(f)


def test_energy_relative_drift_small():
    traj = _run_sim()
    energy = traj["energy"]
    assert len(energy) > 10
    E0 = energy[0]
    assert E0 != 0
    max_rel = max(abs((E - E0) / E0) for E in energy)
    # Velocity Verlet on the figure-8 should keep relative drift well under 1e-3
    # for these settings (often ~1e-6–1e-5).
    assert max_rel < 1e-3, f"energy drift too large: {max_rel}"


def test_frame_count_matches_energy():
    traj = _run_sim(steps=1000, dt=1e-3)
    assert len(traj["frames"]) == len(traj["energy"])
    assert traj["frames"][0]["t"] == 0.0
    assert traj["meta"]["integrator"] == "velocity_verlet"


def test_analyze_energy_csv(tmp_path):
    traj = _run_sim(steps=2000)
    # write via module
    sys.path.insert(0, str(ROOT / "python"))
    from analyze_energy import analyze, write_csv

    rows = analyze(traj)
    csv_path = tmp_path / "e.csv"
    write_csv(rows, csv_path)
    text = csv_path.read_text()
    assert "dE_rel" in text
    assert len(rows) == len(traj["energy"])
