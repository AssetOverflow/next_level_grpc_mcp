SHELL := /bin/bash

PY := python
PIP := python -m pip
PROTOC := python -m grpc_tools.protoc

PROTO_DIR := protos
OUT_DIR := src

PROTO_FILES := $(shell find $(PROTO_DIR) -name '*.proto')

.PHONY: install dev proto lint format typecheck test run clean

install:
	$(PIP) install --upgrade pip
	$(PIP) install -e .

dev:
	$(PIP) install -e .[dev]
	$(PIP) install grpcio-tools

proto:
	@echo "Generating gRPC Python stubs..."
	$(PROTOC) -I. -I$(PROTO_DIR) \
	  --python_out=$(OUT_DIR) \
	  --grpc_python_out=$(OUT_DIR) \
	  $(PROTO_FILES)
	@# Ensure package in src/protos exists
	@mkdir -p $(OUT_DIR)/protos
	@touch $(OUT_DIR)/protos/__init__.py

lint:
	ruff check .

format:
	ruff format .

typecheck:
	mypy src

test:
	pytest -q

run:
	$(PY) scripts/run_server.py

clean:
	rm -rf build dist .pytest_cache .mypy_cache .ruff_cache
	find src -name '*_pb2.py' -delete
	find src -name '*_pb2_grpc.py' -delete
