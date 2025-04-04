#ifndef QUADTREENODE_HPP
#define QUADTREENODE_HPP

#include "utils.hpp"
#include <vector>

// Declare global GIF frames.
extern std::vector<Image> gifFrames;
// Declare capture_frame (defined in quadtreenode.cpp).
void capture_frame(const Image& img);

class QuadTreeNode {
    private:
        int x, y; // Top-left corner of block.
        int width, height;
        bool is_leaf;
        Color color;
        QuadTreeNode* top_left_child;
        QuadTreeNode* top_right_child;
        QuadTreeNode* bottom_left_child;
        QuadTreeNode* bottom_right_child;
    public:
        QuadTreeNode(int _x, int _y, int _width, int _height);
        ~QuadTreeNode();
        void divide_node(const Image& image, int error_choice, double threshold, int min_block_size);
        void draw_node(Image& result_image, bool gif_mode);
        void draw_node_at_depth(Image& result_image, int currentDepth, int maxDepth);
        int count_leaves() const;
};

#endif
