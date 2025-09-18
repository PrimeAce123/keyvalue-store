#include "../include/p1/avl_tree.hpp"
#include "../include/p1/memtable.hpp"
#include "../include/p1/sstable.hpp"
#include "../include/p1/sst_manager.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>

// Simple testing framework
class TestRunner {
private:
    int total_tests = 0;
    int passed_tests = 0;
    
public:
    void run_test(const std::string& test_name, bool (*test_func)()) {
        total_tests++;
        std::cout << "Running test: " << test_name << "... ";
        
        try {
            if (test_func()) {
                std::cout << "PASSED" << std::endl;
                passed_tests++;
            } else {
                std::cout << "FAILED" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "FAILED (Exception: " << e.what() << ")" << std::endl;
        }
    }
    
    void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << (total_tests - passed_tests) << std::endl;
        std::cout << "Success rate: " << (100.0 * passed_tests / total_tests) << "%" << std::endl;
    }
};

// AVL Tree Tests
bool test_avl_basic_operations() {
    p1::AVLTree<int, int> tree;
    
    // Test put operations
    tree.Put(5, 50);
    tree.Put(3, 30);
    tree.Put(7, 70);
    
    // Test get operations
    int value = tree.Get(5);
    if (value != 50) return false;
    
    value = tree.Get(3);
    if (value != 30) return false;
    
    value = tree.Get(7);
    if (value != 70) return false;
    
    // Test size
    if (tree.GetSize() != 3) return false;
    
    return true;
}

bool test_avl_range_query() {
    p1::AVLTree<int, int> tree;
    
    // Insert test data
    tree.Put(5, 50);
    tree.Put(3, 30);
    tree.Put(7, 70);
    tree.Put(1, 10);
    tree.Put(9, 90);
    tree.Put(4, 40);
    tree.Put(6, 60);
    
    // Test range query
    auto results = tree.Scan(3, 7);
    
    // Should return: (3,30), (4,40), (5,50), (6,60), (7,70)
    if (results.size() != 5) return false;
    if (results[0].first != 3 || results[0].second != 30) return false;
    if (results[1].first != 4 || results[1].second != 40) return false;
    if (results[2].first != 5 || results[2].second != 50) return false;
    if (results[3].first != 6 || results[3].second != 60) return false;
    if (results[4].first != 7 || results[4].second != 70) return false;
    
    return true;
}

// Memtable Tests
bool test_memtable_basic_operations() {
    p1::Memtable<uint64_t, uint64_t> memtable(1024); // 1KB limit
    
    // Test put operations
    if (!memtable.Put(1, 100)) return false;
    if (!memtable.Put(2, 200)) return false;
    if (!memtable.Put(3, 300)) return false;
    
    // Test get operations
    if (memtable.Get(1) != 100) return false;
    if (memtable.Get(2) != 200) return false;
    if (memtable.Get(3) != 300) return false;
    
    // Test entry count
    if (memtable.GetEntryCount() != 3) return false;
    
    return true;
}

bool test_memtable_scan() {
    p1::Memtable<uint64_t, uint64_t> memtable(1024);
    
    // Add some data
    memtable.Put(5, 50);
    memtable.Put(1, 10);
    memtable.Put(8, 80);
    memtable.Put(3, 30);
    memtable.Put(7, 70);
    
    // Test scan
    auto results = memtable.Scan(3, 7);
    
    // Should return: (3,30), (5,50), (7,70)
    if (results.size() != 3) return false;
    if (results[0].first != 3 || results[0].second != 30) return false;
    if (results[1].first != 5 || results[1].second != 50) return false;
    if (results[2].first != 7 || results[2].second != 70) return false;
    
    return true;
}

bool test_memtable_size_limit() {
    // Very small memtable to test size limit
    p1::Memtable<uint64_t, uint64_t> memtable(32); // 32 bytes
    
    // Each entry is 16 bytes (8+8), so we can fit 2 entries
    if (!memtable.Put(1, 100)) return false;
    if (!memtable.Put(2, 200)) return false;
    
    // Third entry should fail
    if (memtable.Put(3, 300)) return false;
    
    // Should need flush
    if (!memtable.NeedsFlush()) return false;
    
    return true;
}

// SSTable Tests
bool test_sstable_creation_and_search() {
    // Create test data
    std::vector<std::pair<uint64_t, uint64_t>> data = {
        {1, 100}, {3, 300}, {5, 500}, {7, 700}, {9, 900}
    };
    
    // Create SST file
    std::string filename = "/tmp/test_sst.db";
    if (!p1::SSTable<uint64_t, uint64_t>::CreateSST(filename, data)) {
        return false;
    }
    
    // Test reading
    p1::SSTable<uint64_t, uint64_t> sst(filename);
    if (!sst.Open()) return false;
    
    // Test get operations
    uint64_t value;
    if (!sst.Get(5, value) || value != 500) return false;
    if (!sst.Get(1, value) || value != 100) return false;
    if (!sst.Get(9, value) || value != 900) return false;
    
    // Test non-existent key
    if (sst.Get(4, value)) return false;
    
    // Test key out of range
    if (sst.Get(10, value)) return false;
    
    sst.Close();
    std::filesystem::remove(filename);
    return true;
}

