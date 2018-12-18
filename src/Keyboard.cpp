#include <stdlib.h>

#include "Display.hpp"
#include "ObjectLoader.hpp"
#include "Camera.hpp"

namespace Keyboard
{
	/********************************************************************************
	 *                              KEYBOARD VARIABLES                              *
	 ********************************************************************************/

	// stores current status of keys; used to handle simultaneous key presses
	bool keyPressed[256] = {false};

	// toggle for increasing/decreasing color and clipping values
	bool increase = true;

	/********************************************************************************
	 *                              KEYBOARD FUNCTIONS                              *
	 ********************************************************************************/

	/* Registered as glutKeyboardFunc; when relevant keys are pressed, sets
	* their entry in keyPressed[] to true. Changing display parameters based
	* on the current entry is handled in timer(); this allows multiple key
	* actions to be processed simultaneously. Other parameters are handled
	* directly in this function, that do not require simultaneous change.
	*/
	void keyPress(unsigned char key, int x, int y)
	{
		// escape
		if (key == 27) exit(0);

		// camera translation
		if (key == 'w' || key == 'W') keyPressed['w'] = true;
		if (key == 's' || key == 'S') keyPressed['s'] = true;
		if (key == 'a' || key == 'A') keyPressed['a'] = true;
		if (key == 'd' || key == 'D') keyPressed['d'] = true;

		//// toggle for color/clipping commands
		if (key == 't') increase = !increase;

		// color controls
		if (key == 'r' || key == 'R') keyPressed['r'] = true;
		if (key == 'g' || key == 'G') keyPressed['g'] = true;
		if (key == 'b' || key == 'B') keyPressed['b'] = true;

		// toggle lighting modes (3 total)
		if (key == 'l' || key == 'L')
			Display::light_on == 2 ? Display::light_on = 0 : Display::light_on++;

		// toggle smooth/flat shading
		if (key == 'p') Display::smooth_shading = !Display::smooth_shading;

		// clipping controls
		if (key == 'n' || key == 'N') keyPressed['n'] = true;
		if (key == 'f' || key == 'F') keyPressed['f'] = true;

		// set polygon mode flags for use in display()
		if (key == '1') Display::render_mode = SOLID;
		else if (key == '2') Display::render_mode = WIREFRAME;
		else if (key == '3') Display::render_mode = POINTS;

		if (key == '9') ObjectLoader::changeModel("models\\bunny.obj");
		else if (key == '0') ObjectLoader::changeModel("models\\cactus.obj");

		if (key == ' ')	Camera::resetCamera();
	}

	/* Registered as glutKeyboardUpFunc; when relevant keys are released, sets
	* their entry in keyPressed[] to false.
	*/
	void keyRelease(unsigned char key, int x, int y)
	{
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