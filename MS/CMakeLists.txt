cmake_minimum_required (VERSION 3.8)

set (LIBS
	"sfml-graphics"
	"sfml-system"
	"sfml-window"
)

add_executable (MS "main.cpp")
target_link_libraries (MS ${LIBS})
