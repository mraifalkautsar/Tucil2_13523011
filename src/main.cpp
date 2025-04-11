#include <iostream>
#include "quadtree.hpp"
#include "utils.hpp"
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
    std::string inputFile, outputFile, gifOutput;
    bool to_gif = true;
    int errorChoice;
    double threshold;
    int minBlockSize;
    double targetCompression; 

    std::cout << "[1/7] Enter absolute input image path: ";
    std::getline(std::cin, inputFile);
    std::cout << "[2/7] Enter absolute output image path: ";
    std::getline(std::cin, outputFile);
    std::cout << "Available Error Choices" << std::endl;
    std::cout << "1. Variance" << std::endl;
    std::cout << "2. Mean Absolute Deviance" << std::endl;
    std::cout << "3. Max Pixel Difference" << std::endl;
    std::cout << "4. Entropy" << std::endl;
    std::cout << "5. Structural Similarity Index (SSIM)" << std::endl;
    std::cout << "[3/7] Enter error choice (1: Variance, 2: MAD, 3: MaxDiff, 4: Entropy, 5: SSIM): ";
    std::cin >> errorChoice;
    std::cout << "[4/7] Enter target compression (floating point, 1.0 = no compression, 0 to disable): ";
    std::cin >> targetCompression;
    if (targetCompression == 0) {
        std::cout << "[5/7] Enter threshold: ";
        std::cin >> threshold;
        std::cout << "[6/7] Enter minimum block size: ";
        std::cin >> minBlockSize;
    }
    std::cin.ignore();
    std::cout << "[7/7] Enter absolute GIF output path (blank to not save GIF): ";
    std::getline(std::cin, gifOutput);
    if (gifOutput.empty()) {
        to_gif = false;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    Image input_image = load_image_rgba(inputFile);

    bool smallfile = false;
    double originalFileSizeKB = std::filesystem::exists(inputFile) ? std::filesystem::file_size(inputFile) / 1024.0 : 0;

    if (originalFileSizeKB < 100) {
        smallfile = true;
    }

    if (input_image.empty()) {
        std::cerr << "Error: Could not load input image from " << inputFile << "\n";
        return -1;
    }
    std::string srcFormat = get_extension(inputFile);
    if (srcFormat.empty()) {
        std::cerr << "Warning: Could not determine source image format. Defaulting to PNG.\n";
        srcFormat = "png";
    }

    // Membuat sebuah pointer QuadTree dan mengisinya 
    QuadTree* qt = new QuadTree(input_image);

    // Jika mode target compression menyala, sesuaikan threshold secara iteratif
    if (targetCompression > 0) {
        qt->adjust_threshold_for_target(errorChoice, targetCompression);
    } else {
        qt->build_quad_tree(errorChoice, threshold, minBlockSize);
    }

    // Build the final image.
    if (to_gif) {
        qt->build_image_gradual();
        qt->save_gif(gifOutput, 50);
    } else {
        qt->build_image_fast();
    }

    qt->save_image_rgba_with_format(outputFile, srcFormat, smallfile);


    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    int origHeight = input_image.size();
    int origWidth = input_image[0].size();
    double compressedFileSizeKB = std::filesystem::exists(outputFile) ? std::filesystem::file_size(outputFile) / 1024.0 : 0;
    int originalPixels = origHeight * origWidth;
    int leafCount = qt->get_leaf_count();
    double compressionPercentage = (1.0 - compressedFileSizeKB / originalFileSizeKB) * 100.0;
    int treeDepth = qt->get_tree_depth();
    int totalNodes = qt->get_total_node_count();
    
    std::cout << "[Post-Compression Information]" << std::endl;
    std::cout << "Execution Time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Original File Size: " 
              << originalFileSizeKB << " KB" << std::endl;
    std::cout << "Compressed File Size: " 
              << compressedFileSizeKB << " KB" << std::endl;
    std::cout << "Compression Percentage: " << compressionPercentage << "%" << std::endl;
    std::cout << "Tree Depth: " << treeDepth << std::endl;
    std::cout << "Total Number of Tree Nodes: " << totalNodes << std::endl;

    delete qt; // Bersih-bersih.
    return 0;
}
