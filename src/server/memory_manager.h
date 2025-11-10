#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include "gmcp.grpc.pb.h"

namespace gmcp {

class MemoryManager {
public:
    MemoryManager() = default;
    ~MemoryManager() = default;

    // Register a memory store
    bool RegisterMemory(const MemoryRegistration& registration);
    
    // Store a memory entry
    bool Store(const std::string& memory_id, const MemoryEntry& entry);
    
    // Query memory
    MemoryResult Query(const MemoryQuery& query);
    
    // Get memory registration by ID
    std::shared_ptr<MemoryRegistration> GetMemory(const std::string& memory_id);
    
    // List all registered memory stores
    std::vector<std::string> ListMemories() const;

private:
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<MemoryRegistration>> memories_;
    // Simple in-memory storage: memory_id -> key -> MemoryEntry
    std::map<std::string, std::map<std::string, MemoryEntry>> storage_;
};

} // namespace gmcp
