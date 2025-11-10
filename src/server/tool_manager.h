#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <mutex>
#include "gmcp.grpc.pb.h"

namespace gmcp {

// Tool function signature
using ToolFunction = std::function<std::string(const std::map<std::string, std::string>&)>;

class ToolManager {
public:
    ToolManager() = default;
    ~ToolManager() = default;

    // Register a new tool
    bool RegisterTool(const ToolRegistration& registration, ToolFunction function);
    
    // Invoke a registered tool
    ToolResult InvokeTool(const ToolInvocation& invocation);
    
    // Get tool registration by ID
    std::shared_ptr<ToolRegistration> GetTool(const std::string& tool_id);
    
    // List all registered tools
    std::vector<std::string> ListTools() const;

private:
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<ToolRegistration>> tools_;
    std::map<std::string, ToolFunction> tool_functions_;
};

} // namespace gmcp
