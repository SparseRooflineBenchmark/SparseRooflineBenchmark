#!/bin/bash
set -e
set -x  # Debug mode

function fail {
    local msg="${1:-Command Failed}"  # Sets a default message if none is provided
    echo "Test Failure: $msg"
    exit 1
}


julia src/Generator/generator.jl spmv RMAT -o data || fail

cd example; make; cd -;

./example/spmv -i data -o data || fail

julia src/Generator/generator.jl spmm -o data || fail

./example/spmm -i data -o data || fail

exit 0

