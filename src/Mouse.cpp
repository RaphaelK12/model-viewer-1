#include <stdio.h>

#include "GL/freeglut.h"

#include "Camera.hpp"
#include "Constants.hpp"
#include "Mouse.hpp"

namespace Mouse {

    /* Stores current up/down status of mouse buttons; used to control camera tilt */
    bool left_press = false;
    bool right_press = false;


    /* 
     * Handles all mouse click and scroll events. 
     *
     * Left/right clicks tilt the camera (rotation around the n axis).
     * Scrolls zoom the camera (translation along the n axis).
     */
    void mouseButton(int button, int state, int x, int y) {
        /* Tilts camera CCW */
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_UP) left_press = false;
            else left_press = true;
        }
        
        /* Tilts camera CW */
        if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_UP) right_press = false;
            else right_press = true;
        }

        if (state == GLUT_UP) return; // disregard GLUT_UP events for scrolls

        /* Scrolls translate camera along n axis */
        if (button == 3) {
            // Zoom in
            Camera::translateCamera('n', false);
        } else if (button == 4) {
            // Zoom out
            Camera::translateCamera('n', true);
        }
    }


    /* 
     * Calculates the cursor's relative position to the center of the window, then 
     * calls camera rotation functions accordingly. Resets the cursor to the center 
     * of the window after each call.
     */
    void mouseMove(int x, int y) {
        int originx = Constants::window_w / 2;
        int originy = Constants::window_h / 2;

        if ((x == originx) && (y = originy)) return;

        int deltax = x - originx; // positive if right drag, negative if left
        int deltay = originy - y; // positive if up drag, negative if down

        if (deltax > 1) {
            Camera::rotateCamera('v', -Constants::rotate_speed); // mouse moved right
        } else if (deltax < -1) {
            Camera::rotateCamera('v', Constants::rotate_speed); // mouse moved left
        }

        if (deltay > 1) {
            Camera::rotateCamera('u', Constants::rotate_speed); // mouse moved up
        } else if (deltay < -1) {
            Camera::rotateCamera('u', -Constants::rotate_speed); // mouse moved down
        }

        glutWarpPointer(Constants::window_w / 2, Constants::window_h / 2);
    }

}