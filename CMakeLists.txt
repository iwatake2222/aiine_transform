cmake_minimum_required(VERSION 3.0)

# Create project
set(ProjectName "main")
project(${ProjectName})

# Select build system and set compile options
include(${PROJECT_SOURCE_DIR}/common_helper/cmakes/build_setting.cmake)

# Copy resouce
file(COPY ${PROJECT_SOURCE_DIR}/resource DESTINATION ${CMAKE_BINARY_DIR}/)
add_definitions(-DRESOURCE_DIR="${CMAKE_BINARY_DIR}/resource/")

# Create executable file
add_executable(${ProjectName} main.cpp)

# Sub modules
## CommonHelper
add_subdirectory(${PROJECT_SOURCE_DIR}/common_helper)
target_include_directories(${ProjectName} PUBLIC ${PROJECT_SOURCE_DIR}/common_helper)
target_link_libraries(${ProjectName} CommonHelper)

## InferenceHelper
set(INFERENCE_HELPER_DIR ${PROJECT_SOURCE_DIR}/inference_helper)
set(INFERENCE_HELPER_ENABLE_TFLITE ON CACHE BOOL "INFERENCE_HELPER_ENABLE_TFLITE" FORCE)
set(INFERENCE_HELPER_ENABLE_TFLITE_DELEGATE_XNNPACK ON CACHE BOOL "INFERENCE_HELPER_ENABLE_TFLITE_DELEGATE_XNNPACK" FORCE)
add_subdirectory(${INFERENCE_HELPER_DIR}/inference_helper inference_helper)

## OpenCV
find_package(OpenCV REQUIRED)
target_include_directories(${ProjectName} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${ProjectName} ${OpenCV_LIBS})

## ImageProcessor
add_subdirectory(${PROJECT_SOURCE_DIR}/image_processor)
target_include_directories(${ProjectName} PUBLIC ${PROJECT_SOURCE_DIR}/image_processor)
target_link_libraries(${ProjectName} ImageProcessor)
