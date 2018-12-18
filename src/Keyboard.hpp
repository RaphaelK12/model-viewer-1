#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

namespace Keyboard
{
	/********************************************************************************
	 *                              KEYBOARD VARIABLES                              *
	 ********************************************************************************/

	// stores current status of keys; used to handle simultaneous key presses
	extern bool keyPressed[256];

	// toggle for increasing/decreasing color and clipping values
	extern bool increase;

	/********************************************************************************
	 *                              KEYBOARD FUNCTIONS                              *
	 ********************************************************************************/

	void keyPress(unsigned char key, int x, int y);

	void keyRelease(unsigned char key, int x, int y);
}

#endif
