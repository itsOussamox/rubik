#include "CubeSolver.hpp"

#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool isFaceMove(char c) {
    switch (c) {
    case 'F':
    case 'R':
    case 'U':
    case 'B':
    case 'L':
    case 'D':
        return true;
    default:
        return false;
    }
}

bool isValidModifier(const std::string& token) {
    if (token.size() == 1) {
        return true;
    }
    if (token.size() == 2) {
        return token[1] == '\'' || token[1] == '2';
    }
    return false;
}

std::string normalizeToken(const std::string& raw) {
    if (raw.empty()) {
        return raw;
    }

    std::string token = raw;
    token[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(token[0])));
    return token;
}

bool isValidMoveToken(const std::string& raw) {
    if (raw.empty()) {
        return false;
    }

    std::string token = normalizeToken(raw);

    if (!isFaceMove(token[0])) {
        return false;
    }

    if (!isValidModifier(token)) {
        return false;
    }

    return true;
}

std::vector<std::string> parseScramble(int argc, char** argv) {
    if (argc < 2) {
        throw std::invalid_argument(
            "Usage: rubik <scramble moves...>\nExample: rubik \"R2 D' B' D F2\"");
    }

    std::ostringstream joined;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            joined << ' ';
        }
        joined << argv[i];
    }

    std::istringstream input(joined.str());
    std::vector<std::string> tokens;
    std::string token;

    while (input >> token) {
        if (!isValidMoveToken(token)) {
            throw std::invalid_argument("Invalid move token: " + token);
        }
        tokens.push_back(normalizeToken(token));
    }

    if (tokens.empty()) {
        throw std::invalid_argument("Scramble sequence cannot be empty.");
    }

    return tokens;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const std::vector<std::string> scramble = parseScramble(argc, argv);

        CubeSolver solver;
        const std::string solution = solver.solve(scramble);

        std::cout << solution << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::logic_error& err) {
        std::cerr << "Solver not ready: " << err.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::invalid_argument& err) {
        std::cerr << "Input error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& err) {
        std::cerr << "Unexpected error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
}
