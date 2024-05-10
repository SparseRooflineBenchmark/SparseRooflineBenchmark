function spmv_suitesparse_command(args; kwargs...)
    doc = """Generate spmv problem instances from SuiteSparse.
        y[i] += A[i, j] * x[j]

    Usage:
        generate.jl spmv suitesparse [options] <key>
        generate.jl spmv suitesparse --help

    Options:
        -o, --out <path>    Output file path [default: ../data]
        -e, --ext <ext>     Output file extensions [default: bspnpy]
        <key>               matrix key
        -h, --help          show this screen
    """ 
    parsed_args = docopt(doc, args)
    if parsed_args["<key>"] === nothing
        println(doc)
        exit()
    end
    spmv_suitesparse(
        parsed_args["<key>"];
        out=parsed_args["--out"],
        ext=parsed_args["--ext"]
    )
end

function spmv_suitesparse(key; out = joinpath(@__DIR__, "../data"), ext="bspnpy")
    A = SparseMatrixCSC(matrixdepot(key))
    m, n = size(A)
    x = rand(n)
    y = A * x
    mkpath(out)
    Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(Tensor(Dense(SparseList(Element(0.0)))), 2, 1), A))
    Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
end

function spmv_RMAT_command(args; kwargs...)
    doc = """Generate spmv problem instances from RMAT.
        y[i] += A[i, j] * x[j]

        Usage:
            generate.jl spmv RMAT [options]
            generate.jl spmv RMAT --help

        Options:
            -o, --out=</data>    Destination directory for the generated problem instances [default: ../data]
            -e, --ext <extension>    Generated tensor file format extension [default: bspnpy]
            -A, --A_factor <value>    Factor A in seed matrix [default: 0.57]
            -B, --B_factor <value>    Factor B in seed matrix [default: 0.19]
            -C, --C_factor <value>    Factor C in seed matrix [default: 0.19]
            -N, --N <value>    Use 2^N as the matrix size [default: 10]
            -p, --p <value>    Density (1 - sparsity) of the generated matrix [default: 0.001]
            -s, --seed <value>    Random seed [default: $(rand(UInt))]
            -h, --help          show this screen
    """ 
    parsed_args = docopt(doc, args)
    spmv_RMAT(
        out=parsed_args["--out"],
        ext=parsed_args["--ext"],
        A_factor=parse(Float64, parsed_args["--A_factor"]),
        B_factor=parse(Float64, parsed_args["--B_factor"]),
        C_factor=parse(Float64, parsed_args["--C_factor"]),
        N=parse(Int, parsed_args["--N"]),
        p=parse(Float64, parsed_args["--p"]),
        seed=parse(UInt, parsed_args["--seed"])
    )
end

vuduc02_matrices = [
    "Simon/raefsky3", "Simon/olafu", "Boeing/bcsstk35", "Simon/venkat01", "Boeing/crystk02",
    "Boeing/crystk03", "Nasa/nasasrb", "Rothberg/3dtube", "Boeing/ct20stif", "Bai/af23560",
    "Simon/raefsky4", "FIDAP/ex11", "Zitney/rdist1", "Vavasis/av41092", "HB/orani678",
    "Goodwin/rim", "Hamm/memplus", "HB/gemat11", "Mallya/lhr10", "Goodwin/goodwin",
    "Grund/bayer02", "Grund/bayer10", "Brethour/coater2", "Mulvey/finan512", "ATandT/onetone2",
    "Nasa/pwt", "Cote/vibrobox", "Wang/wang4", "HB/lnsp3937", "HB/lns_3937", "HB/sherman5",
    "HB/sherman3", "HB/orsreg_1", "HB/saylr4", "Shyy/shyy161", "Wang/wang3", "HB/mcfe",
    "HB/jpwh_991", "Gupta/gupta1", "LPnetlib/lp_cre_b", "LPnetlib/lp_cre_d", "LPnetlib/lp_fit2p",
    "Qaplib/lp_nug20"
]

