cmake_minimum_required(VERSION 3.9)
project (ElBoana  C)

find_package(SDL2)

add_executable (ab WIN32
  	${CMAKE_SOURCE_DIR}/c/ab.c
  	${CMAKE_SOURCE_DIR}/c/data.c
	${CMAKE_SOURCE_DIR}/c/eval.c
	${CMAKE_SOURCE_DIR}/c/feature.c
	${CMAKE_SOURCE_DIR}/c/init.c
	${CMAKE_SOURCE_DIR}/c/intrface.c
	${CMAKE_SOURCE_DIR}/c/move.c
	${CMAKE_SOURCE_DIR}/c/search.c
	${CMAKE_SOURCE_DIR}/c/sysdept.c
	${CMAKE_SOURCE_DIR}/c/tables.c
	${CMAKE_SOURCE_DIR}/c/vga.c

	${CMAKE_SOURCE_DIR}/c/graph.c
)
target_link_libraries (ab ${SDL_LIBRARY})
target_include_directories(ab PUBLIC ${CMAKE_SOURCE_DIR}/h ${SDL_INCLUDE_DIR} )
  
set_target_properties(ab PROPERTIES LINKER_LANGUAGE C)
