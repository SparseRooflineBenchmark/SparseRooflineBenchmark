if abspath(PROGRAM_FILE) == @__FILE__
  using Pkg
  Pkg.activate(@__DIR__, io = devnull)
  Pkg.instantiate()
end

using DocOpt
using Suppressor
@suppress using MatrixDepot
using Finch
using TensorMarket
using SparseArrays
using NPZ
using HDF5
using Random
using Base.Iterators
using StatsBase

include("util.jl")
include("spmv.jl")
include("spmm.jl")

kernel_commands = Dict(
    "spmv" => spmv_command,
    "spmm" => spmm_command,
)

function main(args)
    doc = """Generate sparse problem instances.

    <kernel> is one of the following:
        spmv
            y[i] += A[i, j] * x[j]
        spmm
            Y[i, d] += A[i, j] * X[j, d]

    see generate.jl <kernel> --help for more information on each kernel

    Usage:
        generator.jl <kernel> <dataset> ...
        generator.jl <kernel> <dataset> --help
        generator.jl <kernel> --help
        generator.jl --help

    Options:
        -h --help     Show this screen.
    """
    if length(args) >= 1 && haskey(kernel_commands, args[1])
        return kernel_commands[args[1]](args)
    elseif length(args) == 1 && args[1] == "--help"
        println(doc)
    else
        println(doc)
        error()
    end
end

main(ARGS)