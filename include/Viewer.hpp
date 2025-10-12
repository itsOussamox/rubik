#ifndef RUBIK_VIEWER_HPP
#define RUBIK_VIEWER_HPP

#include <array>
#include <string>

class Viewer {
public:
    Viewer();
    ~Viewer();

    void run();
    void applyMovement(const std::string& move);

private:
    bool m_initialized;
    double m_lastX;
    double m_lastY;
    bool m_dragging;
    float m_yaw;
    float m_pitch;
    float m_distance;
    bool m_animating;
    std::string m_animationMove;
    float m_currentAngle;
    float m_targetAngle;
    float m_animationSpeed; // degrees per second

    struct CubeState {
        std::array<std::array<char, 9>, 6> faces;

        enum FaceIndex {
            Up = 0,
            Down = 1,
            Front = 2,
            Back = 3,
            Left = 4,
            Right = 5
        };

        CubeState();
        void reset();
        char stickerFor(int x, int y, int z, char face) const;
        void applyMove(char face, int turns);

    private:
        static FaceIndex faceIndexFromLetter(char face);
        static int indexFromRowCol(int row, int col);
        static void positionAndNormalForSticker(FaceIndex face, int index, int& px, int& py, int& pz, int& nx, int& ny, int& nz);
        static FaceIndex faceFromNormal(int nx, int ny, int nz);
        static int indexFromPosition(FaceIndex face, int px, int py, int pz);
        static void rotateVectorPositive90(int& x, int& y, int& z, char axis);
        void rotateLayer(char axis, int layerCoord, int quarterTurns);
    };

    CubeState m_cube;

    void startAnimation(const std::string& move, float targetAngle);
    void updateAnimation(double deltaTime);
    void finalizeAnimation();
    void drawCube() const;
    void drawCubie(int xi, int yi, int zi, float size) const;

#ifdef RUBIK_WITH_GRAPHICS
    static void cursorPosCallback(struct GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(struct GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(struct GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);
    void handleCursorPos(double xpos, double ypos);
    void handleMouseButton(struct GLFWwindow* window, int button, int action, int mods);
    void handleScroll(double yoffset);
#endif
};

#endif // RUBIK_VIEWER_HPP
