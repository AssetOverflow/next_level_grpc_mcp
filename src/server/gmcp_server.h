#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "gmcp.grpc.pb.h"
#include "tool_manager.h"
#include "memory_manager.h"

namespace gmcp {

class AgentCoordinationServiceImpl final : public AgentCoordination::Service {
public:
    AgentCoordinationServiceImpl();
    ~AgentCoordinationServiceImpl() override;

    // Bidirectional streaming for agent messages
    grpc::Status StreamAgentMessages(
        grpc::ServerContext* context,
        grpc::ServerReaderWriter<AgentMessage, AgentMessage>* stream) override;

    // Tool registration
    grpc::Status RegisterTool(
        grpc::ServerContext* context,
        const ToolRegistration* request,
        RegistrationResponse* response) override;

    // Memory registration
    grpc::Status RegisterMemory(
        grpc::ServerContext* context,
        const MemoryRegistration* request,
        RegistrationResponse* response) override;

    // Tool invocation
    grpc::Status InvokeTool(
        grpc::ServerContext* context,
        const ToolInvocation* request,
        ToolResult* response) override;

    // Memory query
    grpc::Status QueryMemory(
        grpc::ServerContext* context,
        const MemoryQuery* request,
        MemoryResult* response) override;

    // Event subscription
    grpc::Status SubscribeEvents(
        grpc::ServerContext* context,
        const EventSubscription* request,
        grpc::ServerWriter<Event>* writer) override;

    // Initialize example tools and memory stores
    void InitializeExamples();

private:
    std::unique_ptr<ToolManager> tool_manager_;
    std::unique_ptr<MemoryManager> memory_manager_;
    
    // Event queue for subscriptions
    std::mutex event_mutex_;
    std::condition_variable event_cv_;
    std::queue<Event> event_queue_;
    
    void PublishEvent(const Event& event);
};

} // namespace gmcp
