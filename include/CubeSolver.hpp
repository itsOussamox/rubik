#ifndef CUBE_SOLVER_HPP
#define CUBE_SOLVER_HPP

#include <string>
#include <vector>

class CubeSolver {
public:
    CubeSolver();
    ~CubeSolver();

    std::string solve(const std::vector<std::string>& scramble) const;
};

#endif // CUBE_SOLVER_HPP
