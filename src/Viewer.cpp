#include "Viewer.hpp"

#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#ifdef RUBIK_WITH_GRAPHICS

#include <GLFW/glfw3.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace {

struct MoveAxis {
    char axis = 0; // 'X', 'Y', 'Z' for rotation axis
    int layerCoord = 0; // The coordinate of the layer to be rotated (-1, 0, 1)
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
};

constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;

float clampf(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

void setStickerColor(char face) {
    switch (face) {
    case 'U':
        glColor3f(0.9f, 0.9f, 0.9f);
        break;
    case 'D':
        glColor3f(1.0f, 0.8f, 0.0f);
        break;
    case 'F':
        glColor3f(0.0f, 0.6f, 0.2f);
        break;
    case 'B':
        glColor3f(0.1f, 0.2f, 0.7f);
        break;
    case 'L':
        glColor3f(0.9f, 0.4f, 0.0f);
        break;
    case 'R':
        glColor3f(0.8f, 0.0f, 0.0f);
        break;
    default:
        glColor3f(0.05f, 0.05f, 0.05f);
        break;
    }
}

void drawStickeredFace(float size, char face) {
    const float half = size * 0.5f;
    switch (face) {
    case 'F':
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-half, -half, half);
        glVertex3f(half, -half, half);
        glVertex3f(half, half, half);
        glVertex3f(-half, half, half);
        break;
    case 'B':
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(half, -half, -half);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half, half, -half);
        glVertex3f(half, half, -half);
        break;
    case 'L':
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(-half, -half, half);
        glVertex3f(-half, half, half);
        glVertex3f(-half, half, -half);
        break;
    case 'R':
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(half, -half, half);
        glVertex3f(half, -half, -half);
        glVertex3f(half, half, -half);
        glVertex3f(half, half, half);
        break;
    case 'U':
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-half, half, half);
        glVertex3f(half, half, half);
        glVertex3f(half, half, -half);
        glVertex3f(-half, half, -half);
        break;
    case 'D':
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glVertex3f(half, -half, -half);
        glVertex3f(half, -half, half);
        glVertex3f(-half, -half, half);
        break;
    default:
        break;
    }
}

struct MoveInfo {
    char face = 0; // 'F', 'B', 'U', 'D', 'L', 'R'
    bool prime = false; // indicates counter-clockwise turn
    bool doubleTurn = false; // indicates 180-degree turn
    bool valid = false; // indicates if the move is valid
};

MoveInfo parseMove(const std::string& move) {
    MoveInfo info;
    if (move.empty()) {
        return info;
    }

    const char face = static_cast<char>(std::toupper(static_cast<unsigned char>(move[0])));
    switch (face) {
    case 'F':
    case 'B':
    case 'U':
    case 'D':
    case 'L':
    case 'R':
        info.face = face;
        info.valid = true;
        break;
    default:
        return info;
    }

    if (move.size() == 1) {
        return info;
    }

    if (move.size() == 2) {
        if (move[1] == '\'') {
            info.prime = true;
        } else if (move[1] == '2') {
            info.doubleTurn = true;
        } else {
            info.valid = false;
        }
        return info;
    }

    info.valid = false;
    return info;
}

MoveAxis axisForFace(char face) {
    MoveAxis axis;
    switch (face) {
    case 'F':
        axis.axis = 'z';
        axis.layerCoord = 1;
        axis.ax = 0.0f;
        axis.ay = 0.0f;
        axis.az = 1.0f;
        break;
    case 'B':
        axis.axis = 'z';
        axis.layerCoord = -1;
        axis.ax = 0.0f;
        axis.ay = 0.0f;
        axis.az = 1.0f;
        break;
    case 'U':
        axis.axis = 'y';
        axis.layerCoord = 1;
        axis.ax = 0.0f;
        axis.ay = 1.0f;
        axis.az = 0.0f;
        break;
    case 'D':
        axis.axis = 'y';
        axis.layerCoord = -1;
        axis.ax = 0.0f;
        axis.ay = 1.0f;
        axis.az = 0.0f;
        break;
    case 'R':
        axis.axis = 'x';
        axis.layerCoord = 1;
        axis.ax = 1.0f;
        axis.ay = 0.0f;
        axis.az = 0.0f;
        break;
    case 'L':
        axis.axis = 'x';
        axis.layerCoord = -1;
        axis.ax = 1.0f;
        axis.ay = 0.0f;
        axis.az = 0.0f;
        break;
    default:
        break;
    }
    return axis;
}

