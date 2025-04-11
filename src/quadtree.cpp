#include "quadtree.hpp"
#include "gif.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>

/* Constructor QuadTree dengan referensi ke input image */
QuadTree::QuadTree(const Image& input_image) {
    root = new QuadTreeNode(0, 0, input_image[0].size(), input_image.size());
    source_image_matrix = std::move(input_image);
    int height = source_image_matrix.size();
    int width = source_image_matrix[0].size();
    result_image_matrix = Image(height, std::vector<Color>(width, Color{0, 0, 0, 255}));
}

/* Destructor QuadTree */
QuadTree::~QuadTree() {
    delete root;
}

/* Membangun QuadTree berdasarkan parameter dan gambar yang ada */
void QuadTree::build_quad_tree(int error_choice, double threshold, int min_block_size) {
    if (root) {
        root->divide_node(source_image_matrix, error_choice, threshold, min_block_size);
    }
}

/* Mode cepat pembuatan gambar dari QuadTree (untuk opsi tanpa GIF) */
void QuadTree::build_image_fast() {
    if (root) {
        root->draw_node_fast(result_image_matrix);
    }
}

/* Mode gradual pembuatan gambar dari QuadTree (untuk opsi dengan GIF) */
void QuadTree::build_image_gradual() {
    gifFrames.clear();
    int height = result_image_matrix.size();
    int width  = result_image_matrix[0].size();

    int actualMaxDepth = (root) ? root->max_depth() : 0; // Mendapatkan max depth
    
    Image lastFrame;
    // Loop dari 0 ke kedalaman maksimum.
    for (int d = 0; d <= actualMaxDepth; d++) {
        Image temp(height, std::vector<Color>(width, Color{0, 0, 0, 255}));
        if (root) {
            root->draw_node_at_depth(temp, 0, d);
        }
        gifFrames.push_back(temp);
        lastFrame = temp; // Penyetoran frame paling recent
    }
    result_image_matrix = lastFrame; // Update gambar terakhir
}

/* Menyimpan gambar RGBA menjadi file */
void QuadTree::save_image_rgba_with_format(const std::string& filename, const std::string& srcFormat, bool smallfile) {
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
        int quality;
        if (smallfile) {
            quality = 70;          
        } else {
            quality = 90;
        }
        if (!stbi_write_jpg(filename.c_str(), width, height, 4, buffer.data(), quality))
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

/* Menyimpan GIF */
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

/* Memperoleh kedalaman pohon */
int QuadTree::get_tree_depth() const {
    return (root) ? root->get_depth() : 0;
}

/* Memperoleh jumlah simpul dalam pohon */
int QuadTree::get_total_node_count() const {
    return (root) ? root->get_node_count() : 0;
}

/* Metode untuk mode target compression */
void QuadTree::adjust_threshold_for_target(int errorChoice, double targetCompression,
                                             double tolerance, int maxIterations) {
    int minBlockSize = 2;
    double threshold, lowThreshold, highThreshold;
    std::cout << "Error Choice: " << errorChoice << std::endl;
    switch (errorChoice) {
        case 1:
            threshold = 8128;
            lowThreshold = 0;
            highThreshold = 16256;
            break;
        case 2:
            threshold = 63;
            lowThreshold = 0;
            highThreshold = 127;
            break;
        case 3:
            threshold = 127;
            lowThreshold = 0;
            highThreshold = 255;
            break;
        case 4:
            threshold = 4;
            lowThreshold = 0;
            highThreshold = 18;
            break;
        case 5:
            threshold = 0.5;
            lowThreshold = 0;
            highThreshold = 1;
            break;
        default:
            threshold = 50;
            lowThreshold = 0;
            highThreshold = 100;
            break;
    }
    int originalPixels = source_image_matrix.size() * source_image_matrix[0].size();
    double currentCompression = 0.0;
    int iter = 0;

    // Membangun pohon awal
    build_quad_tree(errorChoice, threshold, minBlockSize);
    int leafCount = get_leaf_count();
    currentCompression = (1.0 - static_cast<double>(leafCount) / originalPixels) * 100.0;

    while (std::abs(currentCompression - targetCompression * 100.0) > tolerance && iter < maxIterations) {
        if (currentCompression < targetCompression * 100.0) {
            // Kompresi terlalu sedikit sehingga threshold ditingkatkan
            lowThreshold = threshold;
            threshold = (threshold + highThreshold) / 2.0;
        } else {
            // Kompresi terlalu banyak sehingga threshold dikurangi
            highThreshold = threshold;
            threshold = (lowThreshold + threshold) / 2.0;
        }

        // Hapus pohon lama dan bangun kembali
        delete root;
        root = new QuadTreeNode(0, 0, source_image_matrix[0].size(), source_image_matrix.size());
        build_quad_tree(errorChoice, threshold, minBlockSize);
        leafCount = get_leaf_count();
        currentCompression = (1.0 - static_cast<double>(leafCount) / originalPixels) * 100.0;
        iter++;
        std::cout << "Iteration " << iter << ": threshold = " << threshold 
                  << ", compression = " << currentCompression << "%\n";
    }
}

/* Memperoleh jumlah daun */
int QuadTree::get_leaf_count() const {
    return (root) ? root->count_leaves() : 0;
}