# Convenience tool designed for users who don't want to manage installing project dependencies.
# Includes Julia, binsparse C libraries + utils

## SETUP
# Requires Nix.
# For users with root access: https://nixos.org/download
# For users without root access: https://github.com/DavHau/nix-portable
# Activate with:
#   $ nix-shell (regular nix install)
#   $ nix-portable nix-shell (non-root nix-portable install)
# Think of this like a virtualenv shell - the tools are
# only available from within the nix shell.

## USAGE - C environment
# This environment includes the reference C Binsparse library.
# This library uses runtime polymorphism (scalar value + index types known at
# runtime), unlike the C++ reference library, which requires these types to be
# known at compile-time.

# If compiling with C binsparse libraries, you MUST use the wrapped Nix C
# compiler from inside the nix-shell. This is just the `cc` executable, which
# is usually gcc on linux and clang on mac (just check with cc --version).
# Please let me know if you'd like to set another compiler version
# You MUST use `-lhdf5 -lcjson` flags with the compiler.
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

{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {

  buildInputs = [
    pkgs.julia-bin
    pkgs.hdf5
    pkgs.cjson

    (pkgs.stdenv.mkDerivation rec {
      name = "binsparse-reference-c";
      version = "unstable-2024-05-20";
      src = pkgs.fetchFromGitHub {
        owner = "GraphBLAS";
        repo = "binsparse-reference-c";
        rev = "293228b55e24eaa22e6ed7e9dca372864fd8ce5b";
        hash = "sha256-t2cgBNVdCE6JG5NXJFg2g7HbsTqM1LHD2fP05Trhv/M=";
      };
      
      propagatedBuildInputs = with pkgs; [ hdf5 cjson ];
      nativeBuildInputs = [ pkgs.cmake ];

      #use nix-provided cjson rather than downloading a new copy
      patchPhase = ''
        sed -i '14,19d' CMakeLists.txt
        substituteInPlace CMakeLists.txt --replace 'FetchContent_MakeAvailable' 'find_package'
        substituteInPlace CMakeLists.txt --replace ''\'''${cJSON_SOURCE_DIR}' '${pkgs.cjson}/include/cjson'
        substituteInPlace include/binsparse/read_matrix.h --replace 'cJSON/' 'cjson/'
        substituteInPlace include/binsparse/write_matrix.h --replace 'cJSON/' 'cjson/'
      '';

      enableParallelBuilding = true;

      # some weird case stuff required since nix cjson package is all lowercase
      installPhase = ''
        mkdir -p $out/bin $out/include
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

    # keep this line if you use bash
    pkgs.bashInteractive
  ];
}
