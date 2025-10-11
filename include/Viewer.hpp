#ifndef RUBIK_VIEWER_HPP
#define RUBIK_VIEWER_HPP

class Viewer {
public:
    Viewer();
    ~Viewer();

    void run();

private:
    bool m_initialized;
};

#endif // RUBIK_VIEWER_HPP
