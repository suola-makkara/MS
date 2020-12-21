cmake_minimum_required (VERSION 3.8)

file (GLOB RES "${CMAKE_CURRENT_SOURCE_DIR}/res/*")

foreach (FILE ${RES})
	get_filename_component (FILE_NAME "${FILE}" NAME)
	configure_file("${FILE}" "${CMAKE_CURRENT_BINARY_DIR}/res/${FILE_NAME}" COPYONLY)
endforeach(FILE)

set (LIBS
	"sfml-graphics"
	"sfml-system"
	"sfml-window"
)

add_executable (MS "main.cpp")
target_link_libraries (MS ${LIBS})
