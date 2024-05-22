function spmm_command(args; kwargs...)
    doc = """Generate spmm problem instances of the form
        y[i][k] += A[i, j] * X[j, k]

    Usage:
        generate.jl spmm [options]
        generate.jl spmv --help

    Options:
        -o, --out <path>    Output file path [default: ../data]
        -e, --ext <ext>     Output file extensions [default: bspnpy]
        -d, --dense <d>     Dense dimension
        -h, --help          show this screen
    """ 
    parsed_args = docopt(doc, args)
    spmm(
        out=parsed_args["--out"],
        ext=parsed_args["--ext"],
        d=parsed_args["--dense"]
    )
end

function spmm(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", d = 8)
    m = floor(Int, 10^(4+rand()*2))
    n = floor(Int, 10^(4+rand()*2))

    A = sparse(fsprand(Float64, m, n, 10^6))
    X = rand(n, d)
    Y_ref = A * X
    mkpath(out)
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), A))
    Finch.fwrite(joinpath(out, "X.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), X))
    Finch.fwrite(joinpath(out, "Y_ref.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), Y_ref))

end