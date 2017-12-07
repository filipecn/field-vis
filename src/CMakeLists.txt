file(GLOB HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )

file(GLOB SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PONOS_INCLUDES})

add_executable(fieldVis ${SOURCES} ${HEADERS} particle_system.cpp particle_system.h procedural_field.cpp procedural_field.h utils.cpp utils.h)

target_link_libraries(fieldVis ${PONOS_LIBRARIES})
