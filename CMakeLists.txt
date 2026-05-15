cmake_minimum_required(VERSION 3.17)
project(CTC_smart_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ctc_smart_queue src/main.cpp src/SmartQueue.cpp)

target_compile_options(ctc_smart_queue PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)