bool cubieMatchesLayer(int x, int y, int z, const MoveAxis& axis) {
    switch (axis.axis) {
    case 'x':
        return x == axis.layerCoord;
    case 'y':
        return y == axis.layerCoord;
    case 'z':
        return z == axis.layerCoord;
    default:
        return false;
    }
}

} // namespace

Viewer::Viewer()
    : m_initialized(false)
    , m_lastX(0.0)
    , m_lastY(0.0)
    , m_dragging(false)
    , m_yaw(45.0f)
    , m_pitch(25.0f)
    , m_distance(5.0f)
    , m_animating(false)
    , m_animationMove("")
    , m_currentAngle(0.0f)
    , m_targetAngle(0.0f)
    , m_animationSpeed(180.0f)
    , m_cube() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    m_initialized = true;
}

Viewer::~Viewer() {
    if (m_initialized) {
        glfwTerminate();
    }
}

void Viewer::run() {
    if (!m_initialized) {
        throw std::runtime_error("GLFW was not initialized");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 1);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rubik Viewer", nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, Viewer::cursorPosCallback);
    glfwSetMouseButtonCallback(window, Viewer::mouseButtonCallback);
    glfwSetScrollCallback(window, Viewer::scrollCallback);
    glfwSetKeyCallback(window, Viewer::keyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glShadeModel(GL_SMOOTH);
    // glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const double deltaTime = now - previousTime;
        previousTime = now;

        updateAnimation(deltaTime);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        height = height == 0 ? 1 : height;
        const float aspect = static_cast<float>(width) / static_cast<float>(height);

        glViewport(0, 0, width, height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspect, 0.5, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera Orbit
    const float yawRad = m_yaw * kDegToRad;
    const float pitchRad = m_pitch * kDegToRad;
    const float cosPitch = std::cos(pitchRad);
    const float sinPitch = std::sin(pitchRad);
    const float cosYaw = std::cos(yawRad);
    const float sinYaw = std::sin(yawRad);

    const float camX = m_distance * cosPitch * sinYaw;
    const float camY = m_distance * sinPitch;
    const float camZ = m_distance * cosPitch * cosYaw;

    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawCube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
}

void Viewer::applyMovement(const std::string& move) {
    if (m_animating) {
        std::cout << "Animation already in progress." << std::endl;
        return;
    }

    const MoveInfo info = parseMove(move);
    if (!info.valid) {
        std::cout << "Move " << move << " not implemented yet." << std::endl;
        return;
    }

    const MoveAxis axis = axisForFace(info.face);
    if (axis.axis == 0) {
        std::cout << "Move " << move << " not implemented yet." << std::endl;
        return;
    }

    float baseAngle = (axis.layerCoord > 0) ? -90.0f : 90.0f;
    if (info.doubleTurn) {
        baseAngle *= 2.0f;
    }
    if (info.prime) {
        baseAngle = -baseAngle;
    }

    std::cout << "Applying move: " << move << std::endl;
    startAnimation(move, baseAngle);
}

void Viewer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (!viewer) {
        return;
    }
    viewer->handleCursorPos(xpos, ypos);
}

void Viewer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (!viewer) {
        return;
    }
    viewer->handleMouseButton(window, button, action, mods);
}

void Viewer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (!viewer) {
        return;
    }
    viewer->handleScroll(yoffset);
}

void Viewer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    // cout every information
    std::cout << "Key: " << key << ", Action: " << action << ", Mods: " << mods << std::endl;

    if (action == GLFW_PRESS) {
        const bool withShift = (mods & GLFW_MOD_SHIFT) != 0;
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (key == GLFW_KEY_U) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "U'" : "U");
            }
        } else if (key == GLFW_KEY_D) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "D'" : "D");
            }
        } else if (key == GLFW_KEY_L) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "L'" : "L");
            }
        } else if (key == GLFW_KEY_R) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "R'" : "R");
            }
        } else if (key == GLFW_KEY_F) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "F'" : "F");
            }
        } else if (key == GLFW_KEY_B) {
            Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
            if (viewer) {
                viewer->applyMovement(withShift ? "B'" : "B");
            }
        }
    }
}

