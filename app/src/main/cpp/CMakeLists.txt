# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("androidexample")

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.
add_library(androidexample SHARED
        main.cpp
        AndroidOut.cpp
        Renderer.cpp
        Shader.cpp
        TextureAsset.cpp
        Model.cpp
        Utility.cpp)

# Searches for a package provided by the game activity dependency
find_package(game-activity REQUIRED CONFIG)
add_subdirectory(${CMAKE_SOURCE_DIR}/Externals/assimp-5.4.2)
include_directories(${CMAKE_SOURCE_DIR}/Externals/assimp-5.4.2/include)
include_directories(${CMAKE_SOURCE_DIR}/Externals/glm-1.0.1-light)
# Configure libraries CMake uses to link your target library.
target_link_libraries(androidexample
        # The game activity
        game-activity::game-activity

        assimp
        # EGL and other dependent libraries required for drawing
        # and interacting with Android system
        EGL
        GLESv3
        jnigraphics
        android
        log)

