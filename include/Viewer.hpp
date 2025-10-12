#ifndef RUBIK_VIEWER_HPP
#define RUBIK_VIEWER_HPP

class Viewer {
public:
    Viewer();
    ~Viewer();

    void run();

private:
    bool m_initialized;
    double m_lastX;
    double m_lastY;
    bool m_dragging;
    float m_yaw;
    float m_pitch;
    float m_distance;
#ifdef RUBIK_WITH_GRAPHICS
    static void cursorPosCallback(struct GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(struct GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(struct GLFWwindow* window, double xoffset, double yoffset);
    void handleCursorPos(double xpos, double ypos);
    void handleMouseButton(struct GLFWwindow* window, int button, int action, int mods);
    void handleScroll(double yoffset);
#endif
};

#endif // RUBIK_VIEWER_HPP
