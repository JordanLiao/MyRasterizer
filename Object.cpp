#include "Object.h"

Object::Object(std::string fPath){
	std::string delim = "/";

	std::vector<glm::vec<3, double>> normalsp, pointsp; // temp buffer
	std::vector<glm::vec<2, double>> textsp;
	std::ifstream objFile(fPath); // The obj file we are reading.
	// Check whether the file can be opened.
	if (objFile.is_open()) {
		std::string line; // A line in the file.
		glm::vec<3, double> point, norm;
		glm::vec<2, double> texCoord;
		glm::ivec3 triangle;
		int vCount = 0, aPos, bPos, cPos, a1Pos, b1Pos, c1Pos;
		while (std::getline(objFile, line)) {
			std::stringstream ss;// Turn the line into a string stream for processing.
			ss << line;
			std::string label;
			ss >> label; // Read the first word of the line.

			if (label == "v") {
				ss >> point.x >> point.y >> point.z;
				pointsp.push_back(point);
			}
			else if (label == "vn") {
				ss >> norm.x >> norm.y >> norm.z;
				norm = glm::normalize(norm);
				normalsp.push_back(norm);
			} 
			else if (label == "vt") {
				ss >> texCoord.x >> texCoord.y;
				textsp.push_back(texCoord);
			} 
			else if (label == "f") {
				std::string a, b, c;
				ss >> a >> b >> c;

				//find the locations of the two delimiters
				aPos = a.find(delim);
				bPos = b.find(delim);
				cPos = c.find(delim);
				a1Pos = a.rfind(delim);
				b1Pos = b.rfind(delim);
				c1Pos = c.rfind(delim);

				//get vertex points
				points.push_back(pointsp[std::stoi(a.substr(0, aPos)) - 1]);
				points.push_back(pointsp[std::stoi(b.substr(0, bPos)) - 1]);
				points.push_back(pointsp[std::stoi(c.substr(0, cPos)) - 1]);

				//get texture coords; use conditions because texture value in f section might not exist like 1//3.
				if (aPos + 1 < a1Pos)
					textCoords.push_back(textsp[stoi(a.substr(aPos + 1, a1Pos - aPos - 1)) - 1]);
				if (bPos + 1 < b1Pos)
					textCoords.push_back(textsp[stoi(b.substr(bPos + 1, b1Pos - bPos - 1)) - 1]);
				if (cPos + 1 < c1Pos)
					textCoords.push_back(textsp[stoi(c.substr(cPos + 1, c1Pos - cPos - 1)) - 1]);

				normals.push_back(normalsp[stoi(a.substr(a1Pos + 1, std::string::npos)) - 1]);
				normals.push_back(normalsp[stoi(b.substr(b1Pos + 1, std::string::npos)) - 1]);
				normals.push_back(normalsp[stoi(c.substr(c1Pos + 1, std::string::npos)) - 1]);

				triangle.x = vCount++;
				triangle.y = vCount++;
				triangle.z = vCount++;
				triangles.push_back(triangle);
			}
		}
	}
	else
		std::cout << "Can't open the file " << fPath << std::endl;

	objFile.close();
}