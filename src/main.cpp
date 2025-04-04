#include <iostream>
#include "quadtree.hpp"
#include "utils.hpp"

int main() {
    std::string inputFile, outputFile, gifOutput;
    int errorChoice;
    double threshold;
    int minBlockSize;
    double targetCompression; 

    std::cout << "Enter absolute source image path: ";
    std::getline(std::cin, inputFile);
    std::cout << "Enter absolute output image path: ";
    std::getline(std::cin, outputFile);
    std::cout << "Enter error choice (1=Variance, 2=MAD, 3=MaxDiff, 4=Entropy, 5=SSIM): ";
    std::cin >> errorChoice;
    std::cout << "Enter threshold: ";
    std::cin >> threshold;
    std::cout << "Enter minimum block size: ";
    std::cin >> minBlockSize;
    std::cout << "Enter target compression (floating point, 1.0 = no compression, 0 to disable): ";
    std::cin >> targetCompression;
    std::cin.ignore();
    std::cout << "Enter absolute GIF output path: ";
    std::getline(std::cin, gifOutput);

    Image input_image = load_image_rgba(inputFile);
    if (input_image.empty()) {
        std::cerr << "Error: Could not load input image from " << inputFile << "\n";
        return -1;
    }
    std::string srcFormat = get_extension(inputFile);
    if (srcFormat.empty()) {
        std::cerr << "Warning: Could not determine source image format. Defaulting to PNG.\n";
        srcFormat = "png";
    }

    // Create QuadTree.
    QuadTree qt(input_image);

    // If targetCompression is enabled (> 0), iteratively adjust threshold.
    if (targetCompression > 0) {
        const double tolerance = 2.0; // tolerance in percentage
        int originalPixels = input_image.size() * input_image[0].size();
        double currentCompression = 0.0;
        double lowThreshold = 0.0, highThreshold = threshold;
        qt.build_quad_tree(errorChoice, threshold, minBlockSize);
        int leafCount = qt.get_leaf_count();
        currentCompression = (1.0 - static_cast<double>(leafCount) / originalPixels) * 100.0;
        int iter = 0;
        while (std::abs(currentCompression - targetCompression * 100.0) > tolerance && iter < 20) {
            if (currentCompression < targetCompression * 100.0) {
                highThreshold = threshold;
                threshold = (lowThreshold + threshold) / 2.0;
            } else {
                lowThreshold = threshold;
                threshold = (threshold + highThreshold) / 2.0;
            }
            qt = QuadTree(input_image);
            qt.build_quad_tree(errorChoice, threshold, minBlockSize);
            leafCount = qt.get_leaf_count();
            currentCompression = (1.0 - static_cast<double>(leafCount) / originalPixels) * 100.0;
            iter++;
            std::cout << "Iteration " << iter << ": threshold = " << threshold 
                      << ", compression = " << currentCompression << "%\n";
        }
    } else {
        qt.build_quad_tree(errorChoice, threshold, minBlockSize);
    }

    qt.enable_gif_mode();
    qt.build_image_gradual();
    qt.save_image_rgba_with_format(outputFile, srcFormat);
    qt.save_gif(gifOutput, 50);

    return 0;
}
