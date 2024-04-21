#!/bin/bash
set -e
set -x  # Debug mode

function fail {
    local msg="${1:-Command Failed}"  # Sets a default message if none is provided
    echo "Test Failure: $msg"
    exit 1
}

julia src/generate.jl spmv rmat || fail()

exit 0

