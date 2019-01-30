#pragma once

#ifndef MOUSE_H
#define MOUSE_H

namespace Mouse {

    extern bool left_press;
    extern bool right_press;

    void mouseButton(int button, int state, int x, int y);
    void mouseMove(int x, int y);

}

#endif