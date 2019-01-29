#include "GL/freeglut.h"

namespace Constants {

    /* Window size and positions */
    extern const int window_w = 500;
    extern const int window_h = 500;

    extern const int window1_x = 250;
    extern const int window1_y = 175;

    extern const int window2_x = window_w + window1_x + 50;
    extern const int window2_y = 175;

    /* Speed parameters */
    extern const GLfloat trans_speed = 0.05f;		// camera translation
    extern const GLfloat rotate_speed = 0.005f;	    // camera rotation
    extern const GLfloat color_speed = 0.01f;		// color change rate
    extern const GLfloat clip_speed = 0.025f;		// clipping value change rate
    extern const double framerate = 60.0;

    /* Material properties */
    extern const GLfloat mat_am[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // ambient
    extern const GLfloat mat_di[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // diffuse
    extern const GLfloat mat_sp[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // specular
    extern const GLfloat mat_sh[] = { 50.0f }; // shininess

    /* Print transfomation matrices to stdout (reduce framerate if used) */
    extern const bool DEBUG_MATRICES = false;

    /* Render world/camera axes */
    extern const bool RENDER_AXES = false;

    /* Render vertex normals for all vertices */
    extern const bool RENDER_NORMALS = false;

}