#pragma once

#include "sstable.hpp"
#include <vector>
#include <memory>
#include <filesystem>
#include <algorithm>

namespace p1 {

template<typename K, typename V>
class SSTManager {
public:
    explicit SSTManager(const std::string& db_path) : db_path_(db_path) {
        std::filesystem::create_directories(db_path_);
    }

    // Add a new SST
    bool AddSST(const std::vector<std::pair<K, V>>& sorted_data) {
        std::string filename = SSTable<K, V>::GenerateFilename(db_path_);
        
        if (!SSTable<K, V>::CreateSST(filename, sorted_data)) {
            return false;
        }

        // Add to the existing list of SSTs, keeping the order from newest to oldest for search order
        auto sst = std::make_unique<SSTable<K, V>>(filename);
        sst_files_.insert(sst_files_.begin(), std::move(sst));
        
        return true;
    }

    bool Get(const K& key, V& value) {
        for (auto& sst : sst_files_) {
            if (sst->Get(key, value)) {
                return true;
            }
        }
        return false;
    }

    // Scan across all SSTs and merge results
    std::vector<std::pair<K, V>> Scan(const K& start_key, const K& end_key) {
        std::vector<std::pair<K, V>> all_results;
        
        for (auto& sst : sst_files_) {
            auto sst_results = sst->Scan(start_key, end_key);
            all_results.insert(all_results.end(), sst_results.begin(), sst_results.end());
        }
        std::sort(all_results.begin(), all_results.end());
        auto new_end = std::unique(all_results.begin(), all_results.end(),
            [](const std::pair<K, V>& a, const std::pair<K, V>& b) {
                return a.first == b.first;
            });
        all_results.erase(new_end, all_results.end());

        return all_results;
    }

    bool LoadExistingSSTs() {
        if (!std::filesystem::exists(db_path_)) {
            return true;
        }

        std::vector<std::string> sst_filenames;
        
        // Find all SST files
        for (const auto& entry : std::filesystem::directory_iterator(db_path_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".db") {
                std::string filename = entry.path().filename().string();
                if (filename.substr(0, 4) == "sst_") {
                    sst_filenames.push_back(entry.path().string());
                }
            }
        }

        std::sort(sst_filenames.rbegin(), sst_filenames.rend());

        for (const auto& filename : sst_filenames) {
            auto sst = std::make_unique<SSTable<K, V>>(filename);
            if (sst->Open()) {
                sst_files_.push_back(std::move(sst));
            }
        }

        return true;
    }

    size_t GetSSTCount() const {
        return sst_files_.size();
    }

    size_t GetTotalEntries() const {
        size_t total = 0;
        for (const auto& sst : sst_files_) {
            // Note: This requires the SST to be opened first
            total += sst->GetHeader().num_entries;
        }
        return total;
    }

    const std::string& GetDatabasePath() const {
        return db_path_;
    }

    void Close() {
        for (auto& sst : sst_files_) {
            sst->Close();
        }
    }

private:
    std::string db_path_;
    std::vector<std::unique_ptr<SSTable<K, V>>> sst_files_;
};

}