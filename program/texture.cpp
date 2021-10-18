#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture( std::string path ) {
    int nchannels = -1, w, h;
    unsigned char* pixmap = stbi_load( path.c_str(), &w, &h, &nchannels, 0 );
    if ( !pixmap ) {
        std::cerr << "failed to load texture" << std::endl;
    }

    if ( nchannels != 4 ) {
        std::cerr << "texture must be a 32 bit image" << std::endl;
        stbi_image_free( pixmap );
    }

    count = w / h;
    size = w / count;
    if ( w != h * int(count) ) {
        std::cerr << "the texture file must contain N square textures packed horizontally" << std::endl;
    }

    pixels = std::vector<Color>( w * h );
    for ( int i = 0; i < pixels.size(); i++ ) {
        int root_index = i * 4;
        int r, g, b, a;
        r = pixmap[ root_index ];
        g = pixmap[ root_index + 1 ];
        b = pixmap[ root_index + 2 ];
        a = pixmap[ root_index + 3 ];

        pixels[ i ] = Color( r, g, b, a );
    }

    stbi_image_free( pixmap );
}

size_t Texture::get_size() {
    return size;
}

size_t Texture::get_count() {
    return count;
}

std::vector<Color> Texture::get_column( size_t col_height, int tex_index, int tex_x ) {
    size_t img_w, img_h;
    img_w = size * count;
    img_h = size;
    std::vector<Color> column( col_height );
    for ( int y = 0; y < col_height; y++ ) {
        size_t pixel_x, pixel_y;
        pixel_x = tex_index * size + tex_x;
        pixel_y = (y * size) / col_height;
        column[ y ] = pixels[ pixel_x + pixel_y * img_w ];
    }

    return column;
}

Color Texture::get_pixel( size_t x, size_t y, size_t index ) {
    size_t w = size * count;
    return pixels[ (index * size + x) + y * w ];
}