bool test_sstable_scan() {
    // Create test data
    std::vector<std::pair<uint64_t, uint64_t>> data = {
        {1, 100}, {2, 200}, {3, 300}, {5, 500}, {7, 700}, {8, 800}, {9, 900}
    };
    
    // Create SST file
    std::string filename = "/tmp/test_sst_scan.db";
    if (!p1::SSTable<uint64_t, uint64_t>::CreateSST(filename, data)) {
        return false;
    }
    
    // Test scan
    p1::SSTable<uint64_t, uint64_t> sst(filename);
    if (!sst.Open()) return false;
    
    auto results = sst.Scan(3, 7);
    
    // Should return: (3,300), (5,500), (7,700)
    if (results.size() != 3) return false;
    if (results[0].first != 3 || results[0].second != 300) return false;
    if (results[1].first != 5 || results[1].second != 500) return false;
    if (results[2].first != 7 || results[2].second != 700) return false;
    
    sst.Close();
    std::filesystem::remove(filename);
    return true;
}

// SST Manager Tests
bool test_sst_manager_basic() {
    // Create test directory
    std::string db_path = "/tmp/test_db";
    std::filesystem::create_directories(db_path);
    
    p1::SSTManager<uint64_t, uint64_t> manager(db_path);
    
    // Add first SST
    std::vector<std::pair<uint64_t, uint64_t>> data1 = {
        {1, 100}, {3, 300}, {5, 500}
    };
    if (!manager.AddSST(data1)) return false;
    
    // Add second SST (newer)
    std::vector<std::pair<uint64_t, uint64_t>> data2 = {
        {2, 200}, {4, 400}, {6, 600}
    };
    if (!manager.AddSST(data2)) return false;
    
    // Test get operations
    uint64_t value;
    if (!manager.Get(1, value) || value != 100) return false;
    if (!manager.Get(2, value) || value != 200) return false;
    if (!manager.Get(6, value) || value != 600) return false;
    
    // Test non-existent key
    if (manager.Get(10, value)) return false;
    
    // Test scan
    auto results = manager.Scan(1, 6);
    if (results.size() != 6) return false;
    
    manager.Close();
    std::filesystem::remove_all(db_path);
    return true;
}

bool test_sst_manager_newest_first() {
    // Test that newer SSTs are searched first (for overwrites)
    std::string db_path = "/tmp/test_db_newest";
    std::filesystem::create_directories(db_path);
    
    p1::SSTManager<uint64_t, uint64_t> manager(db_path);
    
    // Add first SST with key 5
    std::vector<std::pair<uint64_t, uint64_t>> data1 = {
        {5, 500}
    };
    if (!manager.AddSST(data1)) return false;
    
    // Add second SST with same key but different value
    std::vector<std::pair<uint64_t, uint64_t>> data2 = {
        {5, 999}  // Should override the previous value
    };
    if (!manager.AddSST(data2)) return false;
    
    // Should get newest value
    uint64_t value;
    if (!manager.Get(5, value) || value != 999) return false;
    
    manager.Close();
    std::filesystem::remove_all(db_path);
    return true;
}

// Integration Tests
bool test_memtable_to_sst_workflow() {
    // Test the workflow of filling memtable and flushing to SST
    p1::Memtable<uint64_t, uint64_t> memtable(64); // Small limit
    
    // Fill memtable
    memtable.Put(1, 100);
    memtable.Put(3, 300);
    memtable.Put(2, 200);
    
    // Get data in sorted order (simulate flush)
    auto data = memtable.Scan(0, UINT64_MAX);
    
    // Should be sorted
    if (data.size() != 3) return false;
    if (data[0].first != 1 || data[0].second != 100) return false;
    if (data[1].first != 2 || data[1].second != 200) return false;
    if (data[2].first != 3 || data[2].second != 300) return false;
    
    // Create SST from memtable data
    std::string filename = "/tmp/test_flush.db";
    if (!p1::SSTable<uint64_t, uint64_t>::CreateSST(filename, data)) {
        return false;
    }
    
    // Verify SST contents
    p1::SSTable<uint64_t, uint64_t> sst(filename);
    if (!sst.Open()) return false;
    
    uint64_t value;
    if (!sst.Get(2, value) || value != 200) return false;
    
    sst.Close();
    std::filesystem::remove(filename);
    return true;
}

int main() {
    TestRunner runner;
    
    std::cout << "=== Running Key-Value Store Tests ===" << std::endl;
    
    // AVL Tree Tests
    std::cout << "\n--- AVL Tree Tests ---" << std::endl;
    runner.run_test("AVL Basic Operations", test_avl_basic_operations);
    runner.run_test("AVL Range Query", test_avl_range_query);
    
    // Memtable Tests
    std::cout << "\n--- Memtable Tests ---" << std::endl;
    runner.run_test("Memtable Basic Operations", test_memtable_basic_operations);
    runner.run_test("Memtable Scan", test_memtable_scan);
    runner.run_test("Memtable Size Limit", test_memtable_size_limit);
    
    // SSTable Tests
    std::cout << "\n--- SSTable Tests ---" << std::endl;
    runner.run_test("SSTable Creation and Search", test_sstable_creation_and_search);
    runner.run_test("SSTable Scan", test_sstable_scan);
    
    // SST Manager Tests
    std::cout << "\n--- SST Manager Tests ---" << std::endl;
    runner.run_test("SST Manager Basic", test_sst_manager_basic);
    runner.run_test("SST Manager Newest First", test_sst_manager_newest_first);
    
    // Integration Tests
    std::cout << "\n--- Integration Tests ---" << std::endl;
    runner.run_test("Memtable to SST Workflow", test_memtable_to_sst_workflow);
    
    runner.print_summary();
    
    return 0;
}