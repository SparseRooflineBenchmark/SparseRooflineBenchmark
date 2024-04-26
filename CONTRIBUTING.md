We welcome contributions to the benchmark suite, and follow the [Julia contributing
guidelines](https://github.com/JuliaLang/julia/blob/master/CONTRIBUTING.md).  If
you use or want to use the benchmark and have a question or bug, please do file a
[Github issue](https://github.com/SparseRooflineBenchmark/SparseRooflineBenchmark/issues)!  If you want
to contribute, please first file an issue to double check that there is
interest from a contributor in the feature or bugfix.

## Versions

The benchmark is currently in a pre-release state. The API is not yet stable, and
breaking changes may occur between minor versions. We follow [semantic
versioning](https://semver.org/) and will release 1.0 when the API is stable.
The main branch is the most up-to-date development branch.
While it is not stable, it should always pass tests.

Contributors will develop and test from a local directory. Please see the
[Julia package documentation](https://pkgdocs.julialang.org/v1/getting-started/) for
more info on Julia versioning, particularly the section on
[developing](https://pkgdocs.julialang.org/v1/managing-packages/#developing).

## Utilities

The benchmark includes several scripts that can be executed directly, e.g. `generate.jl`.
These scripts are all have local [Pkg
environments](https://pkgdocs.julialang.org/v1/getting-started/#Getting-Started-with-Environments).
The scripts include convenience headers to automatically use their respective
environments, so you won't need to worry about `--project=.` flags, etc.

## How to file a bug report

A useful bug report filed as a GitHub issue provides information about how to reproduce the error.

1. Before opening a new [GitHub issue](https://github.com/JuliaLang/julia/issues):
  - Try searching the existing issues to see if someone else has already noticed the same problem.
  - Try some simple debugging techniques to help isolate the problem.

2. When filing a bug report, provide where possible:
  - The full error message, including the backtrace.
  - A minimal working example, i.e. the smallest chunk of code that triggers the error. Ideally, this should be code that can be pasted into a REPL or run from a source file.
  - The version of Julia as provided by the `versioninfo()` command. Occasionally, the longer output produced by `versioninfo(verbose = true)` may be useful also, especially if the issue is related to a specific package.

4. When pasting code blocks or output, put triple backquotes (\`\`\`) around the text so GitHub will format it nicely. Code statements should be surrounded by single backquotes (\`). Be aware that the `@` sign tags users on GitHub, so references to macros should always be in single backquotes. See [GitHub's guide on Markdown](https://guides.github.com/features/mastering-markdown) for more formatting tricks.

## Testing

All pull requests should pass continuous integration testing before merging.
Write a test that exercises the functionality related to the PR --- you can add
your test to one of the existing files, or start a new one, whichever seems most
appropriate to you. The test suite is currently in `./build.sh`