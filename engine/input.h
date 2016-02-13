#ifndef INPUT_H
#define INPUT_H

namespace moar
{

class Input
{
public:
    explicit Input();
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

private:
    double x = 0.0;
    double y = 0.0;
    double deltaX = 0.0;
    double deltaY = 0.0;
    double sensitivity = 0.0;
    float movementSpeed = 0.0;
};

} // moar

#endif // INPUT_H
