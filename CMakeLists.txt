cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(ProfessorSuzu)

add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
find_package(Boost 1.81.0 REQUIRED COMPONENTS log system)

find_package(OpenSSL 3.1.1 REQUIRED COMPONENTS SSL)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_library(Session src/session.cpp)
  target_link_libraries(Session Boost::log Boost::system)

  add_executable(ProfessorSuzu src/main.cpp)
  target_link_libraries(ProfessorSuzu Session)
endif()

if(OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(Session OpenSSL::SSL)
  target_link_libraries(ProfessorSuzu OpenSSL::SSL)
endif()