function spmv_vuduc02_command(args; kwargs...)
    doc = """Generate spmv problem instances from the paper:

    R. Vuduc, J. W. Demmel, K. A. Yelick, S. Kamil, R. Nishtala, and B. Lee,
    “Performance Optimizations and Bounds for Sparse Matrix-Vector Multiply,” in
    SC ’02: Proceedings of the 2002 ACM/IEEE Conference on Supercomputing, Nov.
    2002, pp. 26–26. doi: 10.1109/SC.2002.10025
    url: https://doi.org/10.1109/SC.2002.10025

    Usage:
        generate.jl spmv vuduc02 [options]
        generate.jl spmv vuduc02 --help

    Options:
        -o, --out=<path>    Output file path [default: ../data]
        -e, --ext=<ext>     Output file extensions [default: bspnpy]
        -h, --help          Show this screen.
    """
    parsed_args = docopt(doc, args)
    vuduc02(
        out=parsed_args["--out"],
        ext=parsed_args["--ext"]
    )
end

function spmv_vuduc02(; out = joinpath(@__DIR__, "../data"), ext="bspnpy")
    for matrix_key in vuduc02_matrices
        spmv_suitesparse(matrix_key; out = joinpath(out, matrix_key), ext=ext)
    end
end

spmv_commands["vuduc02"] = spmv_vuduc02_command

langr_matrices = [
    "Buss/12month1", "Sinclair/3Dspectralwave2", "Schenk_AFE/af_shell10", "SNAP/amazon0312",
    "Bourchtein/atmosmodj", "GHS_psdef/bmw7st_1", "vanHeukelum/cage12", "vanHeukelum/cage15",
    "Botonakis/FEM_3D_thermal2", "Lee/fem_hifreq_circuit", "Freescale/Freescale1", "Freescale/FullChip",
    "LAW/hollywood-2009", "LAW/in-2004", "GHS_psdef/ldoor", "Belcastro/mouse_gene", "Schenk/nlpkkt120",
    "Schenk_ISEI/ohne2", "Mittelmann/rail4284", "Rajat/rajat31", "JGD_Relat/relat9", "Fluorem/RM07R",
    "Rucci/Rucci1", "Mittelmann/spal_004", "Schmid/thermal2", "TSOPF/TSOPF_RS_b2383", "Gleich/wb-edu",
    "Gleich/wikipedia-20061104", "VanVelzen/Zd_Jac2"
]

function spmv_langr_command(args; kwargs...)
    doc = """Generate spmv problem instances from the Langr collection.

    Usage:
        generate.jl spmv langr [options]
        generate.jl spmv langr --help

    Options:
        -o, --out=<path>    Output file path [default: ../data]
        -e, --ext=<ext>     Output file extensions [default: bspnpy]
        -h, --help          Show this screen.
    """
    parsed_args = docopt(doc, args)
    langr(
        out=parsed_args["--out"],
        ext=parsed_args["--ext"]
    )
end

function spmv_langr(; out = joinpath(@__DIR__, "../data"), ext="bspnpy")
    for matrix_key in langr_matrices
        spmv_suitesparse(matrix_key; out = joinpath(out, matrix_key), ext=ext)
    end
end

spmv_commands["langr"] = spmv_langr_command

function spmv_RMAT(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", A_factor=0.57, B_factor=0.19, C_factor=0.19, N=10, p=0.001, seed=rand(UInt))
    D_factor = 1-(A_factor+B_factor+C_factor)
    seed = [A_factor B_factor; C_factor D_factor]
    A = sparse(stockronrand(Float64, Iterators.repeated(seed, N), p)...)
    m, n = size(A)
    x = rand(n)
    y = A * x
    mkpath(out)
    Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
    Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(Tensor(Dense(SparseList(Element(0.0)))), 2, 1), A))
    Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
end

spmv_commands = Dict(
    "suitesparse" => spmv_suitesparse_command,
    "RMAT" => spmv_RMAT_command,
)
function spmv_command(args)
    doc = """Generate spmv problem instances.
        y[i] += A[i, j] * x[j]

    <dataset> is one of the following:
        suitesparse
            matrix from SuiteSparse
        RMAT
            matrix from RMAT generator

    see generate.jl spmv <dataset> --help for more information on each dataset

    Usage:
        generate.jl spmv <dataset> ...
        generate.jl spmv <dataset> --help
        generate.jl spmv --help

    Options:
        -h --help     Show this screen.
    """ 
    if length(args) >= 2 && haskey(spmv_commands, args[2])
        return spmv_commands[args[2]](args)
    elseif length(args) == 2 && args[2] == "--help"
        println(doc)
    else
        println(doc)
        error()
    end
end