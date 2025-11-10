#include "memory_manager.h"
#include <chrono>
#include <algorithm>

namespace gmcp {

bool MemoryManager::RegisterMemory(const MemoryRegistration& registration) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (memories_.find(registration.memory_id()) != memories_.end()) {
        return false; // Memory store already registered
    }
    
    auto mem_reg = std::make_shared<MemoryRegistration>(registration);
    memories_[registration.memory_id()] = mem_reg;
    storage_[registration.memory_id()] = std::map<std::string, MemoryEntry>();
    
    return true;
}

bool MemoryManager::Store(const std::string& memory_id, const MemoryEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (memories_.find(memory_id) == memories_.end()) {
        return false; // Memory store not registered
    }
    
    storage_[memory_id][entry.key()] = entry;
    return true;
}

MemoryResult MemoryManager::Query(const MemoryQuery& query) {
    MemoryResult result;
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto mem_it = storage_.find(query.memory_id());
    if (mem_it == storage_.end()) {
        return result; // Empty result
    }
    
    const auto& store = mem_it->second;
    
    switch (query.query_type()) {
        case QueryType::GET: {
            // Get specific key
            auto it = store.find(query.query());
            if (it != store.end()) {
                *result.add_entries() = it->second;
                result.set_total_count(1);
            }
            break;
        }
        
        case QueryType::LIST: {
            // List all entries
            int limit = query.limit() > 0 ? query.limit() : 100;
            int count = 0;
            for (const auto& [key, entry] : store) {
                if (count >= limit) {
                    result.set_has_more(true);
                    break;
                }
                *result.add_entries() = entry;
                count++;
            }
            result.set_total_count(store.size());
            break;
        }
        
        case QueryType::SEARCH: {
            // Simple substring search in keys and values
            int limit = query.limit() > 0 ? query.limit() : 100;
            int count = 0;
            for (const auto& [key, entry] : store) {
                if (count >= limit) {
                    result.set_has_more(true);
                    break;
                }
                if (key.find(query.query()) != std::string::npos ||
                    entry.value().find(query.query()) != std::string::npos) {
                    *result.add_entries() = entry;
                    count++;
                }
            }
            result.set_total_count(count);
            break;
        }
        
        default:
            break;
    }
    
    return result;
}

std::shared_ptr<MemoryRegistration> MemoryManager::GetMemory(const std::string& memory_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = memories_.find(memory_id);
    return (it != memories_.end()) ? it->second : nullptr;
}

std::vector<std::string> MemoryManager::ListMemories() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> memory_ids;
    for (const auto& [id, _] : memories_) {
        memory_ids.push_back(id);
    }
    return memory_ids;
}

} // namespace gmcp
