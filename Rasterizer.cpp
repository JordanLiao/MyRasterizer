#include "Rasterizer.h"

/*  line:
	The idea is to draw along the slope(dy/dx) of the line. For every pixel drawn along the horizontal,
	you want to draw dy/dx amount of a pixel on the vertical, but since you can't just draw a part of a
	pixel, you have to approximate the line. To do this, you keep a accumulated vertical change so far,
	and you only advance to the next vertical level, if the accumulated amount of vertical change 
	exceeds 1 pixel value; if it does not exceeds, then just draw another pixel on the same level 
	and increment the accumulator.
*/

void MyRasterizer::line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {	
	// if the absolute value of the slope is greater than 1, transpose the line coordinates
	bool steep = false;
	if (abs(y1 - y0) > abs(x1 - x0)) {
		steep = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	// if x0 > x1, swap the points to ensure left to right order
	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0; // length of the horizontal component of the line
	int absDy = abs(y1 - y0); // length of the vertical component of the line
	int yf = (y1 > y0) ? 1 : -1;  // vertical component change factor. if negative slope, set to -1.
	int acc = 0; //accumulator to keep track of whether the vertical component should change.
	for (int x = x0, y = y0; x <= x1; x++) {
		if (!steep) // if the point coordinates have been transposed, just transpose back when setting color
			image.set(x, y, color);
		else
			image.set(y, x, color);

		// if the incremented acc is greater than a horizontal component, then next, we need to draw on a new y level
		acc += absDy;
		if (acc >= dx) {
			y += yf;
			acc -= dx;
		}
	}
}

void MyRasterizer::triangle(glm::vec<3, double> pf0, glm::vec<3,double> pf1, glm::vec<3, double> pf2, 
							glm::vec<2, double> t0, glm::vec<2, double> t1, glm::vec<2, double> t2,
	                        TGAImage& texture, TGAImage& outputFrame, std::vector<double>& zBuffer) {
	int width = outputFrame.width(), height = outputFrame.height();
	int t_width = texture.width(), t_height = texture.height();
	// getting the integer point coordinates for figuring out the triangle's bounding box. NOTE: could also add screen clipping
	glm::ivec2 p0((pf0.x + 1.0) * width / 2.0,
		(pf0.y + 1.0) * height / 2.0);
	glm::ivec2 p1((pf1.x + 1.0) * width / 2.0,
		(pf1.y + 1.0) * height / 2.f);
	glm::ivec2 p2((pf2.x + 1.0) * width / 2.0,
		(pf2.y + 1.0) * height / 2.0);
	int left = std::min(std::min(p0.x, p1.x), p2.x);
	int right = std::max(std::max(p0.x, p1.x), p2.x);
	int buttom = std::min(std::min(p0.y, p1.y), p2.y);
	int top = std::max(std::max(p0.y, p1.y), p2.y);

	glm::vec<3, double> v01((pf1.x - pf0.x) * width / 2.0, (pf1.y - pf0.y) * height / 2.0, 0); // vec AB
	glm::vec<3, double> v02((pf2.x - pf0.x) * width / 2.0, (pf2.y - pf0.y) * height / 2.0, 0); // vec AC
	for (int i = left; i <= right; i++) {
		for (int j = buttom; j <= top; j++) {
			glm::vec<2, double> v((pf0.x + 1.0) * width / 2.0 - (double)i, (pf0.y + 1.0) * height / 2.0 - (double)j); // vec PA
			glm::vec<3, double> v1(v02.x, v01.x, v.x); // <v, u, 1>
			glm::vec<3, double> v2(v02.y, v01.y, v.y);
			glm::vec<3, double> cp = glm::cross(v1, v2);
			//Barycentric Coordinates of point ij with respect to this triangle's vertices
			double b_a = cp.x / cp.z;     // v
			double b_b = cp.y / cp.z;     // u
			double b_c = 1.0 - b_a - b_b; // 1-u-v

			if (b_a >= 0.0 && b_b >= 0.0 && b_c >= 0.0 ) { // if either value is negative, then the point is outside the triangle
				double zVal = pf0.z * b_c + pf1.z * b_b + pf2.z * b_a; // interpolated depth z-value for depth testing
				if (zVal > zBuffer[i * height + j]) { // if this fragment is in front of the existing fragment, draw it.
					glm::vec<2, double> t_f = t0 * b_c + t1 * b_b + t2 * b_a; // interpolated textCoord in range 0-1
					TGAColor color = texture.get((int)(t_f.x * t_width), (int)(t_f.y * t_height));
					outputFrame.set(i, j, color);
					zBuffer[i * height + j] = zVal;
				}
			}
		}
	}
}

void MyRasterizer::draw(Object* obj, TGAImage& texture, TGAImage& outputFrame) {
	glm::vec<3, double> lightDir(0.0, 0.0, -1.0);  // this is actually set to the opposite direction of the light
	int width = outputFrame.width(), height = outputFrame.height();
	int triangleNum = obj->triangles.size();
	std::vector<double> zBuffer(width * height, -1.f); // pre-allocate 

	for (auto i = 0; i < triangleNum; i++) {
		// compute each triangle's face norm
		glm::vec<3, double> norm = glm::normalize(glm::cross(
						obj->points[obj->triangles[i].z] - obj->points[obj->triangles[i].x],
						obj->points[obj->triangles[i].y] - obj->points[obj->triangles[i].x]));
		// intensity tells how much is a triangle facing a light ray
		double intensity = glm::dot(norm, lightDir);
		if (intensity > 0.0) { // if the intensity is negative, then this face is one the back side, so do not draw it.
			triangle(obj->points[obj->triangles[i].x], obj->points[obj->triangles[i].y], obj->points[obj->triangles[i].z],
				obj->textCoords[obj->triangles[i].x], obj->textCoords[obj->triangles[i].y], obj->textCoords[obj->triangles[i].z],
				texture, outputFrame, zBuffer);
		}
	}
}

