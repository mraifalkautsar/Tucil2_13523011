#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "quadtreenode.hpp"
#include "utils.hpp"
#include <string>

class QuadTree {
    private:
        QuadTreeNode* root;
        Image source_image_matrix;
        Image result_image_matrix;
    public:
    /* Constructor QuadTree dengan referensi ke input image */
        QuadTree(const Image& input_image);
    /* Destructor QuadTree */
        ~QuadTree();
    /* Membangun QuadTree berdasarkan parameter dan gambar yang ada */
        void build_quad_tree(int error_choice, double threshold, int min_block_size);
    /* Mode cepat pembuatan gambar dari QuadTree (untuk opsi tanpa GIF) */
        void build_image_fast();
    /* Mode gradual pembuatan gambar dari QuadTree (untuk opsi dengan GIF) */
        void build_image_gradual(); 
    /* Menyimpan gambar RGBA menjadi file */
        void save_image_rgba_with_format(const std::string& filename, const std::string& srcFormat, bool smallfile);
    /* Menyimpan GIF */
        void save_gif(const std::string& gifOutputPath, int delay);
    /* Memperoleh kedalaman pohon */
        int get_tree_depth() const;
    /* Memperoleh jumlah simpul dalam pohon */
        int get_total_node_count() const;
    /* Metode untuk mode target compression */
        void adjust_threshold_for_target(int errorChoice,
            double targetCompression, double tolerance = 2.0,
            int maxIterations = 30);
    /* Memperoleh jumlah daun dalam pohon*/
        int get_leaf_count() const; 
};

#endif
