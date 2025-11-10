# gMCP Quick Reference

## Build & Run

```bash
# Build the project
./build.sh

# Run server
./build/gmcp_server

# Run client (in another terminal)
./build/gmcp_client
```

## Docker

```bash
# Build and run with Docker Compose
docker-compose up --build

# Or manually
docker build -t gmcp .
docker run -p 50051:50051 gmcp
```

## Protocol Overview

### Service RPCs

| RPC | Type | Description |
|-----|------|-------------|
| `StreamAgentMessages` | Bidirectional Streaming | Real-time agent communication |
| `RegisterTool` | Unary | Register a new tool plugin |
| `RegisterMemory` | Unary | Register a memory store |
| `InvokeTool` | Unary | Execute a tool |
| `QueryMemory` | Unary | Query memory store |
| `SubscribeEvents` | Server Streaming | Subscribe to events |

### Message Types

| Message | Purpose |
|---------|---------|
| `AgentMessage` | Unified streaming message with typed payloads |
| `ToolRegistration` | Define a new tool with parameters |
| `ToolInvocation` | Request tool execution |
| `ToolResult` | Tool execution result |
| `MemoryRegistration` | Define a memory store |
| `MemoryQuery` | Search/retrieve from memory |
| `MemoryResult` | Query results with entries |
| `Event` | Event notification |

### MessageType Enum

```proto
UNKNOWN = 0;
TOOL_INVOCATION = 1;
TOOL_RESULT = 2;
MEMORY_QUERY = 3;
MEMORY_RESULT = 4;
EVENT = 5;
TEXT = 6;
```

## C++ API Examples

### Server Side

```cpp
// Register a tool
ToolRegistration tool;
tool.set_tool_id("my_tool");
tool.set_name("My Tool");

auto func = [](const std::map<std::string, std::string>& args) -> std::string {
    return "result";
};

tool_manager_->RegisterTool(tool, func);

// Register memory
MemoryRegistration mem;
mem.set_memory_id("my_store");
mem.set_type(MemoryType::KEY_VALUE);
memory_manager_->RegisterMemory(mem);
```

### Client Side

```cpp
// Create client
auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
AgentClient client(channel);

// Invoke tool
std::map<std::string, std::string> args = {{"key", "value"}};
ToolResult result = client.InvokeTool("tool_id", args);

// Stream messages
client.StartStreaming("agent_id");
AgentMessage msg;
msg.set_type(MessageType::TEXT);
msg.set_text_message("Hello");
client.SendMessage(msg);
client.StopStreaming();
```

## Python Client

```python
# Generate Python code
python -m grpc_tools.protoc -I./proto --python_out=. --grpc_python_out=. ./proto/gmcp.proto

# Use client
from gmcp_pb2_grpc import AgentCoordinationStub
client = GMCPPythonClient('localhost:50051')
result = client.invoke_tool("calculator", {"operation": "add", "a": "5", "b": "3"})
```

## Code Generation

```bash
# Generate for specific language
./generate_code.sh python

# Generate for all languages
./generate_code.sh all
```

## Default Examples

### Calculator Tool
- **ID**: `calculator`
- **Operations**: `add`, `subtract`, `multiply`, `divide`
- **Parameters**: `operation`, `a`, `b`

```bash
# Example usage
operation=add a=10 b=5 → 15.0
```

### Default Memory Store
- **ID**: `default_store`
- **Type**: `KEY_VALUE`
- **Initial Data**: `example_key=example_value`

## Port

Default port: **50051**

Change in code or via environment variable:
```cpp
std::string server_address = std::getenv("GMCP_PORT") ?: "0.0.0.0:50051";
```

## Performance Tips

1. **Use streaming** for high-frequency communication
2. **Batch tool invocations** when possible
3. **Limit memory query results** with appropriate limits
4. **Use connection pooling** in clients
5. **Enable compression** for large payloads:
   ```cpp
   builder.SetDefaultCompressionAlgorithm(GRPC_COMPRESS_GZIP);
   ```

## Troubleshooting

### Build fails
- Ensure gRPC and protobuf are installed
- Check C++ compiler supports C++20
- Verify CMake version >= 3.15

### Connection refused
- Check server is running: `./build/gmcp_server`
- Verify port 50051 is not blocked
- Check firewall settings

### Proto compilation errors
- Update protoc and grpc_cpp_plugin
- Check proto syntax with: `protoc --lint proto/gmcp.proto`

## Resources

- **Full Documentation**: README.md
- **Contributing**: CONTRIBUTING.md
- **Examples**: examples/
- **Protocol Definition**: proto/gmcp.proto
