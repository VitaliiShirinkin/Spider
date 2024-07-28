cmake_minimum_required(VERSION 3.20)

add_executable(SpiderApp
	main.cpp
	http_utils.h
	http_utils.cpp
	link.h
	../config/config.h
	../config/config.cpp
	../config/config.ini
	../database/database.h
	../database/database.cpp
	db_client.h
	db_client.cpp
	text_processing.h 
	text_processing.cpp)

target_compile_features(SpiderApp PRIVATE cxx_std_17) 

target_link_libraries(SpiderApp pqxx)

target_link_libraries(SpiderApp OpenSSL::SSL)

target_include_directories(SpiderApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(SpiderApp ${Boost_LIBRARIES})
