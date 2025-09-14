#include "catch/catch.hpp"
#include "p1/avl_tree.hpp"

TEST_CASE("AVL Tree", "[avl]") {
    p1::AVlTree<uint64_t, uint64_t> tree{};

    tree.Insert(1, 100);
    tree.Insert(2, 200);
    tree.Insert(3, 300);
    tree.Insert(4, 400);
    tree.Insert(5, 500);
    tree.Insert(6, 600);
    tree.PrintAVlTree();
    REQUIRE(tree.Get(1) == 100);

}


