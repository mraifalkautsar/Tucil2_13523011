#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>
#include <cctype>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


struct Color {
    int r, g, b;
    int a; // alpha channel
};

typedef std::vector<std::vector<Color>> Image;

// Convert a Color to grayscale
int toGray(const Color& c);

// Compute block error using different methods.
// errorChoice: 1=Variance, 2=MAD, 3=MaxDiff, 4=Entropy, 5=SSIM (simplified)
double compute_block_error(const Image& image, int startX, int startY, int blockWidth, int blockHeight, int errorChoice);

// Compute the average color of a block.
Color compute_average_color(const Image& image, int startX, int startY, int blockWidth, int blockHeight);

// Convert an Image (2D vector) to a flat RGBA buffer.
std::vector<unsigned char> convert_image_to_RGBA(const Image& image);

Image load_image_rgba(const std::string& filename);

std::string get_extension(const std::string& filename);

#endif
