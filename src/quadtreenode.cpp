#include "quadtreenode.hpp"
#include "utils.hpp"
#include <iostream>

// Define the global gifFrames vector.
std::vector<Image> gifFrames;

// Define the capture_frame helper.
void capture_frame(const Image& img) {
    gifFrames.push_back(img);
}

QuadTreeNode::QuadTreeNode(int _x, int _y, int _width, int _height)
    : x(_x), y(_y), width(_width), height(_height),
      is_leaf(false),
      top_left_child(nullptr), top_right_child(nullptr),
      bottom_left_child(nullptr), bottom_right_child(nullptr)
{
    color = {0, 0, 0, 255};
}

QuadTreeNode::~QuadTreeNode() {
    delete top_left_child;
    delete top_right_child;
    delete bottom_left_child;
    delete bottom_right_child;
}

void QuadTreeNode::divide_node(const Image& image, int error_choice, double threshold, int min_block_size) {
    double error = compute_block_error(image, x, y, width, height, error_choice);
    if ((error > threshold) && (width > min_block_size) && (height > min_block_size)) {
        int halfW = width / 2;
        int halfH = height / 2;
        
        top_left_child = new QuadTreeNode(x, y, halfW, halfH);
        top_left_child->divide_node(image, error_choice, threshold, min_block_size);
        
        top_right_child = new QuadTreeNode(x + halfW, y, width - halfW, halfH);
        top_right_child->divide_node(image, error_choice, threshold, min_block_size);
        
        bottom_left_child = new QuadTreeNode(x, y + halfH, halfW, height - halfH);
        bottom_left_child->divide_node(image, error_choice, threshold, min_block_size);
        
        bottom_right_child = new QuadTreeNode(x + halfW, y + halfH, width - halfW, height - halfH);
        bottom_right_child->divide_node(image, error_choice, threshold, min_block_size);
    } else {
        is_leaf = true;
        color = compute_average_color(image, x, y, width, height);
    }
}

void QuadTreeNode::draw_node(Image& result_image, bool gif_mode) {
    if (is_leaf) {
        for (int row = y; row < y + height; ++row) {
            for (int col = x; col < x + width; ++col) {
                result_image[row][col] = color;
            }
        }
        if (gif_mode) {
            capture_frame(result_image);
        }
    } else {
        if (top_left_child)    top_left_child->draw_node(result_image, gif_mode);
        if (top_right_child)   top_right_child->draw_node(result_image, gif_mode);
        if (bottom_left_child) bottom_left_child->draw_node(result_image, gif_mode);
        if (bottom_right_child) bottom_right_child->draw_node(result_image, gif_mode);
    }
}

void QuadTreeNode::draw_node_at_depth(Image& result_image, int currentDepth, int maxDepth) {
    if (currentDepth >= maxDepth || is_leaf) {
        for (int row = y; row < y + height; ++row) {
            for (int col = x; col < x + width; ++col) {
                result_image[row][col] = color;
            }
        }
        return;
    }
    if (top_left_child)    top_left_child->draw_node_at_depth(result_image, currentDepth + 1, maxDepth);
    if (top_right_child)   top_right_child->draw_node_at_depth(result_image, currentDepth + 1, maxDepth);
    if (bottom_left_child) bottom_left_child->draw_node_at_depth(result_image, currentDepth + 1, maxDepth);
    if (bottom_right_child) bottom_right_child->draw_node_at_depth(result_image, currentDepth + 1, maxDepth);
}

int QuadTreeNode::count_leaves() const {
    if (is_leaf)
        return 1;
    int count = 0;
    if (top_left_child)    count += top_left_child->count_leaves();
    if (top_right_child)   count += top_right_child->count_leaves();
    if (bottom_left_child) count += bottom_left_child->count_leaves();
    if (bottom_right_child)count += bottom_right_child->count_leaves();
    return count;
}
