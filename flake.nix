# Convenience tool designed for users who don't want to manage installing project dependencies.
# Includes Julia, binsparse C libraries + utils

## SETUP
# Requires Nix.
# To install nix:
#   For users with root access: https://nixos.org/download
#   For users without root access: https://github.com/DavHau/nix-portable
# Enable flakes by adding the following line to ~/.config/nix/nix.conf or /etc/nix/nix.conf:
#   experimental-features = nix-command flakes
# Activate with:
#   $ nix develop (regular nix install)
#   $ nix-portable nix develop (non-root nix-portable install)
# Think of this like a virtualenv shell - the tools are
# only available from within the nix shell.
# You MUST activate this shell before compilation.

## USAGE - C environment
# This environment includes the reference C Binsparse library.
# This library uses runtime polymorphism (scalar value + index types known at
# runtime), unlike the C++ reference library, which requires these types to be
# known at compile-time.

# If compiling with C binsparse libraries, you MUST use the wrapped Nix C
# compiler from inside the nix-shell. This is just the `cc` executable, which
# is usually gcc on linux and clang on mac (just check with cc --version).
# Please let me know if you'd like to set another compiler version

# You MUST use `-lbinsparse-rc -lhdf5 -lcjson` flags with the compiler.

# Libraries can be accessed using #include <binsparse/binsparse.h>

## USAGE - tools
# This environment includes several executables:
# - bsp2mtx: binsparse -> matrix market conversion
#      Formats must be identical
# - mtx2bsp: matrix market -> binsparse conversion.
#      Formats must be identical, OR must be a COO->CSR conversion
#      To perform CSR conversion, append CSR to the command, e.g.
#      mtx2bsp in.mtx out.hdf5 CSR
# - check_equivalence: check if two binsparse files store the same matrix.
# - bsp-ls: dumps a bit of info about the a binsparse file.
# - bsp_simple_write, bsp_simple_matrix_write:
#       Generates 1000 (vec) or 1000x1000 (mat) file.
# - bsp_simple_read, bsp_simple_matrix_read:
#       Print elements of file. Filenames hardcoded to test.hdf5

{
  description = "sparse benchmark working shell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: 
    flake-utils.lib.eachDefaultSystem (system: 
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            pkgs.julia-bin
            pkgs.hdf5
            pkgs.cjson

            (pkgs.stdenv.mkDerivation rec {
              name = "binsparse-reference-c";
              version = "unstable-2025-03-13";
              src = pkgs.fetchFromGitHub {
                owner = "GraphBLAS";
                repo = "binsparse-reference-c";
                rev = "fbf3902a2aa378477a35adc4315f006fd18f07ee";
                hash = "sha256-gnNuc0+dRxj8MIaA+DufJOJgwpKu+Nmc0HrR84/pGao=";
              };

              propagatedBuildInputs = with pkgs; [ hdf5 cjson ];
              nativeBuildInputs = [ pkgs.cmake ];
              # clang doesn't handle -fext-numeric-literals, so we only want it for gcc
              # On the other hand, clang doesn't like implicit conversions
              CXXFLAGS = if builtins.elem system ["x86_64-linux" "aarch64-linux"] then "-fext-numeric-literals" else "";
              #use nix-provided cjson rather than downloading a new copy
              patchPhase = ''
                sed -i '29,34d' CMakeLists.txt
                substituteInPlace CMakeLists.txt --replace 'FetchContent_MakeAvailable' 'find_package'
                substituteInPlace CMakeLists.txt --replace ''\'''${cJSON_SOURCE_DIR}' '${pkgs.cjson}/include/cjson'
                substituteInPlace include/binsparse/read_matrix.h --replace 'cJSON/' 'cjson/'
                substituteInPlace include/binsparse/write_matrix.h --replace 'cJSON/' 'cjson/'
                substituteInPlace include/binsparse/detail/declamp_values.h --replace '1j' '1i'
              '';

              enableParallelBuilding = true;

              # some weird case stuff required since nix cjson package is all lowercase
              installPhase = ''
                mkdir -p $out/bin $out/include $out/lib
                
                install -Dm644 libbinsparse-rc.a $out/lib/libbinsparse-rc.a

                install -Dm755 examples/simple_read $out/bin/bsp_simple_read
                install -Dm755 examples/simple_write $out/bin/bsp_simple_write
                install -Dm755 examples/simple_matrix_read $out/bin/bsp_simple_matrix_read
                install -Dm755 examples/simple_matrix_write $out/bin/bsp_simple_matrix_write
                install -Dm755 examples/bsp-ls $out/bin/bsp-ls
                install -Dm755 examples/check_equivalence $out/bin/check-equivalence
                install -Dm755 examples/bsp2mtx $out/bin/bsp2mtx
                install -Dm755 examples/mtx2bsp $out/bin/mtx2bsp
                cp -r $src/include/binsparse $out/include
                substituteInPlace $out/include/binsparse/read_matrix.h --replace 'cJSON/' 'cjson/'
                substituteInPlace $out/include/binsparse/write_matrix.h --replace 'cJSON/' 'cjson/'
              '';
            })
          ];
        };
      });
}
