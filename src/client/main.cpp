#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "gmcp_client.h"

void PrintMenu() {
    std::cout << "\n=== gMCP Agent Client ===" << std::endl;
    std::cout << "1. Register Tool" << std::endl;
    std::cout << "2. Register Memory Store" << std::endl;
    std::cout << "3. Invoke Tool (Calculator)" << std::endl;
    std::cout << "4. Query Memory" << std::endl;
    std::cout << "5. Send Text Message (Streaming)" << std::endl;
    std::cout << "6. Subscribe to Events (Blocking)" << std::endl;
    std::cout << "7. Run Demo Sequence" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter choice: ";
}

void RunDemo(gmcp::AgentClient& client) {
    std::cout << "\n=== Running Demo Sequence ===" << std::endl;
    
    // Start bidirectional streaming
    std::cout << "\n1. Starting bidirectional streaming..." << std::endl;
    client.StartStreaming("demo_agent");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Send a text message
    std::cout << "\n2. Sending text message..." << std::endl;
    gmcp::AgentMessage text_msg;
    text_msg.set_agent_id("demo_agent");
    text_msg.set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());
    text_msg.set_type(gmcp::MessageType::TEXT);
    text_msg.set_text_message("Hello from gMCP client!");
    client.SendMessage(text_msg);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Invoke calculator tool through streaming
    std::cout << "\n3. Invoking calculator tool (10 + 5)..." << std::endl;
    gmcp::AgentMessage tool_msg;
    tool_msg.set_agent_id("demo_agent");
    tool_msg.set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());
    tool_msg.set_type(gmcp::MessageType::TOOL_INVOCATION);
    
    auto* invocation = tool_msg.mutable_tool_invocation();
    invocation->set_tool_id("calculator");
    invocation->set_request_id("demo_req_1");
    (*invocation->mutable_arguments())["operation"] = "add";
    (*invocation->mutable_arguments())["a"] = "10";
    (*invocation->mutable_arguments())["b"] = "5";
    
    client.SendMessage(tool_msg);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Query memory through streaming
    std::cout << "\n4. Querying memory store..." << std::endl;
    gmcp::AgentMessage mem_msg;
    mem_msg.set_agent_id("demo_agent");
    mem_msg.set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());
    mem_msg.set_type(gmcp::MessageType::MEMORY_QUERY);
    
    auto* query = mem_msg.mutable_memory_query();
    query->set_memory_id("default_store");
    query->set_query_type(gmcp::QueryType::LIST);
    query->set_limit(10);
    
    client.SendMessage(mem_msg);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Stop streaming
    std::cout << "\n5. Stopping streaming..." << std::endl;
    client.StopStreaming();
    
    std::cout << "\nDemo completed!" << std::endl;
}

int main(int argc, char** argv) {
    std::string server_address = "localhost:50051";
    
    if (argc > 1) {
        server_address = argv[1];
    }
    
    std::cout << "Connecting to gMCP server at " << server_address << std::endl;
    
    // Create a channel to the server
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    gmcp::AgentClient client(channel);
    
    bool running = true;
    while (running) {
        PrintMenu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline
        
        switch (choice) {
            case 1: {
                std::cout << "Registering example tool..." << std::endl;
                gmcp::ToolRegistration tool;
                tool.set_tool_id("example_tool");
                tool.set_name("Example Tool");
                tool.set_description("An example tool for testing");
                tool.set_return_type("string");
                
                auto param = tool.add_parameters();
                param->set_name("input");
                param->set_type("string");
                param->set_required(true);
                
                client.RegisterTool(tool);
                break;
            }
            
            case 2: {
                std::cout << "Registering example memory store..." << std::endl;
                gmcp::MemoryRegistration memory;
                memory.set_memory_id("test_store");
                memory.set_name("Test Store");
                memory.set_description("A test memory store");
                memory.set_type(gmcp::MemoryType::KEY_VALUE);
                
                client.RegisterMemory(memory);
                break;
            }
            
            case 3: {
                std::cout << "Enter operation (add/subtract/multiply/divide): ";
                std::string op;
                std::getline(std::cin, op);
                
                std::cout << "Enter first number: ";
                std::string a;
                std::getline(std::cin, a);
                
                std::cout << "Enter second number: ";
                std::string b;
                std::getline(std::cin, b);
                
                std::map<std::string, std::string> args;
                args["operation"] = op;
                args["a"] = a;
                args["b"] = b;
                
                auto result = client.InvokeTool("calculator", args);
                std::cout << "\nTool Result:" << std::endl;
                std::cout << "  Success: " << (result.success() ? "yes" : "no") << std::endl;
                std::cout << "  Result: " << result.result() << std::endl;
                std::cout << "  Execution time: " << result.execution_time_ms() << "ms" << std::endl;
                break;
            }
            
            case 4: {
                std::cout << "Querying default memory store..." << std::endl;
                auto result = client.QueryMemory("default_store", gmcp::QueryType::LIST, "", 10);
                
                std::cout << "\nMemory Query Result:" << std::endl;
                std::cout << "  Total count: " << result.total_count() << std::endl;
                std::cout << "  Entries returned: " << result.entries_size() << std::endl;
                for (const auto& entry : result.entries()) {
                    std::cout << "    " << entry.key() << " = " << entry.value() << std::endl;
                }
                break;
            }
            
            case 5: {
                client.StartStreaming("interactive_agent");
                
                std::cout << "Enter message (or 'quit' to stop): ";
                std::string msg;
                std::getline(std::cin, msg);
                
                while (msg != "quit") {
                    gmcp::AgentMessage message;
                    message.set_agent_id("interactive_agent");
                    message.set_timestamp(
                        std::chrono::system_clock::now().time_since_epoch().count());
                    message.set_type(gmcp::MessageType::TEXT);
                    message.set_text_message(msg);
                    
                    client.SendMessage(message);
                    
                    std::cout << "Enter message (or 'quit' to stop): ";
                    std::getline(std::cin, msg);
                }
                
                client.StopStreaming();
                break;
            }
            
            case 6: {
                std::cout << "Subscribing to events (Press Ctrl+C to stop)..." << std::endl;
                std::vector<std::string> event_types = {"message_received"};
                client.SubscribeEvents("event_listener", event_types);
                break;
            }
            
            case 7: {
                RunDemo(client);
                break;
            }
            
            case 0: {
                running = false;
                break;
            }
            
            default:
                std::cout << "Invalid choice!" << std::endl;
                break;
        }
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}
