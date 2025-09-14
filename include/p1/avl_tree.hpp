#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>

namespace p1 {

template <typename K, typename V>
class AVlTree {
public:
    struct AVlNode {
        AVlNode() = default;
        AVlNode(const K& Key, const V& Value) : Key_(Key), Value_(Value), Height_(1) {}
        K Key_;
        V Value_;
        AVlNode* Left_{};
        AVlNode* Right_{};
        int Height_{};
    };

public:
    AVlTree() = default;
    void Insert(const K& key, const V& value) {
        Root_ = InsertInternal(Root_, key, value);
    }

    // For now, we assume that the key MUST be in the tree
    // If it isn't in the tree, we just throw an error
    V& Get(const K& key) {
        return GetInternal(Root_, key);
    }

    // Helper function to print the tree
    void PrintAVlTree() {
        if (!Root_) {
            std::cout << "Empty AVlTree" << std::endl;
            return;
        }

        std::queue<AVlNode*> nodes;
        nodes.push(Root_);
        while (!nodes.empty()) {
            auto currNode = nodes.front();
            nodes.pop();
            std::cout << currNode->Key_ << " ";
            if (currNode->Left_) {
                nodes.push(currNode->Left_);
            }

            if (currNode->Right_) {
                nodes.push(currNode->Right_);
            }
        }
        std::cout << std::endl;
    }

private:
    static V& GetInternal(AVlNode* Root, const K& key) {
        if (!Root) {
            throw std::runtime_error("Could not find provided key in AVL Tree");
        }

        if (Root->Key_ > key) {
            return GetInternal(Root->Left_, key);
        }
        if (Root->Key_ < key) {
            return GetInternal(Root->Right_, key);
        }
        return Root->Value_;
    }

    static AVlNode* InsertInternal(AVlNode* Root, const K& Key, const V& Value) {
        if (!Root) {
            Root = new AVlNode(Key, Value);
            return Root;
        }

        // We shouldn't have duplicate keys, but we just add them to the left if it happens (according to handout)
        if (Key <= Root->Key_) {
            Root->Left_ = InsertInternal(Root->Left_, Key, Value);
        } else {
            Root->Right_ = InsertInternal(Root->Right_, Key, Value);
        }

        // Recalculate the height of the Root
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));

        return Rebalance(Root);
    };

    static AVlNode* Rebalance(AVlNode* Root) {
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
    static AVlNode* LeftRotate(AVlNode* Root) {
        // This is the exact inverse of RotateRight
        AVlNode* returnNode = Root->Right_;
        Root->Right_ = returnNode->Left_;
        returnNode->Left_ = Root;

        // The order we increment this matters!!!!!
        // We have to increment the existing root height because our returnNode height calculation depends on the
        // root height calculation
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));
        returnNode->Height_ = 1 + std::max(GetHeight(returnNode->Left_), GetHeight(returnNode->Right_));

        return returnNode;
    }

    static AVlNode* RightRotate(AVlNode* Root) {
        // Reassign the nodes
        AVlNode* returnNode = Root->Left_;
        Root->Left_ = returnNode->Right_;
        returnNode->Right_ = Root;

        // Recalculate the heights
        Root->Height_ = 1 + std::max(GetHeight(Root->Left_), GetHeight(Root->Right_));
        returnNode->Height_ = 1 + std::max(GetHeight(returnNode->Left_), GetHeight(returnNode->Right_));

        return returnNode;
    }

    // Get the height of the AvlTree rooted at Root
    static int GetHeight(AVlNode* Root) {
        if (Root) {
            return Root->Height_;
        }
        return 0;
    }

private:
    AVlNode* Root_;
};

}
