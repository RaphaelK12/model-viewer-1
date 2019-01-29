#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {

    /* Window size and positions */
    extern const int window_w;
    extern const int window_h;

    extern const int window1_x;
    extern const int window1_y;

    extern const int window2_x;
    extern const int window2_y;

    /* Speed parameters */
    extern const GLfloat trans_speed;
    extern const GLfloat rotate_speed;
    extern const GLfloat color_speed;
    extern const GLfloat clip_speed;
    extern const double framerate;

    /* Material properties */
    extern const GLfloat mat_am[];
    extern const GLfloat mat_di[];
    extern const GLfloat mat_sp[];
    extern const GLfloat mat_sh[];

    extern const bool DEBUG_MATRICES;
    extern const bool RENDER_AXES;
    extern const bool RENDER_NORMALS;

}

#endif