function error {
    echo "Error: $1"
    exit 1
}

julia src/generate.jl spmv rmat || error

exit 0

