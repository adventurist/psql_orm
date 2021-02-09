cmake_minimum_required(VERSION 3.6)
project(psqlorm)
set(CMAKE_CXX_STANDARD 17)
set(PQXX /usr/local/include/pqxx)
find_library(UUID uuid)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
file(GLOB_RECURSE SOURCE true src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE
   "src"
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PQXX_LIB} ${PQ_LIB})
