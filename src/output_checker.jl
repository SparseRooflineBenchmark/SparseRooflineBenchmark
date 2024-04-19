import Pkg;
Pkg.add("Finch")
Pkg.add("NPZ")
Pkg.add("TensorMarket")
if length(ARGS) != 3
    println("Usage: julia output_checker.jl <A.ext> <x.ext> <y_ref.ext>")
    exit(1)
end

using Finch
using NPZ
using TensorMarket
A = fread(ARGS[1])
x = fread(ARGS[2])
y_ref = fread(ARGS[3])
(m, n) = size(A)

println(size(A))
println(size(x))
println(size(y_ref))

y = A*x

if (y!=y_ref)
    error("output does not match")
else
    println("matches output")
end