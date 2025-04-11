#ifndef QUADTREENODE_HPP
#define QUADTREENODE_HPP

#include "utils.hpp"
#include <vector>

extern std::vector<Image> gifFrames;
void capture_frame(const Image& img);

class QuadTreeNode {
    private:
        int x, y;
        int width, height;
        bool is_leaf;
        Color color;
        QuadTreeNode* top_left_child;
        QuadTreeNode* top_right_child;
        QuadTreeNode* bottom_left_child;
        QuadTreeNode* bottom_right_child;
    public:
    /* Konstruktor untuk QuadTreeNode */
        QuadTreeNode(int _x, int _y, int _width, int _height);
    /* Destruktor untuk QuadTreeNode */
        ~QuadTreeNode();
    /* Melakukan pembagian terhadap QuadTreeNode berdasarkan threshold dan min_block_size */
        void divide_node(const Image& image, int error_choice, double threshold, int min_block_size);
    /* Penggambaran node untuk mode cepat */
        void draw_node_fast(Image& result_image);
    /* Penggambaran node untuk mode gradual GIF */
        void draw_node_at_depth(Image& result_image, int currentDepth, int maxDepth);
    /* Menghitung jumlah daun dari suatu QuadTreeNode */
        int count_leaves() const;
    /* Menghitung kedalaman maksimal dari suatu QuadTreeNode */
        int max_depth() const;
    /* Menghitung kedalaman dari suatu QuadTreeNode */
        int get_depth() const;
    /* Menghitung jumlah dari suatu QuadTreeNode */
        int get_node_count() const;
};

#endif
