cmake_minimum_required(VERSION 3.20)

project(Spider)


# TODO: Define by your own needs:
set(OPENSSL_ROOT_DIR "C:\\Program Files\\OpenSSL")

set(BOOST_ROOT "C:\\Boost\\boost_1_84_0\\stage")

set(libpqxx_DIR "C:\\Program Files\\libpqxx-7.9.1")


# TODO: If you are running on Windows below 10, change this:

add_compile_definitions(_WIN32_WINNT=0x0A01)

# Boost:
find_package(Boost 1.84.0 REQUIRED COMPONENTS system thread locale)

if(NOT Boost_FOUND)
    message3(FATAL_ERROR "Could not find Boost")
endif()

# PQXX
set(PostgreSQL_ADDITIONAL_VERSIONS "15")

add_subdirectory(${libpqxx_DIR} libpqxx-build) 

# OpenSSL
find_package(OpenSSL REQUIRED)

add_subdirectory(spider)

add_subdirectory(http_server)
