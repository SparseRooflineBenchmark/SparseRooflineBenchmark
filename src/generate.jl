if abspath(PROGRAM_FILE) == @__FILE__
    using Pkg
    Pkg.activate(@__DIR__, io = devnull)
    Pkg.instantiate()
end
module Utils
    using Random
    using Base.Iterators
    using StatsBase

    """
    random sparse Stochastic Kronecker tensor
    ========================
    stockronrand([rng], [T], D, p, [rand]) = (I..., V, dims...)

    generate a random tensor according to the probability distribution

    kron(D...) ./ sum(kron(D...))

    *Input options:*
    + [rng]: a random number generator
    + [T]: an element type
    + D: an iterator over probability Arrays
    + p: the number of nonzero values to sample, as an integer or fraction of the total size.
    + [rand]: a random function to generate values

    *Outputs:*
    + I: unsorted output coordinate vectors, with duplicates
    + V: The output values, with duplicates
    + dims: The size of the output tensor

    *Examples*

    The output of this function may be passed to sparse, as:

    ```
    sparse(stockronrand(Iterators.repeated([0.9 0.1; 0.9 0.1], 4), 200)...))
    ```
    """
    stockronrand(D, m) = stockronrand(Float64, D, m, rand)
    stockronrand(D, m, rand) = stockronrand(Float64, D, m, rand)
    stockronrand(T::Type, D, m) = stockronrand(Random.default_rng(), T, D, m, rand)
    stockronrand(T::Type, D, m, rand) = stockronrand(Random.default_rng(), T, D, m, rand)
    stockronrand(rng::AbstractRNG, D, m) = stockronrand(rng, Float64, D, m, rand)
    stockronrand(rng::AbstractRNG, D, m, rand) = stockronrand(rng, Float64, D, m, rand)
    stockronrand(rng::AbstractRNG, T::Type, D, m) = stockronrand(rng, T, D, m, rand)
    stockronrand(rng::AbstractRNG, T::Type, D, m::AbstractFloat, rand) = 
        stockronrand(rng, T, D, ceil(Int, mapreduce(length, *, D) * m), rand)
    function stockronrand(rng::AbstractRNG, T::Type, D, m::Integer, rand::Rand) where {Rand}
        dims = mapreduce(size, .*, D)
        N = length(dims)
        D = map(d -> (d ./ sum(d)), D)
        I = ntuple(_->Int[], N)
        V = rand(rng, T, m)
        for _ = 1:m
            i = ntuple(n->1, N)
            for d in D
                i = (i .- 1) .* size(d) .+ Tuple(sample(CartesianIndices(size(d)), Weights(reshape(d, :))))
            end
            push!.(I, i)
        end
        return (I..., V, dims...)
    end
end

