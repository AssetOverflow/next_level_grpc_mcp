#!/usr/bin/env python3
"""
Example Python client for gMCP
Demonstrates multi-language support through Protocol Buffers
"""

import grpc
import time
import sys
from concurrent import futures

# Note: You need to generate the Python gRPC files first:
# python -m grpc_tools.protoc -I../../proto --python_out=. --grpc_python_out=. ../../proto/gmcp.proto

try:
    import gmcp_pb2
    import gmcp_pb2_grpc
except ImportError:
    print("Error: gRPC Python files not generated yet.")
    print("Run: python -m grpc_tools.protoc -I../../proto --python_out=. --grpc_python_out=. ../../proto/gmcp.proto")
    sys.exit(1)


class GMCPPythonClient:
    def __init__(self, server_address='localhost:50051'):
        """Initialize the gMCP Python client"""
        self.channel = grpc.insecure_channel(server_address)
        self.stub = gmcp_pb2_grpc.AgentCoordinationStub(self.channel)
        
    def register_tool(self, tool_id, name, description):
        """Register a new tool with the server"""
        tool = gmcp_pb2.ToolRegistration(
            tool_id=tool_id,
            name=name,
            description=description,
            return_type="string"
        )
        
        response = self.stub.RegisterTool(tool)
        print(f"Tool registration: {response.message}")
        return response.success
    
    def invoke_tool(self, tool_id, arguments):
        """Invoke a registered tool"""
        invocation = gmcp_pb2.ToolInvocation(
            tool_id=tool_id,
            request_id=f"python_req_{int(time.time() * 1000)}",
            arguments=arguments
        )
        
        result = self.stub.InvokeTool(invocation)
        return result
    
    def register_memory(self, memory_id, name, description, mem_type):
        """Register a memory store"""
        memory = gmcp_pb2.MemoryRegistration(
            memory_id=memory_id,
            name=name,
            description=description,
            type=mem_type
        )
        
        response = self.stub.RegisterMemory(memory)
        print(f"Memory registration: {response.message}")
        return response.success
    
    def query_memory(self, memory_id, query_type, query, limit=10):
        """Query a memory store"""
        mem_query = gmcp_pb2.MemoryQuery(
            memory_id=memory_id,
            query_type=query_type,
            query=query,
            limit=limit
        )
        
        result = self.stub.QueryMemory(mem_query)
        return result
    
    def stream_messages(self, agent_id):
        """Start bidirectional streaming"""
        def message_generator():
            # Send initial message
            msg = gmcp_pb2.AgentMessage(
                agent_id=agent_id,
                timestamp=int(time.time() * 1000000000),
                type=gmcp_pb2.MessageType.TEXT,
                text_message="Hello from Python client!"
            )
            yield msg
            
            # Send tool invocation
            time.sleep(1)
            tool_msg = gmcp_pb2.AgentMessage(
                agent_id=agent_id,
                timestamp=int(time.time() * 1000000000),
                type=gmcp_pb2.MessageType.TOOL_INVOCATION
            )
            
            invocation = tool_msg.tool_invocation
            invocation.tool_id = "calculator"
            invocation.request_id = "python_stream_req"
            invocation.arguments["operation"] = "add"
            invocation.arguments["a"] = "100"
            invocation.arguments["b"] = "42"
            
            yield tool_msg
        
        # Start streaming
        responses = self.stub.StreamAgentMessages(message_generator())
        
        for response in responses:
            print(f"\n=== Received from server ===")
            print(f"Agent: {response.agent_id}")
            print(f"Type: {gmcp_pb2.MessageType.Name(response.type)}")
            
            if response.HasField('text_message'):
                print(f"Text: {response.text_message}")
            elif response.HasField('tool_result'):
                result = response.tool_result
                print(f"Tool Result:")
                print(f"  Success: {result.success}")
                print(f"  Result: {result.result}")
                print(f"  Time: {result.execution_time_ms}ms")
    
    def subscribe_events(self, agent_id, event_types):
        """Subscribe to server events"""
        subscription = gmcp_pb2.EventSubscription(
            agent_id=agent_id,
            event_types=event_types
        )
        
        print(f"Subscribing to events: {event_types}")
        
        try:
            for event in self.stub.SubscribeEvents(subscription):
                print(f"\n=== Event Received ===")
                print(f"Type: {event.event_type}")
                print(f"Source: {event.source_agent_id}")
                print(f"Timestamp: {event.timestamp}")
        except KeyboardInterrupt:
            print("\nUnsubscribed from events")
    
    def close(self):
        """Close the connection"""
        self.channel.close()


def run_demo():
    """Run a demonstration of the Python client"""
    print("=== gMCP Python Client Demo ===\n")
    
    client = GMCPPythonClient()
    
    try:
        # 1. Invoke calculator tool
        print("1. Invoking calculator tool (7 * 8)...")
        result = client.invoke_tool("calculator", {
            "operation": "multiply",
            "a": "7",
            "b": "8"
        })
        print(f"Result: {result.result}")
        print(f"Execution time: {result.execution_time_ms}ms\n")
        
        # 2. Query memory
        print("2. Querying default memory store...")
        mem_result = client.query_memory(
            "default_store",
            gmcp_pb2.QueryType.LIST,
            "",
            limit=5
        )
        print(f"Total entries: {mem_result.total_count}")
        for entry in mem_result.entries:
            print(f"  {entry.key} = {entry.value}")
        print()
        
        # 3. Register a new tool
        print("3. Registering a Python tool...")
        client.register_tool(
            "python_tool",
            "Python Example Tool",
            "A tool registered from Python client"
        )
        print()
        
        # 4. Streaming demo
        print("4. Starting bidirectional streaming demo...")
        client.stream_messages("python_agent")
        
    except grpc.RpcError as e:
        print(f"RPC Error: {e.code()}: {e.details()}")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        client.close()
        print("\nDemo complete!")


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--events':
        # Event subscription mode
        client = GMCPPythonClient()
        try:
            client.subscribe_events("python_event_listener", ["message_received"])
        except KeyboardInterrupt:
            print("\nShutting down...")
        finally:
            client.close()
    else:
        # Regular demo
        run_demo()
