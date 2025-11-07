cmake_minimum_required(VERSION 3.30.5)
set(CMAKE_MODULE_PATH "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-src/cmake")
project(CheckFortran Fortran)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/result.cmake"
  "set(CMAKE_Fortran_COMPILER \"${CMAKE_Fortran_COMPILER}\")\n"
  
  )