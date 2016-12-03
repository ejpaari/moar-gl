#ifndef INPUT_H
#define INPUT_H

#include "gui.h"

#include <GLFW/glfw3.h>

#include <map>

namespace moar
{

class Input
{
  friend class Engine;

public:
    struct KeyStatus {
      bool pressed;
      bool down;
    };
  
    explicit Input();
    ~Input();
    Input(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(const Input&) = delete;
    Input& operator=(Input&&) = delete;

    void setCursorPosition(double x, double y);
    void setSensitivity(double sensitivity);
    void setMovementSpeed(float speed);

    double getCursorDeltaX() const;
    double getCursorDeltaY() const;
    float getMovementSpeed() const;

    bool isKeyPressed(int key);
    bool isKeyDown(int key);

private:
    void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void reset();
    void resetCursorDelta();
    void setGUI(GUI* gui);

    double x = 0.0;
    double y = 0.0;
    double deltaX = 0.0;
    double deltaY = 0.0;
    double sensitivity = 0.0;
    float movementSpeed = 0.0;
    std::map<int, KeyStatus> keys;
    GUI* gui = nullptr;
};

} // moar

#endif // INPUT_H
