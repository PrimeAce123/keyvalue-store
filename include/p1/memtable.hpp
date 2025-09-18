#pragma once

#include "avl_tree.hpp"

#include <cstdint>
#include <memory>
#include <vector>
#include <stdexcept>

namespace p1 {

template<typename K, typename V>
class Memtable {
public:
    explicit Memtable(size_t size_limit) 
        : size_limit_(size_limit), current_size_(0) {}

    ~Memtable() = default;

    bool Put(const K& key, const V& value) {
        // Calculate size of new entry, assuming fixed sizes
        const size_t entry_size = sizeof(K) + sizeof(V);
        
        if (current_size_ + entry_size > size_limit_) {
            return false;
        }

        // Insert into AVL tree
        tree_.Put(key, value);
        current_size_ += entry_size;
        return true;
    }

    // Get a value by key
    V Get(const K& key) const {
        return tree_.Get(key);
    }

    // Scan for all key-value pairs in range [key1, key2]
    std::vector<std::pair<K, V>> Scan(const K& key1, const K& key2) const {
        return tree_.Scan(key1, key2);
    }

    size_t GetCurrentSize() const {
        return current_size_;
    }

    size_t GetSizeLimit() const {
        return size_limit_;
    }

    // Check if memtable needs to be flushed
    bool NeedsFlush() const {
        return current_size_ >= size_limit_;
    }

    size_t GetEntryCount() const {
        return tree_.GetSize();
    }

    void Clear() {
        tree_.Clear();
        current_size_ = 0;
    }

    // Get iterator to the beginning (for in-order traversal)
    auto begin() {
        return tree_.begin();
    }

    // Get iterator to the end
    auto end() {
        return tree_.end();
    }

    // Get const iterator to the beginning
    auto begin() const {
        return tree_.begin();
    }

    // Get const iterator to the end
    auto end() const {
        return tree_.end();
    }

private:
    AVLTree<K, V> tree_;
    const size_t size_limit_;    
    size_t current_size_;

    Memtable(const Memtable&) = delete;
    Memtable& operator=(const Memtable&) = delete;
};

}