function spmm_erdos_renyi_command(args; kwargs...)
    doc = """Generate spmm problem instances with a uniform random graph
        y[i][k] += A[i, j] * X[j, k]

    Usage:
        generate.jl spmm erdos_renyi [options]
        generate.jl spmm erdos_renyi --help

    Options:
        -o, --out <path>    Output file path [default: ../data]
        -e, --ext <ext>     Output file extensions [default: bspnpy]
        -d, --dense <int>   Dense dimension [default: 8]
        -h, --help          show this screen
    """ 
    parsed_args = docopt(doc, args)
    spmm_erdos_renyi(
        out=parsed_args["--out"],
        ext=parsed_args["--ext"],
        d=parse(Int, parsed_args["--dense"])
    )
end

function spmm_erdos_renyi(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", d = 8)
    m = floor(Int, 10^(2+rand()*2))
    n = floor(Int, 10^(2+rand()*2))

    A = sparse(fsprand(Float64, m, n, 10^4))
    X = rand(n, d)
    Y_ref = A * X
    mkpath(out)
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), A))
    Finch.fwrite(joinpath(out, "X.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), X))
    Finch.fwrite(joinpath(out, "Y_ref.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), Y_ref))

end


spmm_commands = Dict(
    "erdos_renyi" => spmm_erdos_renyi_command
)
function spmm_command(args)
    doc = """Generate spmm problem instances.
        y[i] += A[i, j] * x[j]

    <dataset> is one of the following:
        erdos_renyi
            uniform random graph

    see generate.jl spmm <dataset> --help for more information on each dataset

    Usage:
        generate.jl spmm <dataset> ...
        generate.jl spmm <dataset> --help
        generate.jl spmm --help

    Options:
        -h --help     Show this screen.
    """ 
    if length(args) >= 2 && haskey(spmm_commands, args[2])
        return spmm_commands[args[2]](args)
    elseif length(args) == 2 && args[2] == "--help"
        println(doc)
    else
        println(doc)
        error()
    end
end
