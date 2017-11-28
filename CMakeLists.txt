project(FIELD_VIS)

cmake_minimum_required(VERSION 3.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE "ON")

if (CMAKE_COMPILER_IS_GNUCXX)
    set(DEBUG_FLAGS "-g -O0 -Wall -pedantic -Wextra -Werror --coverage")
    set(RELEASE_FLAGS "-O3 -Wall -pedantic -Wextra -Werror")
    set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(DEBUG_FLAGS "-g -O0 -Wall -pedantic -Wextra -Werror")
    set(RELEASE_FLAGS "-O3 -Wall -pedantic -Wextra -Werror")
    set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
else (CMAKE_COMPILER_IS_GNUCXX)
    set(DEBUG_FLAGS "/Zi /EHsc /MD /MDd /MP")
    set(RELEASE_FLAGS "/EHsc")
endif (CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")

set(FIELD_VIS_BUILD_DIR ${FIELD_VIS_PROJECT_DIR}/build)

if(WIN32)
    set(PONOS_INCLUDES
            ${PROJECT_SOURCE_DIR}/external/ponos/wbuild/include
            ${PROJECT_SOURCE_DIR}/external/ponos/aergia/external/GLEW
            ${PROJECT_SOURCE_DIR}/external/ponos/aergia/external
            )
    set(PONOS_LIBRARIES ${PROJECT_SOURCE_DIR}/external/ponos/wbuild/lib/libaergia.a
                        ${PROJECT_SOURCE_DIR}/external/ponos/wbuild/lib/libponos.a
                        ${PROJECT_SOURCE_DIR}/external/ponos/wbuild/external/lib/libglfw3.a
                        ${PROJECT_SOURCE_DIR}/external/ponos/aergia/external/GLEW/x64/libglew32d.a
                        -lopengl32
                        -lglu32
            )
else(WIN32)
    set(PONOS_INCLUDES
            ${PROJECT_SOURCE_DIR}/external/ponos/build/include
            ${PROJECT_SOURCE_DIR}/external/ponos/aergia/external/GLEW
            ${PROJECT_SOURCE_DIR}/external/ponos/aergia/external
            )
    set(PONOS_LIBRARIES ${PROJECT_SOURCE_DIR}/external/ponos/build/lib/libaergia.a
                        ${PROJECT_SOURCE_DIR}/external/ponos/build/lib/libponos.a
                        -lglfw -lGL -lGLU -lGLEW
            )
endif(WIN32)

add_subdirectory(src)
