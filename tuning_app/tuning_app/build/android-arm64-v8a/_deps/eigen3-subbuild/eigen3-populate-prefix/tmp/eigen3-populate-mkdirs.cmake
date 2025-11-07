# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-src")
  file(MAKE_DIRECTORY "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-src")
endif()
file(MAKE_DIRECTORY
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-build"
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix"
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/tmp"
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp"
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/src"
  "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mr_robot/Desktop/Git/rsync_qt/tuning_app/tuning_app/build/android-arm64-v8a/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
