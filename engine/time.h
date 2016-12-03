#ifndef TIME_H
#define TIME_H

namespace moar
{

class Time
{
public:
    explicit Time();
    ~Time();
    Time(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(const Time&) = delete;
    Time& operator=(Time&&) = delete;

    void update();
    float getTime() const;
    float getDelta() const;

private:
    float time = 0.0;
    float delta = 0.0;
};

} // moar

#endif // TIME_H
