#include <stdlib.h>

#include "Display.hpp"
#include "ObjectLoader.hpp"
#include "Camera.hpp"

namespace Keyboard {

    /* Current status of each key */
    bool keyPressed[256] = { false };

    /* Toggle for increasing/decreasing color and clipping values */
    bool increase = true;


    /* 
     * Handles any key presses that don't require simultaneous processing,
     * and registers keys as pressed for those that do. For example, moving
     * the camera left and right simultaneously should cancel out to no
     * movement, whereas toggling lighting modes has no such special logic
     * for simultaneous events.
     */
    void keyPress(unsigned char key, int x, int y) {
        /* Escape key; terminate program */
        if (key == 27) exit(0);

        /* Camera translation */
        if (key == 'w' || key == 'W') keyPressed['w'] = true;
        if (key == 's' || key == 'S') keyPressed['s'] = true;
        if (key == 'a' || key == 'A') keyPressed['a'] = true;
        if (key == 'd' || key == 'D') keyPressed['d'] = true;

        /* Toggle for color/clipping commands */
        if (key == 't') increase = !increase;

        /* Color controls */
        if (key == 'r' || key == 'R') keyPressed['r'] = true;
        if (key == 'g' || key == 'G') keyPressed['g'] = true;
        if (key == 'b' || key == 'B') keyPressed['b'] = true;

        /* Toggle lighting modes (3 total) */
        if (key == 'l' || key == 'L')
            Display::light_on == 2 ? Display::light_on = 0 : Display::light_on++;

        /* Toggle smooth/flat shading */
        if (key == 'p') Display::smooth_shading = !Display::smooth_shading;

        /* Clipping controls */
        if (key == 'n' || key == 'N') keyPressed['n'] = true;
        if (key == 'f' || key == 'F') keyPressed['f'] = true;

        /* Polygon mode flags */
        if (key == '1') Display::render_mode = SOLID;
        else if (key == '2') Display::render_mode = WIREFRAME;
        else if (key == '3') Display::render_mode = POINTS;

        /* Switch between pre-loaded models */
        if (key == '9') ObjectLoader::changeModel("models\\bunny.obj");
        else if (key == '0') ObjectLoader::changeModel("models\\cactus.obj");

        /* Space */
        if (key == ' ')	Camera::resetCamera();
    }


    /* 
     * Sets the status of released keys to unpressed.
     */
    void keyRelease(unsigned char key, int x, int y) {
        if (key == 'w' || key == 'W') keyPressed['w'] = false;
        if (key == 's' || key == 'S') keyPressed['s'] = false;
        if (key == 'a' || key == 'A') keyPressed['a'] = false;
        if (key == 'd' || key == 'D') keyPressed['d'] = false;
        if (key == 'q' || key == 'Q') keyPressed['q'] = false;
        if (key == 'e' || key == 'E') keyPressed['e'] = false;

        if (key == 'r' || key == 'R') keyPressed['r'] = false;
        if (key == 'g' || key == 'G') keyPressed['g'] = false;
        if (key == 'b' || key == 'B') keyPressed['b'] = false;

        if (key == 'n' || key == 'N') keyPressed['n'] = false;
        if (key == 'f' || key == 'F') keyPressed['f'] = false;
    }

}