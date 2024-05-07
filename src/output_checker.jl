if abspath(PROGRAM_FILE) == @__FILE__
    using Pkg
    Pkg.activate(@__DIR__, io = devnull)
    Pkg.instantiate()
end

if length(ARGS) != 5
    println("Usage: julia output_checker.jl <A.ext> <x.ext> <y.ext> <y_ref.ext> epsilon")
    exit(1)
end

using Finch
using NPZ
using TensorMarket

flag = Scalar(true)
y_bound = Tensor(Dense(Element(0.0)))
A = fread(ARGS[1])
x = fread(ARGS[2])
y = fread(ARGS[3])
y_ref = fread(ARGS[4])
epsilon = fread(ARGS[5])
(m, n) = size(A)

@finch begin
    y_bound .= 0
    for j = _
        for i = _
            @einsum y_bound[i] += abs(A[i, j]*x[j])
        end
    end
    for i = _
        @einsum flag[] &= (abs(y_ref[i] - y[i]) <= n*epsilon*y_bound[i])
    end
    return flag
end


if (flag == Scalar(true))
    println("output is correct")
else
    error("y doesn't match y_ref")
end

