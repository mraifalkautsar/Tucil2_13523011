#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>
#include <cctype>

#include "stb_image.h"
#include "stb_image_write.h"

struct Color {
    int r, g, b;
    int a; // alpha channel
};

typedef std::vector<std::vector<Color>> Image;

/* Menghitung error untuk variansi, MAD, max pixel difference, dan entropi */
double compute_block_error(const Image& image, int startX, int startY, int blockWidth, int blockHeight, int errorChoice);

/* Menghitung rerata warna dari suatu blok*/
Color compute_average_color(const Image& image, int startX, int startY, int blockWidth, int blockHeight);

/* Konversi data gambar menjadi array RGBA */
std::vector<unsigned char> convert_image_to_RGBA(const Image& image);

/* Fungsi pembantu untuk load image */
Image load_image_rgba(const std::string& filename);

/* Fungsi pembantu untuk memperoleh ekstensi */
std::string get_extension(const std::string& filename);

#endif
