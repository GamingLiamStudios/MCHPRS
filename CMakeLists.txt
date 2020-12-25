cmake_minimum_required(VERSION 3.9)
project(mchprs)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
option(BUILD_SHARED_LIBS "" off)

add_subdirectory(extern)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
    # MAIN
    source/main.cpp

    # SERVER
    source/server.cpp
    source/player.cpp

    # NETWORK
    source/network/network.cpp
    source/network/socket.cpp # Stolen from another project due to convienince

)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC "extern")
target_include_directories(${PROJECT_NAME} PUBLIC "source") # to allow paths relative to project root

target_link_libraries(${PROJECT_NAME} PRIVATE zlib Threads::Threads fmt)
IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)         
ENDIF()      
