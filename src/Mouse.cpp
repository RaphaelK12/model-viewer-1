#include <stdio.h>

#include "GL/freeglut.h"

#include "Constants.hpp"
#include "Mouse.hpp"
#include "Camera.hpp"

namespace Mouse
{
	/********************************************************************************
	 *                               MOUSE VARIABLES                                *
	 ********************************************************************************/

	// stores current up/down status of mouse buttons (for camera tilt)
	bool left_press = false;
	bool right_press = false;

	/********************************************************************************
  	 *                               MOUSE FUNCTIONS                                *
	 ********************************************************************************/

	/* Handles all mouse click/scroll events; left/right clicks tilt the camera
	* (rotation around the n axis); scrolls zoom the camera (translation along the
	* n axis).
	*/
	void mouseButton(int button, int state, int x, int y)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			// tilt CCW
			if (state == GLUT_UP) left_press = false;
			else left_press = true;
		}
		if (button == GLUT_RIGHT_BUTTON)
		{
			// til CW
			if (state == GLUT_UP) right_press = false;
			else right_press = true;
		}

		if (state == GLUT_UP) return; // disregard GLUT_UP events for scrolls

		// translate camera along n axis; handled in same way as u/v axis translation
		if (button == 3)
		{
			// zoom in
			Camera::translateCamera('n', false);
		}
		else if (button == 4)
		{
			// zoom out
			Camera::translateCamera('n', true);
		}
	}

	/* Called whenver the mouse is moved; calculates the cursor's relative position
	* to the center of the window, then calls camera rotation functions accordingly.
	* Resets the cursor to the center of the window after each call.
	*/
	void mouseMove(int x, int y)
	{
		int originx = Constants::window_w / 2;
		int originy = Constants::window_h / 2;

		if ((x == originx) && (y = originy)) return;

		int deltax = x - originx; // positive if right drag, negative if left
		int deltay = originy - y; // positive if up drag, negative if down

		if (deltax > 1) // mouse moved right
		{
			Camera::rotateCamera('v', -Constants::rotate_speed);
		}
		else if (deltax < -1) // mouse moved left
		{
			Camera::rotateCamera('v', Constants::rotate_speed);
		}

		if (deltay > 1) // mouse moved up
		{
			Camera::rotateCamera('u', Constants::rotate_speed);
		}
		else if (deltay < -1) // mouse moved down
		{
			Camera::rotateCamera('u', -Constants::rotate_speed);
		}

		glutWarpPointer(Constants::window_w / 2, Constants::window_h / 2);
	}
}