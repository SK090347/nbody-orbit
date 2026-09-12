#pragma once
#include "body.hpp"
#include <vector>

namespace nbody {

// Pairwise Newtonian gravity, O(n^2). Softened: F ~ G m_i m_j r / (r^2 + eps^2)^{3/2}
void compute_accelerations(std::vector<Body>& bodies, double G);

// Kinetic + potential (softened) energy for conservation diagnostics.
double total_energy(const std::vector<Body>& bodies, double G);

/*
 * Barnes–Hut (optional upgrade path — stub documented, not wired in the CLI):
 *
 * Build an octree over the bodies. For each body i, walk the tree:
 *   if a node is "far" (s / d < theta), approximate the subtree as a monopole
 *   at its center of mass; otherwise recurse into children.
 * Complexity drops from O(n^2) toward O(n log n) for large n.
 * Accuracy is controlled by opening angle theta (typical 0.5–0.7).
 * Softening still applies when evaluating leaf or multipole forces.
 *
 * Suggested next steps: BodyOctree, bh_accelerations(bodies, G, theta),
 * and a CLI flag --force=barnes-hut|direct.
 */

}  // namespace nbody
