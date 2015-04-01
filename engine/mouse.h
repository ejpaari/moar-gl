#ifndef MOUSE_H
#define MOUSE_H

namespace moar
{

class Mouse
{
public:
    Mouse();
    ~Mouse();
    Mouse(const Mouse&) = delete;
    Mouse(Mouse&&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    Mouse& operator=(Mouse&&) = delete;

    void setPosition(double x, double y);
    void setSensitivity(double sensitivity) { this->sensitivity = sensitivity; }

    double getDeltaX() const;
    double getDeltaY() const;
    double getSensitivity() const { return sensitivity; }

private:
    double x;
    double y;
    double deltaX;
    double deltaY;
    double sensitivity;
};

} // moar

#endif // MOUSE_H
