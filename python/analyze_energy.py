#!/usr/bin/env python3
"""Analyze energy drift from an nbody-orbit trajectory JSON.

Writes a CSV of (frame_index, t, E, dE_rel) suitable for plotting,
and prints a short conservation summary to stdout.
"""
from __future__ import annotations

import argparse
import csv
import json
import math
import sys
from pathlib import Path


def load_trajectory(path: Path) -> dict:
    with path.open() as f:
        return json.load(f)


def analyze(traj: dict) -> list[dict]:
    energy = traj.get("energy") or []
    frames = traj.get("frames") or []
    if not energy:
        raise ValueError("trajectory has no 'energy' array")
    E0 = energy[0]
    rows = []
    for i, E in enumerate(energy):
        t = frames[i]["t"] if i < len(frames) else float("nan")
        dE = (E - E0) / E0 if E0 != 0 else E - E0
        rows.append({"frame": i, "t": t, "E": E, "dE_rel": dE})
    return rows


def write_csv(rows: list[dict], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["frame", "t", "E", "dE_rel"])
        w.writeheader()
        w.writerows(rows)


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("trajectory", type=Path, help="trajectory JSON from nbody-orbit CLI")
    p.add_argument(
        "-o",
        "--csv",
        type=Path,
        default=Path("data/energy_drift.csv"),
        help="output CSV path",
    )
    args = p.parse_args(argv)

    traj = load_trajectory(args.trajectory)
    rows = analyze(traj)
    write_csv(rows, args.csv)

    max_abs = max(abs(r["dE_rel"]) for r in rows)
    rms = math.sqrt(sum(r["dE_rel"] ** 2 for r in rows) / len(rows))
    print(f"frames={len(rows)}  max|dE/E0|={max_abs:.3e}  rms(dE/E0)={rms:.3e}")
    print(f"wrote {args.csv}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
