#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <thread>
#include "gmcp.grpc.pb.h"

namespace gmcp {

class AgentClient {
public:
    AgentClient(std::shared_ptr<grpc::Channel> channel);
    ~AgentClient();

    // Register a tool
    bool RegisterTool(const ToolRegistration& tool);
    
    // Register a memory store
    bool RegisterMemory(const MemoryRegistration& memory);
    
    // Invoke a tool
    ToolResult InvokeTool(const std::string& tool_id, 
                         const std::map<std::string, std::string>& args);
    
    // Query memory
    MemoryResult QueryMemory(const std::string& memory_id,
                            QueryType type,
                            const std::string& query,
                            int limit = 10);
    
    // Start bidirectional streaming
    void StartStreaming(const std::string& agent_id);
    
    // Send a message through the stream
    bool SendMessage(const AgentMessage& message);
    
    // Subscribe to events
    void SubscribeEvents(const std::string& agent_id,
                        const std::vector<std::string>& event_types);
    
    // Stop streaming
    void StopStreaming();

private:
    std::unique_ptr<AgentCoordination::Stub> stub_;
    
    // Streaming state
    std::unique_ptr<grpc::ClientContext> stream_context_;
    std::unique_ptr<grpc::ClientReaderWriter<AgentMessage, AgentMessage>> stream_;
    std::unique_ptr<std::thread> receive_thread_;
    bool streaming_;
    
    void ReceiveMessages();
};

} // namespace gmcp
