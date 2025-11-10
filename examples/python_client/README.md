# Python Client Example for gMCP

This directory contains an example Python client implementation for gMCP, demonstrating multi-language support through Protocol Buffers.

## Prerequisites

```bash
pip install grpcio grpcio-tools
```

## Generate Python gRPC Files

First, generate the Python bindings from the Protocol Buffer definition:

```bash
cd examples/python_client
python -m grpc_tools.protoc -I../../proto --python_out=. --grpc_python_out=. ../../proto/gmcp.proto
```

This will create:
- `gmcp_pb2.py` - Protocol Buffer message classes
- `gmcp_pb2_grpc.py` - gRPC service stubs

## Running the Example

Make sure the gMCP server is running:
```bash
# In the root directory
./build/gmcp_server
```

Then run the Python client:
```bash
python gmcp_client.py
```

## Demo Features

The Python client demonstrates:
1. **Tool Invocation**: Calling the calculator tool
2. **Memory Queries**: Querying the default memory store
3. **Tool Registration**: Registering new tools from Python
4. **Bidirectional Streaming**: Real-time communication with the server

## Event Subscription

To run in event subscription mode:
```bash
python gmcp_client.py --events
```

This will continuously listen for events from the server. Press Ctrl+C to stop.

## Code Structure

```python
# Initialize client
client = GMCPPythonClient('localhost:50051')

# Invoke a tool
result = client.invoke_tool("calculator", {
    "operation": "add",
    "a": "10",
    "b": "5"
})

# Query memory
mem_result = client.query_memory(
    "default_store",
    gmcp_pb2.QueryType.LIST,
    "",
    limit=10
)

# Close connection
client.close()
```

## Creating Your Own Python Client

Use this example as a template for your own Python-based gMCP agents and clients. The Protocol Buffer definitions ensure type safety and compatibility with the C++ server.
