#include "CubeSolver.hpp"

#include <stdexcept>

CubeSolver::CubeSolver() = default;

CubeSolver::~CubeSolver() = default;

std::string CubeSolver::solve(const std::vector<std::string>& scramble) const {
    (void)scramble;
    throw std::logic_error("CubeSolver::solve is not implemented yet");
}
