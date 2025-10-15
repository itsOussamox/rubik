#ifndef CUBE_SOLVER_HPP
#define CUBE_SOLVER_HPP

#include "Cube.hpp"

#include <string>
#include <vector>

class CubeSolver {
public:
    CubeSolver();
    ~CubeSolver();

    std::string solve(const std::vector<std::string>& scramble) const;

private:
    Cube m_cube;
};

#endif // CUBE_SOLVER_HPP
