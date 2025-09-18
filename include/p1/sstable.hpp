#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

namespace p1 {

// SST Header structure
struct SSTHeader {
    uint32_t version;
    uint64_t num_entries;
    uint64_t creation_timestamp;
    uint64_t min_key;
    uint64_t max_key;
    uint64_t header_size;  // Size of header for offset calculations
};

// SST Entry structure
template<typename K, typename V>
struct SSTEntry {
    K key;
    V value;
};

template<typename K, typename V>
class SSTable {
public:
    // Constructor
    explicit SSTable(const std::string& filename) 
        : filename_(filename), fd_(-1) {}

    // Destructor
    ~SSTable() {
        Close();
    }

    // Create SST from sorted key-value pairs
    static bool CreateSST(const std::string& filename, 
                         const std::vector<std::pair<K, V>>& sorted_data) {
        if (sorted_data.empty()) {
            return false;
        }

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Create header
        SSTHeader header;
        header.version = 1;
        header.num_entries = sorted_data.size();
        header.creation_timestamp = GetCurrentTimestamp();
        header.min_key = static_cast<uint64_t>(sorted_data.front().first);
        header.max_key = static_cast<uint64_t>(sorted_data.back().first);
        header.header_size = sizeof(SSTHeader);

        // Write header
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        if (!file.good()) {
            return false;
        }

        // Write entries
        for (const auto& pair : sorted_data) {
            SSTEntry<K, V> entry;
            entry.key = pair.first;
            entry.value = pair.second;
            file.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
            if (!file.good()) {
                return false;
            }
        }

        file.close();
        return true;
    }

    // Generate SST filename with timestamp
    static std::string GenerateFilename(const std::string& db_path) {
        auto timestamp = GetCurrentTimestamp();
        return db_path + "/sst_" + std::to_string(timestamp) + ".db";
    }

    // Open SST for reading
    bool Open() {
        if (fd_ != -1) {
            return true; // Already open
        }

        fd_ = open(filename_.c_str(), O_RDONLY);
        if (fd_ == -1) {
            return false;
        }

        // Read header
        if (pread(fd_, &header_, sizeof(header_), 0) != sizeof(header_)) {
            close(fd_);
            fd_ = -1;
            return false;
        }

        return true;
    }

    // Close SST
    void Close() {
        if (fd_ != -1) {
            close(fd_);
            fd_ = -1;
        }
    }

    // Binary search for a key in the SST
    bool Get(const K& key, V& value) {
        if (!Open()) {
            return false;
        }

        // Check if key is in range
        if (static_cast<uint64_t>(key) < header_.min_key || 
            static_cast<uint64_t>(key) > header_.max_key) {
            return false;
        }

        int64_t left = 0;
        int64_t right = header_.num_entries - 1;
        
        while (left <= right) {
            int64_t mid = left + (right - left) / 2;
            
            // Calculate offset for entry
            off_t offset = header_.header_size + mid * sizeof(SSTEntry<K, V>);
            
            SSTEntry<K, V> entry;
            if (pread(fd_, &entry, sizeof(entry), offset) != sizeof(entry)) {
                return false;
            }

            if (entry.key == key) {
                value = entry.value;
                return true;
            } else if (entry.key < key) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        return false;
    }

    // Scan for entries in range [start_key, end_key]
    std::vector<std::pair<K, V>> Scan(const K& start_key, const K& end_key) {
        std::vector<std::pair<K, V>> result;
        
        if (!Open()) {
            return result;
        }

        // Check if range overlaps with SST range
        if (static_cast<uint64_t>(end_key) < header_.min_key || 
            static_cast<uint64_t>(start_key) > header_.max_key) {
            return result;
        }

        // Find start position using binary search
        int64_t start_pos = FindFirstGTE(start_key);
        if (start_pos == -1) {
            return result;
        }

        // Read entries sequentially from start position
        for (int64_t i = start_pos; i < static_cast<int64_t>(header_.num_entries); ++i) {
            off_t offset = header_.header_size + i * sizeof(SSTEntry<K, V>);
            
            SSTEntry<K, V> entry;
            if (pread(fd_, &entry, sizeof(entry), offset) != sizeof(entry)) {
                break;
            }

            if (entry.key > end_key) {
                break;
            }

            if (entry.key >= start_key) {
                result.emplace_back(entry.key, entry.value);
            }
        }

        return result;
    }

    // Get SST metadata
    const SSTHeader& GetHeader() const {
        return header_;
    }

    // Check if SST contains key in range
    bool ContainsKeyRange(const K& start_key, const K& end_key) const {
        return !(static_cast<uint64_t>(end_key) < header_.min_key || 
                 static_cast<uint64_t>(start_key) > header_.max_key);
    }

    // Get filename
    const std::string& GetFilename() const {
        return filename_;
    }

private:
    std::string filename_;
    int fd_;
    SSTHeader header_;

    // Get current timestamp in microseconds
    static uint64_t GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    // Binary search to find first entry >= key
    int64_t FindFirstGTE(const K& key) {
        int64_t left = 0;
        int64_t right = header_.num_entries - 1;
        int64_t result = -1;

        while (left <= right) {
            int64_t mid = left + (right - left) / 2;
            
            off_t offset = header_.header_size + mid * sizeof(SSTEntry<K, V>);
            
            SSTEntry<K, V> entry;
            if (pread(fd_, &entry, sizeof(entry), offset) != sizeof(entry)) {
                return -1;
            }

            if (entry.key >= key) {
                result = mid;
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        return result;
    }

    // Prevent copying
    SSTable(const SSTable&) = delete;
    SSTable& operator=(const SSTable&) = delete;
};

} // namespace p1