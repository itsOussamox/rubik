#include "Cube.hpp"

Cube::Cube() {
    // slot = its identity number
    for (uint8_t i = 0; i < 8; ++i) {
        this->corners[i].pos = i;  // each corner is in its own slot
        this->corners[i].ori = 0;  // no twist
    }

    for (uint8_t i = 0; i < 12; ++i) {
        this->edges[i].pos = i;  // each edge is in its own slot
        this->edges[i].ori = 0;  // no flip
    }
};

namespace {
inline uint8_t twist_corner(uint8_t ori, uint8_t delta) {
    return static_cast<uint8_t>((ori + delta) % 3);
}

inline uint8_t flip_edge(uint8_t ori, uint8_t flip) {
    return static_cast<uint8_t>(ori ^ (flip & 0x1));
}

void cycle_corners(std::array<Corner, 8>& corners,
                   const std::array<uint8_t, 4>& idx,
                   const std::array<uint8_t, 4>& twists) {
    const Corner c0 = corners[idx[0]];
    const Corner c1 = corners[idx[1]];
    const Corner c2 = corners[idx[2]];
    const Corner c3 = corners[idx[3]];

    corners[idx[0]] = {c1.pos, twist_corner(c1.ori, twists[0])};
    corners[idx[1]] = {c2.pos, twist_corner(c2.ori, twists[1])};
    corners[idx[2]] = {c3.pos, twist_corner(c3.ori, twists[2])};
    corners[idx[3]] = {c0.pos, twist_corner(c0.ori, twists[3])};
}

void cycle_edges(std::array<Edge, 12>& edges,
                 const std::array<uint8_t, 4>& idx,
                 const std::array<uint8_t, 4>& flips) {
    const Edge e0 = edges[idx[0]];
    const Edge e1 = edges[idx[1]];
    const Edge e2 = edges[idx[2]];
    const Edge e3 = edges[idx[3]];

    edges[idx[0]] = {e1.pos, flip_edge(e1.ori, flips[0])};
    edges[idx[1]] = {e2.pos, flip_edge(e2.ori, flips[1])};
    edges[idx[2]] = {e3.pos, flip_edge(e3.ori, flips[2])};
    edges[idx[3]] = {e0.pos, flip_edge(e0.ori, flips[3])};
}

void cycle_corners_inverse(std::array<Corner, 8>& corners,
                           const std::array<uint8_t, 4>& idx,
                           const std::array<uint8_t, 4>& twists) {
    const Corner c0 = corners[idx[0]];
    const Corner c1 = corners[idx[1]];
    const Corner c2 = corners[idx[2]];
    const Corner c3 = corners[idx[3]];

    corners[idx[0]] = {c3.pos, twist_corner(c3.ori, (3 - twists[3]) % 3)};
    corners[idx[1]] = {c0.pos, twist_corner(c0.ori, (3 - twists[0]) % 3)};
    corners[idx[2]] = {c1.pos, twist_corner(c1.ori, (3 - twists[1]) % 3)};
    corners[idx[3]] = {c2.pos, twist_corner(c2.ori, (3 - twists[2]) % 3)};
}

void cycle_edges_inverse(std::array<Edge, 12>& edges,
                         const std::array<uint8_t, 4>& idx,
                         const std::array<uint8_t, 4>& flips) {
    const Edge e0 = edges[idx[0]];
    const Edge e1 = edges[idx[1]];
    const Edge e2 = edges[idx[2]];
    const Edge e3 = edges[idx[3]];

    edges[idx[0]] = {e3.pos, flip_edge(e3.ori, flips[3])};
    edges[idx[1]] = {e0.pos, flip_edge(e0.ori, flips[0])};
    edges[idx[2]] = {e1.pos, flip_edge(e1.ori, flips[1])};
    edges[idx[3]] = {e2.pos, flip_edge(e2.ori, flips[2])};
}
}

void Cube::print_state() const {
    std::cout << "Corners:\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << "  Slot " << std::setw(2) << i
                  << " -> Cubie " << std::setw(2) << (int)corners[i].pos
                  << " | Ori: " << (int)corners[i].ori << '\n';
    }

    std::cout << "\nEdges:\n";
    for (int i = 0; i < 12; ++i) {
        std::cout << "  Slot " << std::setw(2) << i
                  << " -> Cubie " << std::setw(2) << (int)edges[i].pos
                  << " | Ori: " << (int)edges[i].ori << '\n';
    }
    std::cout << std::endl;
}

void Cube::moveR() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 3, 7, 4}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{0, 11, 4, 8}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveRPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 3, 7, 4}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{0, 11, 4, 8}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveR2() {
    moveR();
    moveR();
}

void Cube::moveL() {
    static constexpr std::array<uint8_t, 4> corner_idx{{1, 5, 6, 2}};
    static constexpr std::array<uint8_t, 4> corner_twist{{2, 1, 2, 1}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{2, 9, 6, 10}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveLPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{1, 5, 6, 2}};
    static constexpr std::array<uint8_t, 4> corner_twist{{2, 1, 2, 1}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{2, 9, 6, 10}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveL2() {
    moveL();
    moveL();
}

void Cube::moveU() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 1, 2, 3}};
    static constexpr std::array<uint8_t, 4> corner_twist{{0, 0, 0, 0}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{0, 1, 2, 3}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveUPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 1, 2, 3}};
    static constexpr std::array<uint8_t, 4> corner_twist{{0, 0, 0, 0}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{0, 1, 2, 3}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveU2() {
    moveU();
    moveU();
}

void Cube::moveD() {
    static constexpr std::array<uint8_t, 4> corner_idx{{4, 5, 6, 7}};
    static constexpr std::array<uint8_t, 4> corner_twist{{0, 0, 0, 0}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{4, 5, 6, 7}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveDPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{4, 5, 6, 7}};
    static constexpr std::array<uint8_t, 4> corner_twist{{0, 0, 0, 0}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{4, 5, 6, 7}};
    static constexpr std::array<uint8_t, 4> edge_flip{{0, 0, 0, 0}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveD2() {
    moveD();
    moveD();
}

void Cube::moveF() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 1, 5, 4}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{1, 9, 5, 8}};
    static constexpr std::array<uint8_t, 4> edge_flip{{1, 1, 1, 1}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveFPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{0, 1, 5, 4}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{1, 9, 5, 8}};
    static constexpr std::array<uint8_t, 4> edge_flip{{1, 1, 1, 1}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveF2() {
    moveF();
    moveF();
}

void Cube::moveB() {
    static constexpr std::array<uint8_t, 4> corner_idx{{2, 3, 7, 6}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{3, 11, 7, 10}};
    static constexpr std::array<uint8_t, 4> edge_flip{{1, 1, 1, 1}};
    cycle_edges(this->edges, edge_idx, edge_flip);
}

void Cube::moveBPrime() {
    static constexpr std::array<uint8_t, 4> corner_idx{{2, 3, 7, 6}};
    static constexpr std::array<uint8_t, 4> corner_twist{{1, 2, 1, 2}};
    cycle_corners_inverse(this->corners, corner_idx, corner_twist);

    static constexpr std::array<uint8_t, 4> edge_idx{{3, 11, 7, 10}};
    static constexpr std::array<uint8_t, 4> edge_flip{{1, 1, 1, 1}};
    cycle_edges_inverse(this->edges, edge_idx, edge_flip);
}

void Cube::moveB2() {
    moveB();
    moveB();
}

Cube::~Cube() {
    // No dynamic memory to free
};
