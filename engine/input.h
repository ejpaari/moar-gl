#ifndef INPUT_H
#define INPUT_H

namespace moar
{

class Input
{
public:
    Input();
    ~Input();
    Input(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(const Input&) = delete;
    Input& operator=(Input&&) = delete;

    void setCursorPosition(double x, double y);
    void setSensitivity(double sensitivity) { this->sensitivity = sensitivity; }
    void setMovementSpeed(float speed) { movementSpeed = speed; }

    double getCursorDeltaX() const;
    double getCursorDeltaY() const;
    double getSensitivity() const { return sensitivity; }
    float getMovementSpeed() const { return movementSpeed; }

private:
    double x;
    double y;
    double deltaX;
    double deltaY;
    double sensitivity;
    float movementSpeed;
};

} // moar

#endif // INPUT_H
