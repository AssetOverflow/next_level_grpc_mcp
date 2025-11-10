#include "gmcp_client.h"
#include <iostream>
#include <chrono>

namespace gmcp {

AgentClient::AgentClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(AgentCoordination::NewStub(channel)), streaming_(false) {
}

AgentClient::~AgentClient() {
    StopStreaming();
}

bool AgentClient::RegisterTool(const ToolRegistration& tool) {
    grpc::ClientContext context;
    RegistrationResponse response;
    
    grpc::Status status = stub_->RegisterTool(&context, tool, &response);
    
    if (status.ok()) {
        std::cout << "Tool registration: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return false;
    }
}

bool AgentClient::RegisterMemory(const MemoryRegistration& memory) {
    grpc::ClientContext context;
    RegistrationResponse response;
    
    grpc::Status status = stub_->RegisterMemory(&context, memory, &response);
    
    if (status.ok()) {
        std::cout << "Memory registration: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return false;
    }
}

ToolResult AgentClient::InvokeTool(const std::string& tool_id,
                                   const std::map<std::string, std::string>& args) {
    grpc::ClientContext context;
    ToolInvocation invocation;
    ToolResult result;
    
    invocation.set_tool_id(tool_id);
    invocation.set_request_id("req_" + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count()));
    
    for (const auto& [key, value] : args) {
        (*invocation.mutable_arguments())[key] = value;
    }
    
    grpc::Status status = stub_->InvokeTool(&context, invocation, &result);
    
    if (!status.ok()) {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
    
    return result;
}

MemoryResult AgentClient::QueryMemory(const std::string& memory_id,
                                     QueryType type,
                                     const std::string& query,
                                     int limit) {
    grpc::ClientContext context;
    MemoryQuery mem_query;
    MemoryResult result;
    
    mem_query.set_memory_id(memory_id);
    mem_query.set_query_type(type);
    mem_query.set_query(query);
    mem_query.set_limit(limit);
    
    grpc::Status status = stub_->QueryMemory(&context, mem_query, &result);
    
    if (!status.ok()) {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
    
    return result;
}

void AgentClient::StartStreaming(const std::string& agent_id) {
    if (streaming_) {
        std::cout << "Already streaming" << std::endl;
        return;
    }
    
    stream_context_ = std::make_unique<grpc::ClientContext>();
    stream_ = stub_->StreamAgentMessages(stream_context_.get());
    streaming_ = true;
    
    // Start receive thread
    receive_thread_ = std::make_unique<std::thread>(&AgentClient::ReceiveMessages, this);
    
    std::cout << "Started bidirectional streaming for agent: " << agent_id << std::endl;
}

bool AgentClient::SendMessage(const AgentMessage& message) {
    if (!streaming_ || !stream_) {
        std::cerr << "Not streaming" << std::endl;
        return false;
    }
    
    return stream_->Write(message);
}

void AgentClient::SubscribeEvents(const std::string& agent_id,
                                 const std::vector<std::string>& event_types) {
    grpc::ClientContext context;
    EventSubscription subscription;
    
    subscription.set_agent_id(agent_id);
    for (const auto& type : event_types) {
        subscription.add_event_types(type);
    }
    
    std::unique_ptr<grpc::ClientReader<Event>> reader(
        stub_->SubscribeEvents(&context, subscription));
    
    std::cout << "Subscribed to events for agent: " << agent_id << std::endl;
    
    Event event;
    while (reader->Read(&event)) {
        std::cout << "Event received: " << event.event_type() 
                  << " from " << event.source_agent_id() << std::endl;
    }
    
    grpc::Status status = reader->Finish();
    if (!status.ok()) {
        std::cerr << "Event subscription ended: " << status.error_message() << std::endl;
    }
}

void AgentClient::StopStreaming() {
    if (!streaming_) {
        return;
    }
    
    streaming_ = false;
    
    if (stream_) {
        stream_->WritesDone();
    }
    
    if (receive_thread_ && receive_thread_->joinable()) {
        receive_thread_->join();
    }
    
    if (stream_) {
        grpc::Status status = stream_->Finish();
        if (!status.ok()) {
            std::cerr << "Stream ended with error: " << status.error_message() << std::endl;
        }
    }
    
    std::cout << "Stopped streaming" << std::endl;
}

void AgentClient::ReceiveMessages() {
    AgentMessage message;
    while (streaming_ && stream_->Read(&message)) {
        std::cout << "\n=== Received message from server ===" << std::endl;
        std::cout << "Agent: " << message.agent_id() << std::endl;
        std::cout << "Type: " << message.type() << std::endl;
        
        switch (message.type()) {
            case MessageType::TOOL_RESULT:
                if (message.has_tool_result()) {
                    const auto& result = message.tool_result();
                    std::cout << "Tool Result: " << std::endl;
                    std::cout << "  Success: " << (result.success() ? "yes" : "no") << std::endl;
                    std::cout << "  Result: " << result.result() << std::endl;
                    std::cout << "  Execution time: " << result.execution_time_ms() << "ms" << std::endl;
                }
                break;
            
            case MessageType::MEMORY_RESULT:
                if (message.has_memory_result()) {
                    const auto& result = message.memory_result();
                    std::cout << "Memory Result: " << std::endl;
                    std::cout << "  Total count: " << result.total_count() << std::endl;
                    std::cout << "  Entries: " << result.entries_size() << std::endl;
                    for (const auto& entry : result.entries()) {
                        std::cout << "    " << entry.key() << " = " << entry.value() << std::endl;
                    }
                }
                break;
            
            case MessageType::TEXT:
                std::cout << "Text: " << message.text_message() << std::endl;
                break;
            
            default:
                std::cout << "Unknown message type" << std::endl;
                break;
        }
        std::cout << "====================================\n" << std::endl;
    }
}

} // namespace gmcp
