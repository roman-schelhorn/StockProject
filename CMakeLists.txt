cmake_minimum_required(VERSION 3.16)
project(stock_analysis_cpp)
set(CMAKE_CXX_STANDARD 17)

add_executable(stock_analysis
    src/main.cpp
)

# If you add libraries later, link them here:
# target_link_libraries(stock_analysis PRIVATE ...)
