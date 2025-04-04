#include "utils.hpp"

int toGray(const Color& c) {
    return (c.r + c.g + c.b) / 3;
}

static double compute_ssim_error_for_block(const Image& image, int startX, int startY, int blockWidth, int blockHeight) {
    int count = blockWidth * blockHeight;
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;
    for (int y = startY; y < startY + blockHeight; ++y) {
        for (int x = startX; x < startX + blockWidth; ++x) {
            const Color &c = image[y][x];
            sumR += c.r;
            sumG += c.g;
            sumB += c.b;
        }
    }
    double meanR = sumR / count;
    double meanG = sumG / count;
    double meanB = sumB / count;
    
    double varR = 0.0, varG = 0.0, varB = 0.0;
    for (int y = startY; y < startY + blockHeight; ++y) {
        for (int x = startX; x < startX + blockWidth; ++x) {
            const Color &c = image[y][x];
            varR += (c.r - meanR) * (c.r - meanR);
            varG += (c.g - meanG) * (c.g - meanG);
            varB += (c.b - meanB) * (c.b - meanB);
        }
    }
    varR /= count;
    varG /= count;
    varB /= count;
    
    // Constants for SSIM for 8-bit images
    const double C1 = (0.01 * 255) * (0.01 * 255); // ≈6.5025 (will cancel out)
    const double C2 = (0.03 * 255) * (0.03 * 255); // ≈58.5225

    // For a uniform block (compressed block), μ_y equals μ_x and variance is 0.
    // Thus, SSIM per channel simplifies to:
    // SSIM_c = (2 μ_x^2 + C1)*(C2) / ((2 μ_x^2 + C1)*(σ_x^2 + C2)) = C2 / (σ_x^2 + C2)
    double ssimR = C2 / (varR + C2);
    double ssimG = C2 / (varG + C2);
    double ssimB = C2 / (varB + C2);
    double ssim = (ssimR + ssimG + ssimB) / 3.0;
    // Error is defined as 1 - SSIM.
    return 1.0 - ssim;
}

double compute_block_error(const Image& image, int startX, int startY, int blockWidth, int blockHeight, int errorChoice) {
    int imageHeight = static_cast<int>(image.size());
    if (imageHeight == 0) return 0.0;
    int imageWidth = static_cast<int>(image[0].size());
    if (startX < 0 || startY < 0 || startX + blockWidth > imageWidth || startY + blockHeight > imageHeight) {
        std::cerr << "Error: Block parameters out of image bounds." << std::endl;
        return 0.0;
    }
    
    const int N = blockWidth * blockHeight;
    if (N == 0) return 0.0;

    if (errorChoice == 5) {
        return compute_ssim_error_for_block(image, startX, startY, blockWidth, blockHeight);
    }
    
    std::vector<int> pixelsR, pixelsG, pixelsB;
    pixelsR.reserve(N);
    pixelsG.reserve(N);
    pixelsB.reserve(N);
    
    for (int yy = startY; yy < startY + blockHeight; ++yy) {
        for (int xx = startX; xx < startX + blockWidth; ++xx) {
            const Color& c = image[yy][xx];
            pixelsR.push_back(c.r);
            pixelsG.push_back(c.g);
            pixelsB.push_back(c.b);
        }
    }
    
    auto computeChannelError = [&](const std::vector<int>& pix) -> double {
        int minVal = 255, maxVal = 0;
        long long sumVal = 0;
        for (int val : pix) {
            if(val < minVal) minVal = val;
            if(val > maxVal) maxVal = val;
            sumVal += val;
        }
        double mean = static_cast<double>(sumVal) / pix.size();
        switch(errorChoice) {
            case 1: { // Variance
                double var = 0.0;
                for (int val : pix) {
                    double diff = val - mean;
                    var += diff * diff;
                }
                return var / pix.size();
            }
            case 2: { // Mean Absolute Deviation (MAD)
                double mad = 0.0;
                for (int val : pix) {
                    mad += std::fabs(val - mean);
                }
                return mad / pix.size();
            }
            case 3: { // Max Pixel Difference
                return static_cast<double>(maxVal - minVal);
            }
            case 4: { // Entropy
                std::map<int,int> freq;
                for (int val : pix) {
                    freq[val]++;
                }
                double entropy = 0.0;
                for (const auto& kv : freq) {
                    double p = static_cast<double>(kv.second) / pix.size();
                    entropy += -p * std::log2(p);
                }
                return entropy;
            }
            default:
                std::cerr << "Unknown errorChoice! Defaulting to variance.\n";
                double var = 0.0;
                for (int val : pix) {
                    double diff = val - mean;
                    var += diff * diff;
                }
                return var / pix.size();
        }
    };
    
    double errorR = computeChannelError(pixelsR);
    double errorG = computeChannelError(pixelsG);
    double errorB = computeChannelError(pixelsB);
    return (errorR + errorG + errorB) / 3.0;
}

Color compute_average_color(const Image& image, int startX, int startY, int blockWidth, int blockHeight) {
    long long sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    int count = blockWidth * blockHeight;
    for (int yy = startY; yy < startY + blockHeight; ++yy) {
        for (int xx = startX; xx < startX + blockWidth; ++xx) {
            const Color &c = image[yy][xx];
            sumR += c.r;
            sumG += c.g;
            sumB += c.b;
            sumA += c.a;
        }
    }
    Color avg;
    avg.r = static_cast<int>(sumR / count);
    avg.g = static_cast<int>(sumG / count);
    avg.b = static_cast<int>(sumB / count);
    avg.a = static_cast<int>(sumA / count);
    return avg;
}

std::vector<unsigned char> convert_image_to_RGBA(const Image& image) {
    int height = image.size();
    int width = (height > 0) ? image[0].size() : 0;
    std::vector<unsigned char> buffer(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            const Color &c = image[y][x];
            buffer[idx + 0] = static_cast<unsigned char>(c.r);
            buffer[idx + 1] = static_cast<unsigned char>(c.g);
            buffer[idx + 2] = static_cast<unsigned char>(c.b);
            buffer[idx + 3] = static_cast<unsigned char>(c.a);
        }
    }
    return buffer;
}

// Helper to get file extension in lowercase.
std::string get_extension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    std::string ext = filename.substr(pos + 1);
    for (auto &ch : ext) {
        ch = std::tolower(ch);
    }
    return ext;
}

Image load_image_rgba(const std::string& filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << "\n";
        return {};
    }
    Image img(height, std::vector<Color>(width, Color{0, 0, 0, 255}));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            Color c;
            c.r = data[idx + 0];
            c.g = data[idx + 1];
            c.b = data[idx + 2];
            c.a = data[idx + 3];
            img[y][x] = c;
        }
    }
    stbi_image_free(data);
    std::cout << "Loaded image: " << filename 
              << " (width=" << width << ", height=" << height << ", forced RGBA)\n";
    return img;
}

