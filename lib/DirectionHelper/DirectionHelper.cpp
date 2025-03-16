#include <Arduino.h>
#include <AppHelper.cpp>

#define DRAW_DEBUG 0

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
    int32_t u = 22;
    int32_t topleft_x = 32; // WIDTH*2/15
    int32_t topleft_y = 16; // WIDTH*1/15
    #if DRAW_DEBUG
    _lcd.drawRect(topleft_x, topleft_y, 8*u, 8*u);
    _lcd.drawCenterString("00000", screenHalfWidth, topleft_x + 8*u + 5);
    #endif
    switch (static_cast<Maneuver>(std::stoi(value)))
    {
        case TURN_LEFT:
            _lcd.fillTriangle(topleft_x, topleft_y + 2.5*u, topleft_x + 3*u, topleft_y, topleft_x + 3*u, topleft_y + 5*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3*u, topleft_y + 2*u, 2*u, u, WRITE_COLOR);
            _lcd.fillArc(topleft_x + 5*u, topleft_y + 4*u, u, 2*u, 270, 360, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 6*u, topleft_y + 4*u, u, 3*u, WRITE_COLOR);
            break;
        case TURN_RIGHT:
            _lcd.fillTriangle(topleft_x + 8*u, topleft_y + 2.5*u, topleft_x + 5*u, topleft_y, topleft_x + 5*u, topleft_y + 5*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3*u, topleft_y + 2*u, 2*u, u, WRITE_COLOR);
            _lcd.fillArc(topleft_x + 3*u, topleft_y + 4*u, u, 2*u, 180, 270, WRITE_COLOR);
            _lcd.fillRect(topleft_x + u, topleft_y + 4*u, u, 3*u, WRITE_COLOR);
            break;
        case TURN_SLIGHT_LEFT:
            _lcd.fillTriangle(topleft_x + u, topleft_y + u, topleft_x + 4*u, topleft_y + u, topleft_x + u, topleft_y + 4*u, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 95, topleft_y + 80, topleft_x + 45, topleft_y + 30, topleft_x + 80, topleft_y + 95, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 40, topleft_y + 55, topleft_x + 45, topleft_y + 30, topleft_x + 80, topleft_y + 95, WRITE_COLOR);
            _lcd.fillArc(topleft_x + 63, topleft_y + 5*u, u, 2*u, 312, 360, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 85, topleft_y + 5*u, u, 2.5*u, WRITE_COLOR);
            break;
        case TURN_SLIGHT_RIGHT:
            _lcd.fillTriangle(topleft_x + 7*u, topleft_y + u, topleft_x + 4*u, topleft_y + u, topleft_x + 7*u, topleft_y + 4*u, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 8*u - 79, topleft_y + 94, topleft_x + 8*u - 45, topleft_y + 60, topleft_x + 8*u - 95, topleft_y + 79, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 8*u - 61, topleft_y + 45, topleft_x + 8*u - 45, topleft_y + 60, topleft_x + 8*u - 95, topleft_y + 79, WRITE_COLOR);
            _lcd.fillArc(topleft_x + 8*u - 63, topleft_y + 5*u, u, 2*u, 180, 224, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 8*u - 107, topleft_y + 5*u, u, 2.5*u, WRITE_COLOR);
            break;
        case RAMP_LEFT:
            _lcd.fillTriangle(topleft_x + 4*u, topleft_y + u, topleft_x + 2*u, topleft_y + 3*u, topleft_x + 6*u, topleft_y + 3*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3.5*u, topleft_y + 2*u, u, 5*u, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 80, topleft_y + 73, topleft_x + 41, topleft_y + 125, topleft_x + 80, topleft_y + 98, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 52, topleft_y + 136, topleft_x + 41, topleft_y + 125, topleft_x + 80, topleft_y + 98, WRITE_COLOR);
            break;
        case RAMP_RIGHT:
            _lcd.fillTriangle(topleft_x + 4*u, topleft_y + u, topleft_x + 2*u, topleft_y + 3*u, topleft_x + 6*u, topleft_y + 3*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3.5*u, topleft_y + 2*u, u, 5*u, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 95, topleft_y + 72, topleft_x + 95, topleft_y + 97, topleft_x + 135, topleft_y + 125, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 124, topleft_y + 136, topleft_x + 95, topleft_y + 97, topleft_x + 135, topleft_y + 125, WRITE_COLOR);
            break;
        case MERGE:
            _lcd.fillTriangle(topleft_x + 4*u, topleft_y + u, topleft_x + 2*u, topleft_y + 3*u, topleft_x + 6*u, topleft_y + 3*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3.5*u, topleft_y + 2*u, u, 5*u, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 80, topleft_y + 73, topleft_x + 41, topleft_y + 125, topleft_x + 80, topleft_y + 98, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 52, topleft_y + 136, topleft_x + 41, topleft_y + 125, topleft_x + 80, topleft_y + 98, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 95, topleft_y + 72, topleft_x + 95, topleft_y + 97, topleft_x + 135, topleft_y + 125, WRITE_COLOR);
            _lcd.fillTriangle(topleft_x + 124, topleft_y + 136, topleft_x + 95, topleft_y + 97, topleft_x + 135, topleft_y + 125, WRITE_COLOR);
            break;
        case STRAIGHT:
            _lcd.fillTriangle(topleft_x + 4*u, topleft_y + u, topleft_x + 2*u, topleft_y + 3*u, topleft_x + 6*u, topleft_y + 3*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3.5*u, topleft_y + 2*u, u, 5*u, WRITE_COLOR);
            break;
        case DEPART:
            _lcd.fillTriangle(topleft_x + 7*u, topleft_y + 1.5*u, topleft_x + 5*u, topleft_y + 1.5*u, topleft_x + 7*u, topleft_y + 3.5*u, WRITE_COLOR);
            _lcd.fillRect(topleft_x + 3*u, topleft_y + 6*u, 2*u, u, WRITE_COLOR);
            _lcd.fillArc(topleft_x, topleft_y + 6*u, 3.7*u, 4.3*u, 285, 360, WRITE_COLOR);
            _lcd.fillArc(topleft_x + 8*u, topleft_y + 6*u, 3.7*u, 4.3*u, 180, 250, WRITE_COLOR);
            break;
        case UNKNOWN:
        case NONE:
        default:
            _lcd.drawCenterString("X", topleft_x + 4*u, topleft_y + 4*u);
            break;
    }
}