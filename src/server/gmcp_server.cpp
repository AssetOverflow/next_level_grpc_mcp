#include "gmcp_server.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace gmcp {

AgentCoordinationServiceImpl::AgentCoordinationServiceImpl() 
    : tool_manager_(std::make_unique<ToolManager>()),
      memory_manager_(std::make_unique<MemoryManager>()) {
}

AgentCoordinationServiceImpl::~AgentCoordinationServiceImpl() = default;

grpc::Status AgentCoordinationServiceImpl::StreamAgentMessages(
    grpc::ServerContext* context,
    grpc::ServerReaderWriter<AgentMessage, AgentMessage>* stream) {
    
    std::cout << "New agent connected for bidirectional streaming" << std::endl;
    
    AgentMessage message;
    while (stream->Read(&message)) {
        std::cout << "Received message from agent: " << message.agent_id() << std::endl;
        
        // Process message based on type
        AgentMessage response;
        response.set_agent_id("server");
        response.set_timestamp(
            std::chrono::system_clock::now().time_since_epoch().count());
        
        switch (message.type()) {
            case MessageType::TOOL_INVOCATION: {
                if (message.has_tool_invocation()) {
                    ToolResult result = tool_manager_->InvokeTool(message.tool_invocation());
                    response.set_type(MessageType::TOOL_RESULT);
                    *response.mutable_tool_result() = result;
                    stream->Write(response);
                }
                break;
            }
            
            case MessageType::MEMORY_QUERY: {
                if (message.has_memory_query()) {
                    MemoryResult result = memory_manager_->Query(message.memory_query());
                    response.set_type(MessageType::MEMORY_RESULT);
                    *response.mutable_memory_result() = result;
                    stream->Write(response);
                }
                break;
            }
            
            case MessageType::TEXT: {
                // Echo back text messages
                response.set_type(MessageType::TEXT);
                response.set_text_message("Echo: " + message.text_message());
                stream->Write(response);
                break;
            }
            
            default:
                std::cout << "Unknown message type" << std::endl;
                break;
        }
        
        // Publish event for this message
        Event event;
        event.set_event_id("evt_" + std::to_string(message.timestamp()));
        event.set_event_type("message_received");
        event.set_source_agent_id(message.agent_id());
        event.set_timestamp(
            std::chrono::system_clock::now().time_since_epoch().count());
        PublishEvent(event);
    }
    
    std::cout << "Agent disconnected" << std::endl;
    return grpc::Status::OK;
}

grpc::Status AgentCoordinationServiceImpl::RegisterTool(
    grpc::ServerContext* context,
    const ToolRegistration* request,
    RegistrationResponse* response) {
    
    // Create a simple example tool function
    auto tool_func = [](const std::map<std::string, std::string>& args) -> std::string {
        std::string result = "Tool executed with args: ";
        for (const auto& [key, value] : args) {
            result += key + "=" + value + ", ";
        }
        return result;
    };
    
    bool success = tool_manager_->RegisterTool(*request, tool_func);
    
    response->set_success(success);
    if (success) {
        response->set_message("Tool registered successfully");
        response->set_registration_id(request->tool_id());
        std::cout << "Registered tool: " << request->name() << std::endl;
    } else {
        response->set_message("Tool already registered or registration failed");
    }
    
    return grpc::Status::OK;
}

grpc::Status AgentCoordinationServiceImpl::RegisterMemory(
    grpc::ServerContext* context,
    const MemoryRegistration* request,
    RegistrationResponse* response) {
    
    bool success = memory_manager_->RegisterMemory(*request);
    
    response->set_success(success);
    if (success) {
        response->set_message("Memory store registered successfully");
        response->set_registration_id(request->memory_id());
        std::cout << "Registered memory store: " << request->name() << std::endl;
    } else {
        response->set_message("Memory store already registered or registration failed");
    }
    
    return grpc::Status::OK;
}

