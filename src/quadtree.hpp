#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "quadtreenode.hpp"
#include "utils.hpp"
#include <string>

class QuadTree {
    private:
        bool gif_mode = false;
        QuadTreeNode* root;
        Image source_image_matrix;
        Image result_image_matrix;
    public:
        QuadTree(const Image& input_image);
        ~QuadTree();
        void build_quad_tree(int error_choice, double threshold, int min_block_size);
        void build_image();
        void build_image_gradual();  // builds gradual frames for GIF visualization
        const Image& get_result_image() const;
        void save_gif(const std::string& gifOutputPath, int delay);
        void enable_gif_mode();
        int get_leaf_count() const; // for compression target
        void save_image_rgba_with_format(const std::string& filename, const std::string& srcFormat);
};

#endif
