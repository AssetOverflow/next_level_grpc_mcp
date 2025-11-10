#!/usr/bin/env python
from __future__ import annotations

import logging
import signal
from src.next_level_bus.grpc.server import NeuralBusServer

logging.basicConfig(level=logging.INFO)

def main():
    server = NeuralBusServer()
    server.start()
    def shutdown(*_):
        server.stop()
    signal.signal(signal.SIGINT, shutdown)
    signal.signal(signal.SIGTERM, shutdown)
    server.wait_for_termination()

if __name__ == "__main__":
    main()
