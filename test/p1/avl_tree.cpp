#include "catch/catch.hpp"
#include "p1/avl_tree.hpp"

TEST_CASE("AVL Tree", "[avl]") {
    using Tree = p1::AVLTree<uint64_t, uint64_t>;

    Tree root;
    root.Put(1, 100);
    root.Put(2, 200);
    root.Put(3, 300);
    root.Put(4, 400);
    root.Put(5, 500);
    root.Put(6, 600);

    REQUIRE(root.Get(1) == 100);
}




