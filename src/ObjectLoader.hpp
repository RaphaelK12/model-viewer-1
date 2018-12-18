#pragma once

#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <list>

namespace ObjectLoader
{
	extern const bool debug;

	extern std::vector<GLfloat> faceNormals;
	extern std::vector<GLfloat> faceAreas;

	extern std::vector< std::list< GLuint > > memberFaces;

	/********************************************************************************
	 *                            OBJECT LOADER FUNCTIONS                           *
	 ********************************************************************************/

	bool loadObject(char *filepath);

	void changeModel(char *filepath);

	void processFaces();

	void printMemberFaces();

	void calculateVertexNormals();
}

#endif