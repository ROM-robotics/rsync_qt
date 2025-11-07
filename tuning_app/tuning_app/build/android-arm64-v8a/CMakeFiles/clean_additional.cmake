# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/tuning_app_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/tuning_app_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/spbenchsolver_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/spbenchsolver_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/spsolver_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/spsolver_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/test_sparseLU_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/bench/spbench/CMakeFiles/test_sparseLU_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/bench/spbench/spbenchsolver_autogen"
  "_deps/eigen3-build/bench/spbench/spsolver_autogen"
  "_deps/eigen3-build/bench/spbench/test_sparseLU_autogen"
  "_deps/eigen3-build/blas/CMakeFiles/eigen_blas_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/blas/CMakeFiles/eigen_blas_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/blas/CMakeFiles/eigen_blas_static_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/blas/CMakeFiles/eigen_blas_static_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/blas/eigen_blas_autogen"
  "_deps/eigen3-build/blas/eigen_blas_static_autogen"
  "_deps/eigen3-build/lapack/CMakeFiles/eigen_lapack_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/lapack/CMakeFiles/eigen_lapack_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/lapack/CMakeFiles/eigen_lapack_static_autogen.dir/AutogenUsed.txt"
  "_deps/eigen3-build/lapack/CMakeFiles/eigen_lapack_static_autogen.dir/ParseCache.txt"
  "_deps/eigen3-build/lapack/eigen_lapack_autogen"
  "_deps/eigen3-build/lapack/eigen_lapack_static_autogen"
  "tuning_app_autogen"
  )
endif()
