using Pkg
Pkg.add("MatrixMarket")
using SparseArrays
using MatrixMarket

A = MatrixMarket.mmread("/nscratch/icy2150/converter/A_output.mtx")
B = MatrixMarket.mmread("/nscratch/icy2150/converter/A_output.mtx")

if (size(A,2) != size(B,1))
	error("Incompatible dimensions for matrix multiplication")
end

result = A * B

MatrixMarket.mmwrite("/nscratch/icy2150/converter/C_output_check.mtx", result)

println("Resulting matrix:")
println(result)
