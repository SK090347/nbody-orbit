const PALETTE = [
  ["#6ee7ff", "#1a6cff"],
  ["#ff7ab6", "#ff3d81"],
  ["#ffe66d", "#ff9f1c"],
  ["#7dffa3", "#2ecc71"],
  ["#c4b5fd", "#8b5cf6"],
];

export class OrbitViz {
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext("2d");
    this.trails = [];
    this.maxTrail = 900;
    this.scale = 160;
    this.stars = this._makeStars(180);
    this.bodies = [];
    this.t = 0;
    this.E0 = null;
    this.E = null;
    this.running = true;
    this._resize();
    window.addEventListener("resize", () => this._resize());
  }

  _makeStars(n) {
    const s = [];
    for (let i = 0; i < n; i++) {
      s.push({
        x: Math.random(),
        y: Math.random(),
        r: Math.random() * 1.4 + 0.2,
        a: Math.random() * 0.6 + 0.2,
      });
    }
    return s;
  }

  _resize() {
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    const rect = this.canvas.parentElement.getBoundingClientRect();
    this.canvas.width = Math.floor(rect.width * dpr);
    this.canvas.height = Math.floor(rect.height * dpr);
    this.dpr = dpr;
    this.w = rect.width;
    this.h = rect.height;
  }

  reset(bodies, scale) {
    this.bodies = bodies;
    this.scale = scale;
    this.trails = bodies.map(() => []);
    this.t = 0;
    this.E0 = null;
    this.E = null;
  }

  setEnergy(E) {
    if (this.E0 === null) this.E0 = E;
    this.E = E;
  }

  pushTrail() {
    for (let i = 0; i < this.bodies.length; i++) {
      const b = this.bodies[i];
      const tr = this.trails[i];
      tr.push({ x: b.pos.x, y: b.pos.y });
      if (tr.length > this.maxTrail) tr.shift();
    }
  }

  worldToScreen(x, y) {
    return {
      x: this.w / 2 + x * this.scale,
      y: this.h / 2 - y * this.scale,
    };
  }

  draw() {
    const ctx = this.ctx;
    const dpr = this.dpr;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

    // Motion blur / fade for cinematic trails
    ctx.fillStyle = "rgba(5, 6, 15, 0.22)";
    ctx.fillRect(0, 0, this.w, this.h);

    // Stars
    for (const s of this.stars) {
      ctx.beginPath();
      ctx.fillStyle = `rgba(220,230,255,${s.a})`;
      ctx.arc(s.x * this.w, s.y * this.h, s.r, 0, Math.PI * 2);
      ctx.fill();
    }

    // Soft nebula glow behind system
    const g = ctx.createRadialGradient(
      this.w / 2,
      this.h / 2,
      10,
      this.w / 2,
      this.h / 2,
      Math.min(this.w, this.h) * 0.45
    );
    g.addColorStop(0, "rgba(80, 60, 160, 0.12)");
    g.addColorStop(1, "rgba(5, 6, 15, 0)");
    ctx.fillStyle = g;
    ctx.fillRect(0, 0, this.w, this.h);

    // Trails
    for (let i = 0; i < this.trails.length; i++) {
      const tr = this.trails[i];
      if (tr.length < 2) continue;
      const [c0, c1] = PALETTE[i % PALETTE.length];
      ctx.beginPath();
      for (let k = 0; k < tr.length; k++) {
        const p = this.worldToScreen(tr[k].x, tr[k].y);
        if (k === 0) ctx.moveTo(p.x, p.y);
        else ctx.lineTo(p.x, p.y);
      }
      ctx.strokeStyle = c0;
      ctx.globalAlpha = 0.55;
      ctx.lineWidth = 1.6;
      ctx.shadowColor = c1;
      ctx.shadowBlur = 8;
      ctx.stroke();
      ctx.shadowBlur = 0;
      ctx.globalAlpha = 1;
    }

    // Bodies
    for (let i = 0; i < this.bodies.length; i++) {
      const b = this.bodies[i];
      const p = this.worldToScreen(b.pos.x, b.pos.y);
      const [c0, c1] = PALETTE[i % PALETTE.length];
      const radius = Math.max(3.5, Math.min(18, 4 + Math.log10(b.mass + 1) * 6));

      const glow = ctx.createRadialGradient(p.x, p.y, 0, p.x, p.y, radius * 4);
      glow.addColorStop(0, c0 + "cc");
      glow.addColorStop(0.4, c1 + "55");
      glow.addColorStop(1, "transparent");
      ctx.fillStyle = glow;
      ctx.beginPath();
      ctx.arc(p.x, p.y, radius * 4, 0, Math.PI * 2);
      ctx.fill();

      ctx.beginPath();
      ctx.fillStyle = c0;
      ctx.shadowColor = c1;
      ctx.shadowBlur = 16;
      ctx.arc(p.x, p.y, radius, 0, Math.PI * 2);
      ctx.fill();
      ctx.shadowBlur = 0;

      ctx.fillStyle = "rgba(255,255,255,0.85)";
      ctx.font = "11px ui-sans-serif, system-ui";
      ctx.fillText(b.name, p.x + radius + 4, p.y - 4);
    }
  }
}
