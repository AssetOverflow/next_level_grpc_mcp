#include "tool_manager.h"
#include <chrono>
#include <stdexcept>

namespace gmcp {

bool ToolManager::RegisterTool(const ToolRegistration& registration, ToolFunction function) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (tools_.find(registration.tool_id()) != tools_.end()) {
        return false; // Tool already registered
    }
    
    auto tool_reg = std::make_shared<ToolRegistration>(registration);
    tools_[registration.tool_id()] = tool_reg;
    tool_functions_[registration.tool_id()] = function;
    
    return true;
}

ToolResult ToolManager::InvokeTool(const ToolInvocation& invocation) {
    ToolResult result;
    result.set_request_id(invocation.request_id());
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = tool_functions_.find(invocation.tool_id());
    if (it == tool_functions_.end()) {
        result.set_success(false);
        result.set_error_message("Tool not found: " + invocation.tool_id());
        return result;
    }
    
    try {
        // Convert arguments
        std::map<std::string, std::string> args;
        for (const auto& [key, value] : invocation.arguments()) {
            args[key] = value;
        }
        
        // Execute tool function
        std::string tool_result = it->second(args);
        
        result.set_success(true);
        result.set_result(tool_result);
    } catch (const std::exception& e) {
        result.set_success(false);
        result.set_error_message(std::string("Tool execution error: ") + e.what());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    result.set_execution_time_ms(duration.count());
    
    return result;
}

std::shared_ptr<ToolRegistration> ToolManager::GetTool(const std::string& tool_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tools_.find(tool_id);
    return (it != tools_.end()) ? it->second : nullptr;
}

std::vector<std::string> ToolManager::ListTools() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> tool_ids;
    for (const auto& [id, _] : tools_) {
        tool_ids.push_back(id);
    }
    return tool_ids;
}

} // namespace gmcp
