cmake_minimum_required(VERSION 3.30)
project(particles VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)
FetchContent_MakeAvailable(SDL_image)

FetchContent_Declare(
    SDL_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
)
FetchContent_MakeAvailable(SDL_ttf)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(DUMMY_FILE "${CMAKE_BINARY_DIR}/force_recompile.txt")
add_custom_command(
    OUTPUT ${DUMMY_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Force recompile" > ${DUMMY_FILE}
    COMMAND ${CMAKE_COMMAND} -E touch ${DUMMY_FILE}
    COMMENT "Updating dummy file to force recompilation of sources"
)

add_custom_target(update_dummy ALL DEPENDS ${DUMMY_FILE})

foreach(src ${SOURCES})
    set_source_files_properties(${src} PROPERTIES OBJECT_DEPENDS ${DUMMY_FILE})
endforeach()

add_executable(particles ${SOURCES})

target_include_directories(particles PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(particles PRIVATE
    SDL3::SDL3 
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

add_dependencies(particles update_dummy)
