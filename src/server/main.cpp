#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "gmcp_server.h"

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    gmcp::AgentCoordinationServiceImpl service;
    
    // Initialize example tools and memory stores
    service.InitializeExamples();
    
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    
    grpc::ServerBuilder builder;
    
    // Listen on the given address without any authentication mechanism
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // Register "service" as the instance through which we'll communicate
    builder.RegisterService(&service);
    
    // Assemble the server
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "gMCP Server listening on " << server_address << std::endl;
    std::cout << "Ultra-low-latency, bidirectional agent coordination ready!" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Bidirectional streaming for real-time communication" << std::endl;
    std::cout << "  - Dynamic tool registration and invocation" << std::endl;
    std::cout << "  - Memory plugin system" << std::endl;
    std::cout << "  - Event subscription and streaming" << std::endl;
    std::cout << "\nPress Ctrl+C to shutdown" << std::endl;
    
    // Wait for the server to shutdown
    server->Wait();
}

int main(int argc, char** argv) {
    try {
        RunServer();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
