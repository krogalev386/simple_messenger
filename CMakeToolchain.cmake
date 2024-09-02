set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_FLAGS "-std=c++17")

include_directories("/usr/lib/llvm-14/include")
link_directories("usr/lib/llvm-14/lib")
