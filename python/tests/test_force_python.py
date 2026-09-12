"""Pure-Python reference checks (no C++ binary required for this file alone)."""
from __future__ import annotations

import math


def soft_potential(m1, m2, r2, soft2, G=1.0):
    return -G * m1 * m2 / math.sqrt(r2 + soft2)


def test_soft_potential_finite_at_zero():
    # Without softening this would diverge; with soft2 it stays finite.
    u = soft_potential(1.0, 1.0, 0.0, 1e-6)
    assert math.isfinite(u)
    assert u < 0
