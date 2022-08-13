#pragma once

#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_


#include "glm/glm.hpp"
#include "tgaimage.h"
#include "Object.h"

#include <vector>

namespace MyRasterizer {
	void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
	void triangle(glm::vec<3, double> pf0, glm::vec<3, double> pf1, glm::vec<3, double> pf2, 
		glm::vec<2, double> t0, glm::vec<2, double> t1, glm::vec<2, double> t2,
		TGAImage& texture, TGAImage& outputFrame, std::vector<double>& zBuffer);
	void draw(Object* obj, TGAImage& texture, TGAImage& outputFrame);
}

#endif