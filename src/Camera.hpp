#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

namespace Camera 
{
	extern const bool DEBUG;

	/********************************************************************************
	 *                               CAMERA VARIABLES                               *
	 ********************************************************************************/

	// camera attributes (for gluLookAt)
	extern glm::vec3 camera;
	extern glm::vec3 target;
	extern glm::vec3 up;

	extern GLdouble near_clip, far_clip;

	/********************************************************************************
	 *                               CAMERA FUNCTIONS                               *
	 ********************************************************************************/

	void resetCamera();

	void translateCamera(char axis, bool pos);

	void rotateCamera(char axis, float angle);

	void printModelViewMatrix();

	void printProjectionMatrix();

	void calcModelViewMat();

	void calcProjectionMat();

	void increaseNearClip();

	void decreaseFarClip();
}

#endif