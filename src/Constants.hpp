#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants
{
	// window size and position
	extern const int window_w;
	extern const int window_h;

	extern const int window1_x;
	extern const int window1_y;

	extern const int window2_x;
	extern const int window2_y;

	// various speed parameters
	extern const GLfloat trans_speed;	// camera translation
	extern const GLfloat rotate_speed;	// camera rotation
	extern const GLfloat color_speed;	// color change
	extern const GLfloat clip_speed;	// clipping value change
	extern const double framerate;

	// material properties
	extern const GLfloat mat_am[];
	extern const GLfloat mat_di[];
	extern const GLfloat mat_sp[];
	extern const GLfloat mat_sh[];

	extern const bool DEBUG_MATRICES;
	extern const bool RENDER_AXES;
}

#endif