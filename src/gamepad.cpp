#include "gamepad.h"

#include <complex>

joystick gamepad::get_left_stick() const {
    joystick stick;
    stick.x = left_stick_x / 32767.0f;
    stick.y = left_stick_y / 32767.0f;
    return stick;
}

joystick gamepad::get_right_stick() const {
    joystick stick;
    stick.x = right_stick_x / 32767.0f;
    stick.y = right_stick_y / 32767.0f;
    return stick;
}

void gamepad::process_event(const SDL_GamepadAxisEvent &event) {
    float value = event.value;
    if (std::abs(value) < 4096) value = 0;

    switch (event.axis) {
        case SDL_GAMEPAD_AXIS_LEFTX:
            left_stick_x = value;
            break;
        case SDL_GAMEPAD_AXIS_LEFTY:
            left_stick_y = value;
            break;
        case SDL_GAMEPAD_AXIS_RIGHTX:
            right_stick_x = value;
            break;
        case SDL_GAMEPAD_AXIS_RIGHTY:
            right_stick_y = value;
            break;
        default: ;
    }
}
