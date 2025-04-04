#include "quadtree.hpp"
#include "gif.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>

QuadTree::QuadTree(const Image& input_image) {
    root = new QuadTreeNode(0, 0, input_image[0].size(), input_image.size());
    source_image_matrix = std::move(input_image);
    int height = source_image_matrix.size();
    int width = source_image_matrix[0].size();
    result_image_matrix = Image(height, std::vector<Color>(width, Color{0, 0, 0, 255}));
}

QuadTree::~QuadTree() {
    delete root;
}

void QuadTree::build_quad_tree(int error_choice, double threshold, int min_block_size) {
    if (root) {
        root->divide_node(source_image_matrix, error_choice, threshold, min_block_size);
    }
}

void QuadTree::build_image() {
    if (root) {
        root->draw_node(result_image_matrix, gif_mode);
    }
}

void QuadTree::build_image_gradual() {
    gifFrames.clear();
    int height = result_image_matrix.size();
    int width  = result_image_matrix[0].size();
    int maxDepth = 20; // or compute the max depth dynamically
    Image lastFrame;
    for (int d = 0; d <= maxDepth; d++) {
        Image temp(height, std::vector<Color>(width, Color{0, 0, 0, 255}));
        if (root) {
            root->draw_node_at_depth(temp, 0, d);
        }
        gifFrames.push_back(temp);
        lastFrame = temp; // store the most recent frame
    }
    result_image_matrix = lastFrame; // update final image
}

const Image& QuadTree::get_result_image() const {
    return result_image_matrix;
}

void QuadTree::save_gif(const std::string& gifOutputPath, int delay) {
    if (gifFrames.empty()) {
        std::cerr << "No frames recorded for GIF. Make sure gradual build is performed.\n";
        return;
    }
    int height = gifFrames[0].size();
    int width = gifFrames[0][0].size();
    GifWriter writer = {};
    if (!GifBegin(&writer, gifOutputPath.c_str(), width, height, delay)) {
        std::cerr << "Failed to initialize GIF writer.\n";
        return;
    }
    for (const auto &frame : gifFrames) {
        std::vector<unsigned char> buffer = convert_image_to_RGBA(frame);
        GifWriteFrame(&writer, buffer.data(), width, height, delay);
    }
    GifEnd(&writer);
    std::cout << "Saved compression process GIF to: " << gifOutputPath << "\n";
}

void QuadTree::enable_gif_mode() {
    gif_mode = true;
}

int QuadTree::get_leaf_count() const {
    return (root) ? root->count_leaves() : 0;
}

void QuadTree::save_image_rgba_with_format(const std::string& filename, const std::string& srcFormat) {
    if (result_image_matrix.empty() || result_image_matrix[0].empty()) {
        std::cerr << "Empty image, cannot save.\n";
        return;
    }
    int height = result_image_matrix.size(), width = result_image_matrix[0].size();
    std::vector<unsigned char> buffer(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            const Color& c = result_image_matrix[y][x];
            buffer[idx + 0] = static_cast<unsigned char>(c.r);
            buffer[idx + 1] = static_cast<unsigned char>(c.g);
            buffer[idx + 2] = static_cast<unsigned char>(c.b);
            buffer[idx + 3] = static_cast<unsigned char>(c.a);
        }
    }
    if (srcFormat == "png") {
        if (!stbi_write_png(filename.c_str(), width, height, 4, buffer.data(), width * 4))
            std::cerr << "Failed to save PNG image: " << filename << "\n";
    } else if (srcFormat == "jpg" || srcFormat == "jpeg") {
        if (!stbi_write_jpg(filename.c_str(), width, height, 4, buffer.data(), 100))
            std::cerr << "Failed to save JPEG image: " << filename << "\n";
    } else if (srcFormat == "bmp") {
        if (!stbi_write_bmp(filename.c_str(), width, height, 4, buffer.data()))
            std::cerr << "Failed to save BMP image: " << filename << "\n";
    } else if (srcFormat == "tga") {
        if (!stbi_write_tga(filename.c_str(), width, height, 4, buffer.data()))
            std::cerr << "Failed to save TGA image: " << filename << "\n";
    } else {
        std::cerr << "Unknown source format \"" << srcFormat << "\"; defaulting to PNG.\n";
        if (!stbi_write_png(filename.c_str(), width, height, 4, buffer.data(), width * 4))
            std::cerr << "Failed to save PNG image: " << filename << "\n";
    }
    std::cout << "Saved compressed image to: " << filename << "\n";
}
