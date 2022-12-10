#!/bin/bash

# Enforce software rendering (llvmpipe)
# WSL2 will attempt to use your GPU by default that will need additional setup
export LIBGL_ALWAYS_SOFTWARE=true

# Run all tests in the test directory
pytest tests/