module Generate
     
    using Comonicon

    """
        spmv
    
    y[i] += A[i, j] * x[j]

    # Intro

    Problems of the form y[i] += A[i, j] * x[j]
    """
    module SpMV
        using Comonicon
        using ...Utils: stockronrand
        using Suppressor
        @suppress using MatrixDepot
        using Finch
        using TensorMarket
        using SparseArrays
        using NPZ
        using HDF5
        using Random
        using Base.Iterators
        using JSON

        """
        generate spmv suitesparse
        
        generate an instance of SpMV from the SuiteSparse matrix collection
        
        # Intro

        generate an instance of SpMV from the SuiteSparse matrix collection

        # Args
        
        - `key`: SuiteSparse matrix collection key
        
        # Options
        
        - `-o, --out=</data>`: destination directory for the generated problem instances
        - `-e, --ext <extension>`: generated tensor file format extension
        
        """
        @cast function suitesparse(key; out = joinpath(@__DIR__, "../data"), ext="bspnpy", seed = rand(UInt))
            Random.seed!(seed)
            A = SparseMatrixCSC(matrixdepot(key))
            m, n = size(A)
            x = rand(n)
            y = A * x
            Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
            Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(SparseList(Element(0.0)))), (2, 1)), 2, 1), A))
            Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
            open(joinpath(out, "seed.json"), "w") do f
                JSON.print(f, Dict("seed"=>seed))
            end
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
        """
        generate spmv vuduc02
        
        generate an instance of SpMV from the SuiteSparse matrix collection
        
        # Intro

        generate an instance of SpMV from the SuiteSparse matrix collection
        
        # Options
        
        - `-o, --out=</data>`: destination directory for the generated problem instances
        - `-e, --ext <extension>`: generated tensor file format extension
        
        """
        @cast function vuduc02(out = joinpath(@__DIR__, "../data"), ext="bspnpy")
            for matrix_key in vuduc02_matrices
                suitesparse(matrix_key; out = joinpath(out, matrix_key), ext=ext)
            end

        end

        langr_matrices = [
            "Buss/12month1", "Sinclair/3Dspectralwave2", "Schenk_AFE/af_shell10", "SNAP/amazon0312",
            "Bourchtein/atmosmodj", "GHS_psdef/bmw7st_1", "vanHeukelum/cage12", "vanHeukelum/cage15",
            "Botonakis/FEM_3D_thermal2", "Lee/fem_hifreq_circuit", "Freescale/Freescale1", "Freescale/FullChip",
            "LAW/hollywood-2009", "LAW/in-2004", "GHS_psdef/ldoor", "Belcastro/mouse_gene", "Schenk/nlpkkt120",
            "Schenk_ISEI/ohne2", "Mittelmann/rail4284", "Rajat/rajat31", "JGD_Relat/relat9", "Fluorem/RM07R",
            "Rucci/Rucci1", "Mittelmann/spal_004", "Schmid/thermal2", "TSOPF/TSOPF_RS_b2383", "Gleich/wb-edu",
            "Gleich/wikipedia-20061104", "VanVelzen/Zd_Jac2"
        ]
        """
        generate spmv langr
        
        generate an instance of SpMV from the SuiteSparse matrix collection
        
        # Intro

        generate an instance of SpMV from the SuiteSparse matrix collection
        
        # Options
        
        - `-o, --out=</data>`: destination directory for the generated problem instances
        - `-e, --ext <extension>`: generated tensor file format extension
        
        """
        @cast function langr(out = joinpath(@__DIR__, "../data"), ext="bspnpy")
            for matrix_key in langr_matrices
                suitesparse(matrix_key; out = joinpath(out, matrix_key), ext=ext)
            end

        end

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
        - `-S, --sample_size <value>`: number of sample problems to generate
        
        """
        @cast function RMAT(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", A_factor=0.57, B_factor=0.19, C_factor=0.19, N=10, p=0.001, seed=rand(UInt), sample_size=100)
            Random.seed!(seed)
            mkpath(out)
            for i = 1:sample_size
                instance_out = joinpath(out, "instance_$i")
                mkpath(instance_out)
                D_factor = 1-(A_factor+B_factor+C_factor)
                abcd = [A_factor B_factor; C_factor D_factor]
                A = sparse(stockronrand(Float64, Iterators.repeated(abcd, N), p)...)
                m, n = size(A)
                x = rand(n)
                y = A * x

                Finch.fwrite(joinpath(instance_out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
                Finch.fwrite(joinpath(instance_out, "A.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(SparseList(Element(0.0)))), (2, 1)), 2, 1), A))
                Finch.fwrite(joinpath(instance_out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
                open(joinpath(instance_out, "instance.json"), "w") do f
                    JSON.print(f, Dict("instance"=>i, "seed"=>seed))
                end
            end
        end

        """
        dense generator
        
        # Intro
        specify a particular size for matrix and seed for dense generation
        
        # Options
        
        - `-o, --out=</data>`: destination directory for the generated problem instances
        - `-e, --ext <extension>`: generated tensor file format extension
        - `-m, --m <value>`: number of rows
        - `-n, --n <value>`: number of columns
        - `-s, --seed <value>`: random seed
        
        """
        @cast function dense(;out = joinpath(@__DIR__, "../data"), ext="bspnpy", m = 1000, n = 1000, seed = rand(UInt))
            Random.seed!(seed);
            A = rand(m, n)
            x = rand(n)
            y = A * x
            Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
            Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(permutedims(Tensor(Dense(Dense(Element(0.0)))), (2, 1)), 2, 1), A))
            Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
            open(joinpath(out, "seed.json"), "w") do f
                JSON.print(f, Dict("seed" => seed))
            end
            
        end


    end

    @cast SpMV

    @main

end # module

using .Generate
Generate.command_main()