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
        @cast function suitesparse(key; out = joinpath(@__DIR__, "../data"), ext="bspnpy")
            A = SparseMatrixCSC(matrixdepot(key))
            m, n = size(A)
            x = rand(n)
            y = A * x
            Finch.fwrite(joinpath(out, "y_ref.$ext"), copyto!(Tensor(Dense(Element(0.0))), y))
            Finch.fwrite(joinpath(out, "A.$ext"), copyto!(swizzle(Tensor(Dense(SparseList(Element(0.0)))), 2, 1), A))
            Finch.fwrite(joinpath(out, "x.$ext"), copyto!(Tensor(Dense(Element(0.0))), x))
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

    @cast SpMV

    @main

end # module

using .Generate
Generate.command_main()