grpc::Status AgentCoordinationServiceImpl::InvokeTool(
    grpc::ServerContext* context,
    const ToolInvocation* request,
    ToolResult* response) {
    
    *response = tool_manager_->InvokeTool(*request);
    return grpc::Status::OK;
}

grpc::Status AgentCoordinationServiceImpl::QueryMemory(
    grpc::ServerContext* context,
    const MemoryQuery* request,
    MemoryResult* response) {
    
    *response = memory_manager_->Query(*request);
    return grpc::Status::OK;
}

grpc::Status AgentCoordinationServiceImpl::SubscribeEvents(
    grpc::ServerContext* context,
    const EventSubscription* request,
    grpc::ServerWriter<Event>* writer) {
    
    std::cout << "Agent " << request->agent_id() << " subscribed to events" << std::endl;
    
    // Stream events from the queue
    while (!context->IsCancelled()) {
        std::unique_lock<std::mutex> lock(event_mutex_);
        event_cv_.wait_for(lock, std::chrono::seconds(1), [this] {
            return !event_queue_.empty();
        });
        
        while (!event_queue_.empty()) {
            Event event = event_queue_.front();
            event_queue_.pop();
            
            // Filter by event types if specified
            if (request->event_types_size() > 0) {
                bool matches = false;
                for (const auto& type : request->event_types()) {
                    if (event.event_type() == type) {
                        matches = true;
                        break;
                    }
                }
                if (!matches) continue;
            }
            
            lock.unlock();
            if (!writer->Write(event)) {
                return grpc::Status::CANCELLED;
            }
            lock.lock();
        }
    }
    
    return grpc::Status::OK;
}

void AgentCoordinationServiceImpl::PublishEvent(const Event& event) {
    std::lock_guard<std::mutex> lock(event_mutex_);
    event_queue_.push(event);
    event_cv_.notify_all();
}

void AgentCoordinationServiceImpl::InitializeExamples() {
    // Register example tools
    ToolRegistration calc_tool;
    calc_tool.set_tool_id("calculator");
    calc_tool.set_name("Calculator");
    calc_tool.set_description("Performs basic arithmetic operations");
    calc_tool.set_return_type("string");
    
    auto param1 = calc_tool.add_parameters();
    param1->set_name("operation");
    param1->set_type("string");
    param1->set_required(true);
    param1->set_description("Operation to perform: add, subtract, multiply, divide");
    
    auto param2 = calc_tool.add_parameters();
    param2->set_name("a");
    param2->set_type("number");
    param2->set_required(true);
    param2->set_description("First operand");
    
    auto param3 = calc_tool.add_parameters();
    param3->set_name("b");
    param3->set_type("number");
    param3->set_required(true);
    param3->set_description("Second operand");
    
    auto calc_func = [](const std::map<std::string, std::string>& args) -> std::string {
        try {
            double a = std::stod(args.at("a"));
            double b = std::stod(args.at("b"));
            std::string op = args.at("operation");
            
            double result = 0.0;
            if (op == "add") result = a + b;
            else if (op == "subtract") result = a - b;
            else if (op == "multiply") result = a * b;
            else if (op == "divide") result = b != 0 ? a / b : 0;
            
            return std::to_string(result);
        } catch (...) {
            return "Error: Invalid arguments";
        }
    };
    
    tool_manager_->RegisterTool(calc_tool, calc_func);
    std::cout << "Initialized example calculator tool" << std::endl;
    
    // Register example memory store
    MemoryRegistration mem_store;
    mem_store.set_memory_id("default_store");
    mem_store.set_name("Default Key-Value Store");
    mem_store.set_description("Default in-memory key-value storage");
    mem_store.set_type(MemoryType::KEY_VALUE);
    
    memory_manager_->RegisterMemory(mem_store);
    
    // Add some example entries
    MemoryEntry entry1;
    entry1.set_key("example_key");
    entry1.set_value("example_value");
    entry1.set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());
    memory_manager_->Store("default_store", entry1);
    
    std::cout << "Initialized example memory store" << std::endl;
}

} // namespace gmcp
