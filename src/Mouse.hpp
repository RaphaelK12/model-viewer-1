#pragma once

#ifndef MOUSE_H
#define MOUSE_H

namespace Mouse
{
	/********************************************************************************
	 *                               MOUSE VARIABLES                                *
	 ********************************************************************************/

	extern bool left_press;
	extern bool right_press;

	/********************************************************************************
	 *                               MOUSE FUNCTIONS                                *
	 ********************************************************************************/

	void mouseButton(int button, int state, int x, int y);

	void mouseMove(int x, int y);
}

#endif