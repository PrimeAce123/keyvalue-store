#include "catch/catch.hpp"
#include "p1/avl_tree.hpp"

TEST_CASE("AVL Tree", "[avl]") {
    using Tree = p1::AVlTree<uint64_t, uint64_t>;

    Tree::AVlNode *root = nullptr;
    root = Tree::InsertKey(root, 1, 100);
    root = Tree::InsertKey(root, 2, 200);
    root = Tree::InsertKey(root, 3, 300);
    root = Tree::InsertKey(root, 4, 400);
    root = Tree::InsertKey(root, 5, 500);
    root = Tree::InsertKey(root, 6, 600);
    Tree::PrintAVlTree(root);
    REQUIRE(Tree::GetValue(root, 1) == 100);
    Tree::FreeAVLTree(root);
}

TEST_CASE("Left Right", "[avl]") {
    WARN("Left Right");
    using Tree = p1::AVlTree<uint64_t, uint64_t>;

    Tree::AVlNode *root = nullptr;
    root = Tree::InsertKey(root, 3, 300);
    root = Tree::InsertKey(root, 1, 100);
    root = Tree::InsertKey(root, 2, 200);
    Tree::PrintAVlTree(root);
    REQUIRE(Tree::GetValue(root, 1) == 100);
    REQUIRE_THROWS(Tree::GetValue(root, 4));
    Tree::FreeAVLTree(root);
}

TEST_CASE("Left Left", "[avl]") {
    WARN("Left Left");
    using Tree = p1::AVlTree<uint64_t, uint64_t>;

    Tree::AVlNode *root = nullptr;
    root = Tree::InsertKey(root, 3, 300);
    root = Tree::InsertKey(root, 2, 200);
    root = Tree::InsertKey(root, 1, 100);
    Tree::PrintAVlTree(root);
    REQUIRE(Tree::GetValue(root, 1) == 100);
    REQUIRE_THROWS(Tree::GetValue(root, 4));
    Tree::FreeAVLTree(root);
}

TEST_CASE("Right Left", "[avl]") {
    WARN("Right Left");
    using Tree = p1::AVlTree<uint64_t, uint64_t>;

    Tree::AVlNode *root = nullptr;
    root = Tree::InsertKey(root, 1, 100);
    root = Tree::InsertKey(root, 3, 300);
    root = Tree::InsertKey(root, 2, 200);
    Tree::PrintAVlTree(root);
    REQUIRE(Tree::GetValue(root, 1) == 100);
    REQUIRE_THROWS(Tree::GetValue(root, 4));
    Tree::FreeAVLTree(root);
}

TEST_CASE("Right Right", "[avl]") {
    WARN("Right Right");
    using Tree = p1::AVlTree<uint64_t, uint64_t>;

    Tree::AVlNode *root = nullptr;
    root = Tree::InsertKey(root, 1, 100);
    root = Tree::InsertKey(root, 2, 200);
    root = Tree::InsertKey(root, 3, 300);
    Tree::PrintAVlTree(root);
    REQUIRE(Tree::GetValue(root, 1) == 100);
    REQUIRE_THROWS(Tree::GetValue(root, 4));
    Tree::FreeAVLTree(root);
}



