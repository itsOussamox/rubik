#ifndef RUBIK_CUBE_HPP
#define RUBIK_CUBE_HPP
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>

// Cubelets Model Indices
// Edge slots:                Corner slots:
//   0  = UR  (Up-Right)         0 = URF (Up-Right-Front)
//   1  = UF  (Up-Front)         1 = UFL (Up-Front-Left)
//   2  = UL  (Up-Left)          2 = ULB (Up-Left-Back)
//   3  = UB  (Up-Back)          3 = UBR (Up-Back-Right)
//   4  = DR  (Down-Right)       4 = DFR (Down-Front-Right)
//   5  = DF  (Down-Front)       5 = DLF (Down-Left-Front)
//   6  = DL  (Down-Left)        6 = DBL (Down-Back-Left)
//   7  = DB  (Down-Back)        7 = DRB (Down-Right-Back)
//   8  = FR  (Front-Right)
//   9  = FL  (Front-Left)
//   10 = BL  (Back-Left)
//   11 = BR  (Back-Right)

struct Corner {
    uint8_t pos; // 0..7  — which corner cubie currently sits in this slot
    uint8_t ori; // 0..2  — corner twist (0 = correctly oriented)
};

struct Edge {
    uint8_t pos; // 0..11 — which edge cubie sits in this slot
    uint8_t ori; // 0..1  — edge flip (0 = correct, 1 = flipped)
};
class Cube {
    std::array<Corner, 8> corners;  // corners[slot] = {cubie_id, orientation}
    std::array<Edge, 12> edges;     // edges[slot]   = {cubie_id, orientation}
public:
    Cube();
    void print_state() const;
    void moveR();
    void moveRPrime();
    void moveR2();
    void moveL();
    void moveLPrime();
    void moveL2();
    void moveU();
    void moveUPrime();
    void moveU2();
    void moveD();
    void moveDPrime();
    void moveD2();
    void moveF();
    void moveFPrime();
    void moveF2();
    void moveB();
    void moveBPrime();
    void moveB2();
    ~Cube();
};

#endif // RUBIK_CUBE_HPP
