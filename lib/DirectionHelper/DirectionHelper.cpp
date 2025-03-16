#include <Arduino.h>
#include <AppHelper.cpp>

enum Maneuver {
    UNKNOWN = -1,
    TURN_LEFT,
    TURN_RIGHT,
    TURN_SLIGHT_LEFT,
    TURN_SLIGHT_RIGHT,
    RAMP_LEFT,
    RAMP_RIGHT,
    MERGE,
    STRAIGHT,
    DEPART,
    NONE
};

void draw(const char *value);

void draw(const char *value) {
    switch (static_cast<Maneuver>(std::stoi(value)))
    {
        case TURN_LEFT:
            _lcd.drawCenterString("TURN_LEFT", screenHalfWidth, screenHalfHeight);
            break;
        case TURN_RIGHT:
            _lcd.drawCenterString("TURN_RIGHT", screenHalfWidth, screenHalfHeight);
            break;
        case TURN_SLIGHT_LEFT:
            _lcd.drawCenterString("TURN_SLIGHT_LEFT", screenHalfWidth, screenHalfHeight);
            break;
        case TURN_SLIGHT_RIGHT:
            _lcd.drawCenterString("TURN_SLIGHT_RIGHT", screenHalfWidth, screenHalfHeight);
            break;
        case RAMP_LEFT:
            _lcd.drawCenterString("RAMP_LEFT", screenHalfWidth, screenHalfHeight);
            break;
        case RAMP_RIGHT:
            _lcd.drawCenterString("RAMP_RIGHT", screenHalfWidth, screenHalfHeight);
            break;
        case MERGE:
            _lcd.drawCenterString("MERGE", screenHalfWidth, screenHalfHeight);
            break;
        case STRAIGHT:
            _lcd.drawCenterString("RAMP_RIGHT", screenHalfWidth, screenHalfHeight);
            break;
        case DEPART:
            _lcd.drawCenterString("DEPART", screenHalfWidth, screenHalfHeight);
            break;
        case UNKNOWN:
        default:
            _lcd.drawCenterString("X", screenHalfWidth, screenHalfHeight);
            break;
    }
}