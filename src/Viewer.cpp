#include "Viewer.hpp"

#include <stdexcept>
#include <cmath>
#include <algorithm>

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

constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;

float clampf(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

void drawCube() {
    glBegin(GL_QUADS);

    // Front face (Green)
    glColor3f(0.0f, 0.6f, 0.2f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face (Blue)
    glColor3f(0.1f, 0.2f, 0.7f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face (Orange)
    glColor3f(0.9f, 0.4f, 0.0f);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Right face (Red)
    glColor3f(0.8f, 0.0f, 0.0f);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // Top face (White)
    glColor3f(0.9f, 0.9f, 0.9f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Bottom face (Yellow)
    glColor3f(1.0f, 0.8f, 0.0f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();
}

} // namespace

Viewer::Viewer()
    : m_initialized(false)
    , m_lastX(0.0)
    , m_lastY(0.0)
    , m_dragging(false)
    , m_yaw(45.0f)
    , m_pitch(25.0f)
    , m_distance(5.0f) {
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

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        height = height == 0 ? 1 : height;
        const float aspect = static_cast<float>(width) / static_cast<float>(height);

        glViewport(0, 0, width, height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspect, 0.1, 100.0);

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

#else

Viewer::Viewer()
    : m_initialized(false)
    , m_lastX(0.0)
    , m_lastY(0.0)
    , m_dragging(false)
    , m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_distance(5.0f) {}

Viewer::~Viewer() = default;

void Viewer::run() {
    throw std::runtime_error("Graphics support not enabled. Rebuild with GRAPHICS=1");
}

#endif
