cmake_minimum_required(VERSION 3.20)

add_executable(HttpServerApp 
	main.cpp
	http_connection.h
	http_connection.cpp
	search_engine.h 
	search_engine.cpp
	../config/config.h
	../config/config.cpp
	../config/config.ini
	../database/database.h
	../database/database.cpp
	../spider/text_processing.h
	../spider/text_processing.cpp
)

target_compile_features(HttpServerApp PRIVATE cxx_std_17) 

target_link_libraries(HttpServerApp pqxx)  

target_include_directories(HttpServerApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(HttpServerApp ${Boost_LIBRARIES})

target_link_libraries(HttpServerApp OpenSSL::SSL)

