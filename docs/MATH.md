# Mathematics — nbody-orbit

## Softened Newtonian potential

Pair potential used for energy diagnostics:

$$
V(r) = -\frac{G m_i m_j}{\sqrt{r^2 + \varepsilon^2}}
$$

Force = $-\nabla V$. As $\varepsilon \to 0$, recover classical $1/r$ potential / $1/r^2$ force.

## Symplecticity note

Velocity Verlet is a composition of exact shears in phase space (kick–drift–kick). For a separable Hamiltonian $H = T(p) + V(q)$ it preserves a nearby shadow Hamiltonian, so energy error is oscillatory rather than secular for fixed step size.

## Figure-8 IC

Equal-mass three-body choreography (Chenciner & Montgomery, 2000). Periodic structure amplifies integrator mistakes — ideal energy-drift fixture.
