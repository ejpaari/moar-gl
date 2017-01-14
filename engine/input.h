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

    void setCursorPosition(float x, float y);
	void setCursorPosition(double x, double y);
    void setSensitivity(float sensitivity);
    void setMovementSpeed(float speed);

	float getCursorDeltaX() const;
	float getCursorDeltaY() const;
	float getMovementSpeed() const;

    bool isKeyPressed(int key);
    bool isKeyDown(int key);

private:
    void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void reset();
    void resetCursorDelta();
    void setGUI(GUI* gui);

    float x = 0.0;
	float y = 0.0;
	float deltaX = 0.0;
	float deltaY = 0.0;
	float sensitivity = 0.0;
	float movementSpeed = 0.0;
    std::map<int, KeyStatus> keys;
    GUI* gui = nullptr;
};

} // moar

#endif // INPUT_H
