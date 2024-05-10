function spmv_suitesparse_command(args; kwargs...)
    doc = """
    Usage: generate.jl spmv suitesparse <key> 

    Options:
    <key>      matrix key
    """ 
    parsed_args = docopt(doc, args)
    spmv_suitesparse(parsed_args["key"]; kwargs...)
end

function spmv_suitesparse(key; out = joinpath(@__DIR__, "../data"), ext="ttx")
    A = SparseMatrixCSC(matrixdepot(key))
    m, n = size(A)
    x = rand(n)
    y = A * x
    Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(Tensor(Dense(SparseList(Element(0.0)))), 2, 1), A))
    Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
end

#=
"""
RMAT

generate an instance of SpMV from the RMAT matrix generator.

# Intro

generate an instance of SpMV from the RMAT matrix generator.
Uses the following seed matrix:
[A B;
    C D]
where D = 1-(A+B+C) = 0.05

# Options

- `-o, --out=</data>`: destination directory for the generated problem instances
- `-e, --ext <extension>`: generated tensor file format extension
- `-A, --A_factor <value>`: factor A in seed matrix
- `-B, --B_factor <value>`: factor B in seed matrix
- `-C, --C_factor <value>`: factor C in seed matrix
- `-N, --N <value>`: use 2^N as the matrix size
- `-p, --p <value>`: density (1 - sparsity) of the generated matrix
- `-s, --seed <value>`: random seed

"""
@cast function RMAT(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", A_factor=0.57, B_factor=0.19, C_factor=0.19, N=10, p=0.001, seed=rand(UInt))
    D_factor = 1-(A_factor+B_factor+C_factor)
    seed = [A_factor B_factor; C_factor D_factor]
    A = sparse(stockronrand(Float64, Iterators.repeated(seed, N), p)...)
    m, n = size(A)
    x = rand(n)
    y = A * x
    Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(Tensor(Dense(SparseList(Element(0.0)))), 2, 1), A))
    Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
end
end
=#

spmv_subcommands = Dict(
    "suitesparse" => spmv_suitesparse_command,
)
function spmv_command(args; kwargs...)
    doc = """spmv

    dataset is one of the following:

    Usage:
        spmv <dataset> <args>...
        spmv --help
        spmv --version

    Options:
    <key>      matrix key
    """ 
    parsed_args = docopt(doc, args)
    spmv_suitesparse(parsed_args["key"]; kwargs...)
end