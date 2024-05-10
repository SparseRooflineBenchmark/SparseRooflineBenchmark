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

kernel_commands = Dict(
    "spmv" => spmv_command,
#    "spmm" => spmm_command,
)

doc = """Generate sparse problem instances.

kernel is one of the following:
spmv    y[i] += A[i, j] * x[j]
spmm    Y[i, d] += A[i, j] * X[j, d]
see generate.jl <kernel> --help for more information on each

Usage:
  generator.jl [options] <kernel> <dataset> [<args>...]
  generator.jl [options] <kernel> <dataset> [<args>...]
  generator.jl [--help] [--version]
  generator.jl <kernel> [--help] [--version]

Options:
  -h --help     Show this screen.
  --version     Show version.
  --out <path>  Output file path [default: ./]
  --ext <ext>   Output file extensions [default: .ttx]

"""
function main(args)
    parsed_args = docopt(doc, args; version=v"2.0.0", options_first=true)

    println(parsed_args)
    sub_args = parsed_args["<args>"]
    if parsed_args["<kernel>"] === nothing
        println("Please specify a kernel")
        exit()
    end
    if parsed_args["<dataset>"] !== nothing
        pushfirst!(sub_args, parsed_args["<dataset>"])
    end
    pushfirst!(sub_args, parsed_args["<kernel>"])

    kernel_commands[parsed_args["<kernel>"]](args, out=parsed_args["--out"], ext=parsed_args["--ext"])
end

main(ARGS)