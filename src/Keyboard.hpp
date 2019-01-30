#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

namespace Keyboard {

    extern bool keyPressed[256];
    extern bool increase;

    void keyPress(unsigned char key, int x, int y);
    void keyRelease(unsigned char key, int x, int y);

}

#endif
