#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

namespace Camera {

	extern const bool DEBUG;

	/********************************************************************************
	 *                               CAMERA VARIABLES                               *
	 ********************************************************************************/

	/* Camera attributes for gluLookAt() */
	extern glm::vec3 camera;
	extern glm::vec3 target;
	extern glm::vec3 up;

    extern glm::vec3 n_axis;
    extern glm::vec3 u_axis;
    extern glm::vec3 v_axis;

	extern GLfloat near_clip, far_clip;


	/********************************************************************************
	 *                               CAMERA FUNCTIONS                               *
	 ********************************************************************************/

	void resetCamera();

    void updateCameraAxes();

	void translateCamera(char axis, bool pos);

	void rotateCamera(char axis, float angle);

	void calcModelViewMat();

	void calcProjectionMat();

	void increaseNearClip();

	void decreaseFarClip();

    void printModelViewMatrix();

    void printProjectionMatrix();

}

#endif