#include "GL/freeglut.h"

namespace Constants
{
	// window size and position
	extern const int window_w(500);
	extern const int window_h(500);

	extern const int window1_x(250);
	extern const int window1_y(175);

	extern const int window2_x(window_w + window1_x + 50);
	extern const int window2_y(175);

	// various speed parameters
	extern const GLfloat trans_speed(0.05);		// camera translation
	extern const GLfloat rotate_speed(0.005);	// camera rotation
	extern const GLfloat color_speed(0.01);		// color change
	extern const GLfloat clip_speed(0.025);		// clipping value change
	extern const double framerate(60.0);

	// material properties
	extern const GLfloat mat_am[] = { 0.3, 0.3, 0.3, 1.0 };
	extern const GLfloat mat_di[] = { 0.8, 0.8, 0.8, 1.0 };
	extern const GLfloat mat_sp[] = { 0.3, 0.3, 0.3, 1.0 };
	extern const GLfloat mat_sh[] = { 50.0 };

	extern const bool DEBUG_MATRICES(false); // reduce framerate if used
	extern const bool RENDER_AXES(false);
}