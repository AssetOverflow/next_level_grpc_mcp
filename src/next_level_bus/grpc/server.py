from __future__ import annotations

import logging
import grpc
from concurrent import futures

# from . import neural_bus_service_impl, agent_service_impl
# Import generated protos after compilation:
# import protos.neural_bus_pb2_grpc as neural_bus_pb2_grpc
# import protos.agent_service_pb2_grpc as agent_service_pb2_grpc
# import protos.embedding_service_pb2_grpc as embedding_service_pb2_grpc

logger = logging.getLogger(__name__)

class NeuralBusServer:
    def __init__(self, host: str = "0.0.0.0", port: int = 50051):
        self.host = host
        self.port = port
        self.server = grpc.server(futures.ThreadPoolExecutor(max_workers=32))
        # neural_bus_pb2_grpc.add_NeuralBusServiceServicer_to_server(
        #     neural_bus_service_impl.NeuralBusServicer(), self.server)
        # agent_service_pb2_grpc.add_AgentServiceServicer_to_server(
        #     agent_service_impl.AgentServicer(), self.server)
        # embedding_service_pb2_grpc.add_EmbeddingServiceServicer_to_server(
        #     embedding_service_pb2_grpc.EmbeddingServicer(), self.server)
        self.server.add_insecure_port(f"{self.host}:{self.port}")

    def start(self):
        logger.info("Starting NeuralBus gRPC server on %s:%d", self.host, self.port)
        self.server.start()

    def wait_for_termination(self):
        self.server.wait_for_termination()

    def stop(self, grace: int = 5):
        logger.info("Stopping server...")
        self.server.stop(grace)