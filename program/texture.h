#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <string>
#include <vector>

#include "color.h"

class Texture {
public:
    Texture( std::string path );
    size_t get_size();
    size_t get_count();
    std::vector<Color> get_column( size_t col_height, int tex_index, int tex_x );
    Color get_pixel( size_t x, size_t y, size_t index );

private:
    std::vector<Color> pixels;
    size_t size;
    size_t count;
};

#endif
