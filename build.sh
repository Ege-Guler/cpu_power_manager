#!/bin/bash

BUILD_DIR="build"
LOG_FILE="build.log"

cmake --build "$BUILD_DIR" 2>&1 | tee "$LOG_FILE"
