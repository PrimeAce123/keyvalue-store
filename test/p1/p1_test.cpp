#include "catch/catch.hpp"
#include "p1/avl_tree.hpp"

TEST_CASE("AVL Tree", "[avl]") {
    p1::AVlTree<uint64_t, uint64_t> tree{};

    tree.Insert(6, 100);
    tree.Insert(5, 200);
    tree.Insert(4, 300);
    tree.Insert(3, 400);
    tree.Insert(2, 500);
    tree.Insert(1, 600);
    tree.PrintAVlTree();
    REQUIRE(tree.Get(1) == 600);
}