void Viewer::handleCursorPos(double xpos, double ypos) {
    if (m_dragging) {
        const float sensitivity = 0.3f;
        const float deltaX = static_cast<float>(xpos - m_lastX);
        const float deltaY = static_cast<float>(ypos - m_lastY);
        m_yaw += deltaX * sensitivity;
        m_pitch -= deltaY * sensitivity;
        m_pitch = clampf(m_pitch, -89.0f, 89.0f);
    }

    m_lastX = xpos;
    m_lastY = ypos;
}

void Viewer::handleMouseButton(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos = 0.0;
            double ypos = 0.0;
            glfwGetCursorPos(window, &xpos, &ypos);
            m_lastX = xpos;
            m_lastY = ypos;
            m_dragging = true;
        } else if (action == GLFW_RELEASE) {
            m_dragging = false;
        }
    }
}

void Viewer::handleScroll(double yoffset) {
    m_distance -= static_cast<float>(yoffset);
    m_distance = clampf(m_distance, 2.0f, 20.0f);
}

void Viewer::startAnimation(const std::string& move, float targetAngle) {
    m_animating = true;
    m_animationMove = move;
    m_currentAngle = 0.0f;
    m_targetAngle = targetAngle;
}

void Viewer::updateAnimation(double deltaTime) {
    if (!m_animating) {
        return;
    }

    const float direction = (m_targetAngle >= 0.0f) ? 1.0f : -1.0f;
    const float angleStep = static_cast<float>(deltaTime) * m_animationSpeed * direction;
    m_currentAngle += angleStep;

    const bool reachedTarget = (direction > 0.0f && m_currentAngle >= m_targetAngle) ||
        (direction < 0.0f && m_currentAngle <= m_targetAngle);

    if (reachedTarget) {
        m_currentAngle = m_targetAngle;
        finalizeAnimation();
    }
}

void Viewer::finalizeAnimation() {
    const MoveInfo info = parseMove(m_animationMove);
    if (info.valid) {
        int turns = 0;
        if (info.doubleTurn) {
            turns = 2;
        } else if (info.prime) {
            turns = -1;
        } else {
            turns = 1;
        }
        m_cube.applyMove(info.face, turns);
    }

    m_animating = false;
    m_animationMove.clear();
    m_currentAngle = 0.0f;
    m_targetAngle = 0.0f;
}

void Viewer::drawCube() const {
    const float cubieSize = 0.28f;

    bool hasActiveMove = false;
    MoveAxis activeAxis;
    if (m_animating) {
        const MoveInfo info = parseMove(m_animationMove);
        if (info.valid) {
            activeAxis = axisForFace(info.face);
            hasActiveMove = activeAxis.axis != 0;
        }
    }

    for (int y = -1; y <= 1; ++y) {
        for (int z = -1; z <= 1; ++z) {
            for (int x = -1; x <= 1; ++x) {
                glPushMatrix();
                if (hasActiveMove && cubieMatchesLayer(x, y, z, activeAxis)) {
                    glRotatef(m_currentAngle, activeAxis.ax, activeAxis.ay, activeAxis.az);
                }

                drawCubie(x, y, z, cubieSize);
                glPopMatrix();
            }
        }
    }
}

void Viewer::drawCubie(int xi, int yi, int zi, float size) const {
    glPushMatrix();
    const float spacing = size * 1.05f;
    glTranslatef(static_cast<float>(xi) * spacing, static_cast<float>(yi) * spacing, static_cast<float>(zi) * spacing);

    glBegin(GL_QUADS);

    const bool onFront = zi == 1;
    const bool onBack = zi == -1;
    const bool onRight = xi == 1;
    const bool onLeft = xi == -1;
    const bool onUp = yi == 1;
    const bool onDown = yi == -1;

    const char frontSticker = onFront ? m_cube.stickerFor(xi, yi, zi, 'F') : 0;
    const char backSticker = onBack ? m_cube.stickerFor(xi, yi, zi, 'B') : 0;
    const char leftSticker = onLeft ? m_cube.stickerFor(xi, yi, zi, 'L') : 0;
    const char rightSticker = onRight ? m_cube.stickerFor(xi, yi, zi, 'R') : 0;
    const char upSticker = onUp ? m_cube.stickerFor(xi, yi, zi, 'U') : 0;
    const char downSticker = onDown ? m_cube.stickerFor(xi, yi, zi, 'D') : 0;

    setStickerColor(frontSticker);
    drawStickeredFace(size, 'F');

    setStickerColor(backSticker);
    drawStickeredFace(size, 'B');

    setStickerColor(leftSticker);
    drawStickeredFace(size, 'L');

    setStickerColor(rightSticker);
    drawStickeredFace(size, 'R');

    setStickerColor(upSticker);
    drawStickeredFace(size, 'U');

    setStickerColor(downSticker);
    drawStickeredFace(size, 'D');

    glEnd();

    glPopMatrix();
}

