if abspath(PROGRAM_FILE) == @__FILE__
    using Pkg
    Pkg.activate(@__DIR__)
    Pkg.instantiate()
end

using Finch
using MatrixDepot
using TensorMarket
using SparseArrays
using NPZ
using HDF5

generator = Dict()

"""
    generate(problem, name, dst, ext, args...)

Generate a problem of type `problem` with generator named `name` and save it to
the path `dst` with extension `ext`.
"""
function generate(problem, name, dst, ext, args...)
    generator[problem][name](dst, ext, args...)
end

function spmv(dst, ext, mtx)
    A = copyto!(Fiber!(Dense(SparseList(Element(0.0)))), matrixdepot(mtx))
    m, n = size(A)
    x = copyto!(Fiber!(Dense(Element(0.0))), rand(n))
    y = copyto!(Fiber!(Dense(Element(0.0))), A * x)
    Finch.fwrite(joinpath(dst, "y_ref.$ext"), y)
    Finch.fwrite(joinpath(dst, "A.$ext"), A)
    Finch.fwrite(joinpath(dst, "x.$ext"), x)
end 

generator["spmv"] = Dict()
generator["spmv"]["matrixmarket"] = spmv

function main(args...)
    generate(args...)
end

if abspath(PROGRAM_FILE) == @__FILE__
    main(ARGS...)
end