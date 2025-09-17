#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>

namespace p1 {

template <typename K, typename V>
class AVLTree {
public:
    struct AVLNode {
        AVLNode() = default;
        AVLNode(const K& Key, const V& Value) : Key_(Key), Value_(Value), Height_(1) {}
        K Key_;
        V Value_;
        AVLNode* Left_{};
        AVLNode* Right_{};
        int Height_{};
    };

    AVLTree() : Root_(nullptr), Size_(0), TotalDataSize_(0) {}
    
    ~AVLTree() {
        FreeAVLTree(Root_);
    }

    void Put(const K& key, const V& value) {
        size_t oldSize = GetDataSize(key, value);
        Root_ = InsertKey(Root_, key, value);
        Size_++;
        TotalDataSize_ += oldSize;
    }

    V& Get(const K& key) {
        return GetValue(Root_, key);
    }

    std::vector<std::pair<K, V>> Scan(const K& key1, const K& key2) {
        std::vector<std::pair<K, V>> result;
        InOrderTraversal(Root_, [&](const K& key, const V& value) {
            if (key >= key1 && key <= key2) {
                result.emplace_back(key, value);
            }
            return key <= key2;
        });
        return result;
    }

    void InOrderTraversal(std::function<bool(const K&, const V&)> callback) {
        InOrderTraversal(Root_, callback);
    }

    size_t GetSize() const {
        return Size_;
    }

    size_t GetTotalDataSize() const {
        return TotalDataSize_;
    }

    bool IsEmpty() const {
        return Root_ == nullptr;
    }

    void Clear() {
        FreeAVLTree(Root_);
        Root_ = nullptr;
        Size_ = 0;
        TotalDataSize_ = 0;
    }

    void PrintTree() const {
        PrintAVLTree(Root_);
    }

private:
    AVLNode* Root_;
    size_t Size_;
    size_t TotalDataSize_;

    size_t GetDataSize(const K& key, const V& value) const {
        return sizeof(K) + sizeof(V);
    }

    static void PrintAVlTree(AVLNode* Root, const std::string& Prefix = "", bool isLeft = false) {
        // Nice way of printing a tree
        // https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
        if (Root) {
            std::cout << Prefix;
            std::cout << (isLeft ? "├──" : "└──");
            std::cout << Root->Key_ << std::endl;
            PrintAVlTree(Root->Left_, Prefix + (isLeft ? "│   " : "    "), true);
            PrintAVlTree(Root->Right_, Prefix + (isLeft ? "│   " : "    "), false);
        }
    }


    static V& GetValue(AVLNode* Root, const K& key) {
        if (Root == nullptr) {
            throw std::runtime_error("Could not find provided key in AVL Tree");
        }

        if (Root->Key_ > key) {
            return GetValue(Root->Left_, key);
        }
        if (Root->Key_ < key) {
            return GetValue(Root->Right_, key);
        }
        return Root->Value_;
    }

    static AVLNode* InsertKey(AVLNode* Root, const K& Key, const V& Value) {
        if (Root == nullptr) {
            Root = new AVLNode(Key, Value);
            return Root;
        }

        // We shouldn't have duplicate keys, but we just add them to the left if it happens (according to handout)
        if (Key <= Root->Key_) {
            Root->Left_ = InsertKey(Root->Left_, Key, Value);
        } else {
            Root->Right_ = InsertKey(Root->Right_, Key, Value);
        }

        // Recalculate the height of the Root
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));

        return Rebalance(Root);
    }

    static void FreeAVLTree(AVLNode* Root) {
        if (Root == nullptr) {
            return;
        }

        FreeAVLTree(Root->Left_);
        FreeAVLTree(Root->Right_);
        delete Root;
    }

    // Get the height of the AvlTree rooted at Root
    static int GetHeight(AVLNode* Root) {
        if (Root) {
            return Root->Height_;
        }
        return 0;
    }

    // In-order traversal of the AVL tree
    static void InOrderTraversal(AVLNode* Root, std::function<bool(const K&, const V&)> callback) {
        if (Root == nullptr) {
            return;
        }
        
        InOrderTraversal(Root->Left_, callback);
        
        if (!callback(Root->Key_, Root->Value_)) {
            return;
        }
        
        InOrderTraversal(Root->Right_, callback);
    }

    static AVLNode* Rebalance(AVLNode* Root) {
        assert(Root);
        int balance = GetHeight(Root->Left_) - GetHeight(Root->Right_);
        if (balance > 1) {
            assert(Root->Left_);
            // We can have two possible cases right now
            // Either Left Right, or Left Left
            const int leftBalance = GetHeight(Root->Left_->Left_) - GetHeight(Root->Left_->Right_);
            assert(leftBalance == -1 || leftBalance == 1);
            if (leftBalance == 1) {
                // Left Left
                return RightRotate(Root);
            }

            // Left Right
            Root->Left_ = LeftRotate(Root->Left_);
            return RightRotate(Root);
        }

        if (balance < -1) {
            assert(Root->Right_);
            // We can have two possible cases right now
            // Either Right Left, or Right Right
            const int rightBalance = GetHeight(Root->Right_->Left_) - GetHeight(Root->Right_->Right_);
            assert(rightBalance == -1 || rightBalance == 1);
            if (rightBalance == -1) {
                // Right Right
                return LeftRotate(Root);
            }

            // Right Left
            Root->Right_ = RightRotate(Root->Right_);
            return LeftRotate(Root);
        }

        return Root;
    }

    // We don't do any safety checking for these functions
    // It is up to the caller that the rotation is called correctly on a particular AvlNode
    static AVLNode* LeftRotate(AVLNode* Root) {
        // This is the exact inverse of RotateRight
        AVLNode* returnNode = Root->Right_;
        Root->Right_ = returnNode->Left_;
        returnNode->Left_ = Root;

        // The order we increment this matters!!!!!
        // We have to increment the existing root height because our returnNode height calculation depends on the
        // root height calculation
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));
        returnNode->Height_ = 1 + std::max(GetHeight(returnNode->Left_), GetHeight(returnNode->Right_));

        return returnNode;
    }

    static AVLNode* RightRotate(AVLNode* Root) {
        // Reassign the nodes
        AVLNode* returnNode = Root->Left_;
        Root->Left_ = returnNode->Right_;
        returnNode->Right_ = Root;

        // Recalculate the heights
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));
        returnNode->Height_ = 1 + std::max(GetHeight(returnNode->Left_), GetHeight(returnNode->Right_));

        return returnNode;
    }
};

}
