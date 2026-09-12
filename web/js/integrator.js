/**
 * JS port of the C++ velocity-Verlet / O(n²) force model for live demos.
 * Softened Newtonian gravity: a ∝ r / (r² + ε²)^{3/2}
 */
export function createBody(name, mass, x, y, z, vx, vy, vz) {
  return {
    name,
    mass,
    pos: { x, y, z: z || 0 },
    vel: { x: vx || 0, y: vy || 0, z: vz || 0 },
    acc: { x: 0, y: 0, z: 0 },
  };
}

export function computeAccelerations(bodies, G, soft2) {
  const n = bodies.length;
  for (const b of bodies) {
    b.acc.x = b.acc.y = b.acc.z = 0;
  }
  for (let i = 0; i < n; i++) {
    for (let j = i + 1; j < n; j++) {
      const dx = bodies[j].pos.x - bodies[i].pos.x;
      const dy = bodies[j].pos.y - bodies[i].pos.y;
      const dz = bodies[j].pos.z - bodies[i].pos.z;
      const r2 = dx * dx + dy * dy + dz * dz + soft2;
      const invR = 1 / Math.sqrt(r2);
      const invR3 = invR * invR * invR;
      const f = G * invR3;
      const aix = dx * f * bodies[j].mass;
      const aiy = dy * f * bodies[j].mass;
      const aiz = dz * f * bodies[j].mass;
      bodies[i].acc.x += aix;
      bodies[i].acc.y += aiy;
      bodies[i].acc.z += aiz;
      bodies[j].acc.x -= dx * f * bodies[i].mass;
      bodies[j].acc.y -= dy * f * bodies[i].mass;
      bodies[j].acc.z -= dz * f * bodies[i].mass;
    }
  }
}

export function totalEnergy(bodies, G, soft2) {
  let ke = 0;
  let pe = 0;
  const n = bodies.length;
  for (const b of bodies) {
    const v2 = b.vel.x ** 2 + b.vel.y ** 2 + b.vel.z ** 2;
    ke += 0.5 * b.mass * v2;
  }
  for (let i = 0; i < n; i++) {
    for (let j = i + 1; j < n; j++) {
      const dx = bodies[j].pos.x - bodies[i].pos.x;
      const dy = bodies[j].pos.y - bodies[i].pos.y;
      const dz = bodies[j].pos.z - bodies[i].pos.z;
      const dist = Math.sqrt(dx * dx + dy * dy + dz * dz + soft2);
      pe -= (G * bodies[i].mass * bodies[j].mass) / dist;
    }
  }
  return ke + pe;
}

export function velocityVerletStep(bodies, dt, G, soft2) {
  for (const b of bodies) {
    b.pos.x += b.vel.x * dt + 0.5 * b.acc.x * dt * dt;
    b.pos.y += b.vel.y * dt + 0.5 * b.acc.y * dt * dt;
    b.pos.z += b.vel.z * dt + 0.5 * b.acc.z * dt * dt;
  }
  const aOld = bodies.map((b) => ({ x: b.acc.x, y: b.acc.y, z: b.acc.z }));
  computeAccelerations(bodies, G, soft2);
  for (let i = 0; i < bodies.length; i++) {
    const b = bodies[i];
    b.vel.x += 0.5 * (aOld[i].x + b.acc.x) * dt;
    b.vel.y += 0.5 * (aOld[i].y + b.acc.y) * dt;
    b.vel.z += 0.5 * (aOld[i].z + b.acc.z) * dt;
  }
}

export const SCENARIOS = {
  figure8: {
    label: "Figure-8 (3-body)",
    G: 1,
    dt: 0.001,
    soft2: 1e-8,
    scale: 180,
    bodies: () => {
      const x1 = 0.97000436,
        y1 = -0.24308753;
      const vx3 = -0.93240737,
        vy3 = -0.86473146;
      return [
        createBody("A", 1, x1, y1, 0, -vx3 / 2, -vy3 / 2, 0),
        createBody("B", 1, -x1, -y1, 0, -vx3 / 2, -vy3 / 2, 0),
        createBody("C", 1, 0, 0, 0, vx3, vy3, 0),
      ];
    },
  },
  binary: {
    label: "Equal-mass binary + planet",
    G: 1,
    dt: 0.002,
    soft2: 1e-6,
    scale: 90,
    bodies: () => [
      createBody("Star1", 1, -1, 0, 0, 0, -0.5, 0),
      createBody("Star2", 1, 1, 0, 0, 0, 0.5, 0),
      createBody("Planet", 0.05, 0, 2.5, 0, 1.1, 0, 0),
    ],
  },
  solar: {
    label: "Sun–Earth–Jupiter (AU)",
    G: 4 * Math.PI * Math.PI,
    dt: 0.001,
    soft2: 1e-10,
    scale: 55,
    bodies: () => {
      const earthV = (2 * Math.PI) / Math.sqrt(1);
      const jupV = (2 * Math.PI) / Math.sqrt(5.2);
      return [
        createBody("Sun", 1, 0, 0, 0, 0, 0, 0),
        createBody("Earth", 3.003e-6, 1, 0, 0, 0, earthV, 0),
        createBody("Jupiter", 9.55e-4, 5.2, 0, 0, 0, jupV, 0),
      ];
    },
  },
};