Viewer::CubeState::CubeState() {
    reset();
}

void Viewer::CubeState::reset() {
    const std::array<char, 6> solved = { 'U', 'D', 'F', 'B', 'L', 'R' };
    for (std::size_t face = 0; face < faces.size(); ++face) {
        faces[face].fill(solved[face]);
    }
}

char Viewer::CubeState::stickerFor(int x, int y, int z, char face) const {
    const FaceIndex faceIdx = faceIndexFromLetter(face);
    const auto& stickers = faces[static_cast<int>(faceIdx)];

    int row = 0;
    int col = 0;

    switch (face) {
    case 'F':
        row = 1 - y;
        col = x + 1;
        break;
    case 'B':
        row = 1 - y;
        col = 1 - x;
        break;
    case 'L':
        row = 1 - y;
        col = z + 1;
        break;
    case 'R':
        row = 1 - y;
        col = 1 - z;
        break;
    case 'U':
        row = z + 1;
        col = x + 1;
        break;
    case 'D':
        row = 1 - z;
        col = x + 1;
        break;
    default:
        return 0;
    }

    return stickers[indexFromRowCol(row, col)];
}

void Viewer::CubeState::applyMove(char face, int turns) {
    if (turns == 0) {
        return;
    }

    char axis = 0;
    int layerCoord = 0;

    switch (face) {
    case 'F':
        axis = 'z';
        layerCoord = 1;
        break;
    case 'B':
        axis = 'z';
        layerCoord = -1;
        break;
    case 'U':
        axis = 'y';
        layerCoord = 1;
        break;
    case 'D':
        axis = 'y';
        layerCoord = -1;
        break;
    case 'R':
        axis = 'x';
        layerCoord = 1;
        break;
    case 'L':
        axis = 'x';
        layerCoord = -1;
        break;
    default:
        return;
    }

    int normalized = turns % 4;
    if (normalized < 0) {
        normalized += 4;
    }

    if (normalized == 0) {
        return;
    }

    if (normalized == 2) {
        rotateLayer(axis, layerCoord, 2);
        return;
    }

    const int direction = (layerCoord > 0) ? 1 : -1;
    const int quarterTurns = (normalized == 1) ? -direction : direction;
    rotateLayer(axis, layerCoord, quarterTurns);
}

Viewer::CubeState::FaceIndex Viewer::CubeState::faceIndexFromLetter(char face) {
    switch (face) {
    case 'U':
        return Up;
    case 'D':
        return Down;
    case 'F':
        return Front;
    case 'B':
        return Back;
    case 'L':
        return Left;
    case 'R':
        return Right;
    default:
        return Front;
    }
}

int Viewer::CubeState::indexFromRowCol(int row, int col) {
    return row * 3 + col;
}

void Viewer::CubeState::positionAndNormalForSticker(FaceIndex face, int index, int& px, int& py, int& pz, int& nx, int& ny, int& nz) {
    const int row = index / 3;
    const int col = index % 3;

    switch (face) {
    case Up:
        px = col - 1;
        py = 1;
        pz = row - 1;
        nx = 0;
        ny = 1;
        nz = 0;
        break;
    case Down:
        px = col - 1;
        py = -1;
        pz = 1 - row;
        nx = 0;
        ny = -1;
        nz = 0;
        break;
    case Front:
        px = col - 1;
        py = 1 - row;
        pz = 1;
        nx = 0;
        ny = 0;
        nz = 1;
        break;
    case Back:
        px = 1 - col;
        py = 1 - row;
        pz = -1;
        nx = 0;
        ny = 0;
        nz = -1;
        break;
    case Left:
        px = -1;
        py = 1 - row;
        pz = col - 1;
        nx = -1;
        ny = 0;
        nz = 0;
        break;
    case Right:
        px = 1;
        py = 1 - row;
        pz = 1 - col;
        nx = 1;
        ny = 0;
        nz = 0;
        break;
    }
}

