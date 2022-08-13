#include <iostream>
#include "tgaimage.h"
#include "Rasterizer.h"
#include "Object.h"


int main() {
    TGAColor white = TGAColor(255, 255, 255, 255);
    TGAColor red = TGAColor(255, 0, 0, 255);

    int width = 800;
    int height = 800;
    TGAImage image(width, height,TGAImage::RGB);
    //image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    Object obj("Assets/african_head.obj");
    TGAImage tex;
    tex.read_tga_file("Assets/african_head_diffuse.tga");
    tex.flip_vertically();
    //tex.flip_horizontally();

    //draw wire frame
    /*for (auto i = 0; i < obj.triangles.size(); i++) {
        glm::ivec3 tri = obj.triangles[i];
        for (int j = 0; j < 3; j++) {
            glm::vec3 v0 = obj.points[tri[j]];
            glm::vec3 v1 = obj.points[tri[(j + 1) % 3]];
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            MyRasterizer::line(x0, y0, x1, y1, image, white);
        }
    }*/
    //MyRasterizer::triangle(glm::ivec2(0 ,10), glm::ivec2(50, 100), glm::ivec2(150, 150), image, white);
    
    MyRasterizer::draw(&obj, tex, image);

    image.write_tga_file("output.tga");
    return 0;
}