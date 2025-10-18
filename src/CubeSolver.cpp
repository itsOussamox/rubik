#include "CubeSolver.hpp"

#include <stdexcept>

CubeSolver::CubeSolver() : m_cube() {
    std::cout << "Solved:\n";
    m_cube.print_state();

    m_cube.moveR2();
    std::cout << "\nAfter R2:\n";
    m_cube.print_state();

    m_cube.moveR2();
    std::cout << "\nAfter R2:\n";
    m_cube.print_state();
};

CubeSolver::~CubeSolver() {

    // No dynamic memory to free
};

std::string CubeSolver::solve(const std::vector<std::string>& scramble) const {
    (void)scramble;
    throw std::logic_error("CubeSolver::solve is not implemented yet");
}