Viewer::CubeState::FaceIndex Viewer::CubeState::faceFromNormal(int nx, int ny, int nz) {
    if (nx == 1) {
        return Right;
    }
    if (nx == -1) {
        return Left;
    }
    if (ny == 1) {
        return Up;
    }
    if (ny == -1) {
        return Down;
    }
    if (nz == 1) {
        return Front;
    }
    return Back;
}

int Viewer::CubeState::indexFromPosition(FaceIndex face, int px, int py, int pz) {
    int row = 0;
    int col = 0;

    switch (face) {
        case Up:
            row = pz + 1;
            col = px + 1;
            break;
        case Down:
            row = 1 - pz;
            col = px + 1;
            break;
        case Front:
            row = 1 - py;
            col = px + 1;
            break;
        case Back:
            row = 1 - py;
            col = 1 - px;
            break;
        case Left:
            row = 1 - py;
            col = pz + 1;
            break;
        case Right:
            row = 1 - py;
            col = 1 - pz;
            break;
    }

    return indexFromRowCol(row, col);
}

void Viewer::CubeState::rotateVectorPositive90(int& x, int& y, int& z, char axis) {
    int tmp = 0;
    switch (axis) {
    case 'x':
        tmp = y;
        y = -z;
        z = tmp;
        break;
    case 'y':
        tmp = x;
        x = z;
        z = -tmp;
        break;
    case 'z':
        tmp = x;
        x = -y;
        y = tmp;
        break;
    default:
        break;
    }
}

void Viewer::CubeState::rotateLayer(char axis, int layerCoord, int quarterTurns) {
    int turns = quarterTurns % 4;
    if (turns < 0) {
        turns += 4;
    }
    if (turns == 0) {
        return;
    }

    const auto original = faces;
    auto updated = faces;

    for (int faceIdx = 0; faceIdx < 6; ++faceIdx) {
        for (int stickerIdx = 0; stickerIdx < 9; ++stickerIdx) {
            int px = 0;
            int py = 0;
            int pz = 0;
            int nx = 0;
            int ny = 0;
            int nz = 0;
            positionAndNormalForSticker(static_cast<FaceIndex>(faceIdx), stickerIdx, px, py, pz, nx, ny, nz);

            const int coord = (axis == 'x') ? px : (axis == 'y' ? py : pz);
            if (coord != layerCoord) {
                continue;
            }

            int rpx = px;
            int rpy = py;
            int rpz = pz;
            int rnx = nx;
            int rny = ny;
            int rnz = nz;

            for (int i = 0; i < turns; ++i) {
                rotateVectorPositive90(rpx, rpy, rpz, axis);
                rotateVectorPositive90(rnx, rny, rnz, axis);
            }

            const FaceIndex newFace = faceFromNormal(rnx, rny, rnz);
            const int newIndex = indexFromPosition(newFace, rpx, rpy, rpz);
            updated[static_cast<int>(newFace)][newIndex] = original[faceIdx][stickerIdx];
        }
    }

    faces = updated;
}

#else

Viewer::Viewer()
    : m_initialized(false)
    , m_lastX(0.0)
    , m_lastY(0.0)
    , m_dragging(false)
    , m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_distance(5.0f)
    , m_animating(false)
    , m_animationMove("")
    , m_currentAngle(0.0f)
    , m_targetAngle(0.0f)
    , m_animationSpeed(180.0f)
    , m_cube() {}

Viewer::~Viewer() = default;

void Viewer::run() {
    throw std::runtime_error("Graphics support not enabled. Rebuild with GRAPHICS=1");
}

void Viewer::applyMovement(const std::string& move) {
    (void)move;
}

void Viewer::startAnimation(const std::string& move, float targetAngle) {
    (void)move;
    (void)targetAngle;
}

void Viewer::updateAnimation(double) {}

void Viewer::finalizeAnimation() {}

void Viewer::drawCube() const {}

void Viewer::drawCubie(int, int, int, float) const {}

#endif
