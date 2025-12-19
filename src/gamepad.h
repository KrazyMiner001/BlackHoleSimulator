#ifndef BLACKHOLESIMULATOR_GAMEPAD_H
#define BLACKHOLESIMULATOR_GAMEPAD_H
#include "SDL3/SDL_events.h"

struct joystick {
    float x;
    float y;
};

class gamepad {
public:
    void process_event(const SDL_GamepadAxisEvent &event);

    joystick get_left_stick() const;

    joystick get_right_stick() const;

private:
    float left_stick_x = 0;
    float left_stick_y = 0;
    float right_stick_x = 0;
    float right_stick_y = 0;
};


#endif //BLACKHOLESIMULATOR_GAMEPAD_H
