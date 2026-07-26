#!/usr/bin/env bash
SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
BUILD_DIR=build
CMAKE_ARGS=(-S $SCRIPT_DIR -B $SCRIPT_DIR/$BUILD_DIR)

CMAKE_ARGS+=(-G "Unix Makefiles" $(cat $(find -type f -name "*.mk")))

cmake "${CMAKE_ARGS[@]}"
