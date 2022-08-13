#pragma once
#ifndef _OBJECT_H_
#define _OBJECT_H_


#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"



class Object {
public: 
	std::vector<glm::ivec3> triangles;
	std::vector<glm::vec<3, double>> points;
	std::vector<glm::vec<3, double>> normals;
	std::vector<glm::vec<2, double>> textCoords;

	Object(std::string fPath);
};

#endif