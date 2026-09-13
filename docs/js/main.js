import {
  SCENARIOS,
  computeAccelerations,
  totalEnergy,
  velocityVerletStep,
} from "./integrator.js";
import { OrbitViz } from "./viz.js";

const canvas = document.getElementById("canvas");
const viz = new OrbitViz(canvas);
const sel = document.getElementById("scenario");
const stats = document.getElementById("stats");
const speed = document.getElementById("speed");
const btnPlay = document.getElementById("btnPlay");
const btnReset = document.getElementById("btnReset");
const badge = document.getElementById("badge");

for (const [id, sc] of Object.entries(SCENARIOS)) {
  const opt = document.createElement("option");
  opt.value = id;
  opt.textContent = sc.label;
  sel.appendChild(opt);
}

let state = {
  key: "figure8",
  bodies: [],
  G: 1,
  dt: 0.001,
  soft2: 1e-8,
  substeps: 8,
};

function boot(key) {
  const sc = SCENARIOS[key];
  state.key = key;
  state.bodies = sc.bodies();
  state.G = sc.G;
  state.dt = sc.dt;
  state.soft2 = sc.soft2;
  computeAccelerations(state.bodies, state.G, state.soft2);
  viz.reset(state.bodies, sc.scale);
  viz.setEnergy(totalEnergy(state.bodies, state.G, state.soft2));
  badge.textContent = sc.label;
}

sel.addEventListener("change", () => boot(sel.value));
btnReset.addEventListener("click", () => boot(state.key));
btnPlay.addEventListener("click", () => {
  viz.running = !viz.running;
  btnPlay.textContent = viz.running ? "Pause" : "Play";
});

boot("figure8");

function frame() {
  if (viz.running) {
    const mult = Number(speed.value) || 1;
    const n = Math.max(1, Math.floor(state.substeps * mult));
    for (let i = 0; i < n; i++) {
      velocityVerletStep(state.bodies, state.dt, state.G, state.soft2);
      viz.t += state.dt;
    }
    viz.pushTrail();
    const E = totalEnergy(state.bodies, state.G, state.soft2);
    viz.setEnergy(E);
    const dE = viz.E0 !== 0 ? (E - viz.E0) / viz.E0 : E - viz.E0;
    stats.innerHTML = [
      `t = <strong>${viz.t.toFixed(3)}</strong>`,
      `bodies = <strong>${state.bodies.length}</strong>`,
      `E = <strong>${E.toExponential(4)}</strong>`,
      `|ΔE/E₀| = <strong>${Math.abs(dE).toExponential(3)}</strong>`,
      `integrator = <strong>velocity Verlet</strong>`,
      `force = <strong>O(n²) direct</strong>`,
    ].join("<br>");
  }
  viz.draw();
  requestAnimationFrame(frame);
}

requestAnimationFrame(frame);
