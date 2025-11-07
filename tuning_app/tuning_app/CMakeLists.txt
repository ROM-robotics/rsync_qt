cmake_minimum_required(VERSION 3.19)
project(tuning_app LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui Charts Network WebSockets Quick QuickWidgets Qml)

if (ANDROID)
    message(STATUS "Configuring for Android: ABI(s)=${QT_ANDROID_ABIS} SDK=${ANDROID_SDK_ROOT} NDK=${ANDROID_NDK_ROOT}")
endif()

# Prefer Eigen via package if available (portable for Android)
find_package(Eigen3 QUIET)

qt_standard_project_setup()

qt_add_executable(tuning_app
    MANUAL_FINALIZATION
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sdk/rom_time.hpp
    design/rom_structures.h
    communication/ros_bridge_client.hpp
    communication/ros_bridge_client.cpp
    design/rom_design.hpp
    design/readmeviewer.h
    design/readmeviewer.cpp
    design/covarianceDisplay.hpp
    design/covarianceDisplay.cpp
    sdk/rom_map_widget.hpp
    sdk/rom_time.cpp
    sdk/rom_map_widget.cpp
    sdk/carto_rom_map_widget.hpp
    sdk/carto_rom_map_widget.cpp





    
)

target_link_libraries(tuning_app PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    Qt${QT_VERSION_MAJOR}::Qml
)

# Add Eigen includes if found; otherwise fall back to system include path on non-Android
if (Eigen3_FOUND)
    target_link_libraries(tuning_app PRIVATE Eigen3::Eigen)
else()
    # Fallback: fetch Eigen headers when not found (works for Android too)
    include(FetchContent)
    set(EIGEN_VERSION 3.4.0)
    set(FETCHCONTENT_QUIET OFF)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        eigen3
        URL https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(eigen3)
    # Use provided target if available; otherwise create a simple interface
    if (TARGET Eigen3::Eigen)
        target_link_libraries(tuning_app PRIVATE Eigen3::Eigen)
    else()
        add_library(Eigen3::Eigen INTERFACE IMPORTED)
        set_target_properties(Eigen3::Eigen PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${eigen3_SOURCE_DIR}"
        )
        target_link_libraries(tuning_app PRIVATE Eigen3::Eigen)
    endif()
endif()

include(GNUInstallDirs)

qt_add_resources(tuning_app "resources"
    PREFIX "/"
    FILES
        Speed.qml
        RightSpeed.qml
        LeftSpeed.qml
        ActualSpeed.qml
        ActualRightSpeed.qml
        ActualLeftSpeed.qml
)



# Android packaging: point to custom android/ dir if present
if (ANDROID)
    # Use the Qt-specific property name (Qt 6)
    set_property(TARGET tuning_app PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

# Finalize target for Qt 6 (sets up bundles, rpaths, etc.)
qt_finalize_executable(tuning_app)

# Desktop/Linux install and deploy; skip for Android where Gradle handles packaging
if (NOT ANDROID)
install(TARGETS tuning_app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET tuning_app
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
endif()


