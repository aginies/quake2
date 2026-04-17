cmake_minimum_required(VERSION 3.5)

project(QPAKMAN CXX)

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(qpakman
	arc_spec.cc u_assert.cc u_file.cc u_util.cc im_color.cc im_image.cc
	 im_gen.cc im_mip.cc im_png.cc im_tex.cc archive.cc pakfile.cc main.cc)

target_include_directories(qpakman PRIVATE ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
target_compile_definitions(qpakman PRIVATE ${PNG_DEFINITIONS})

if(UNIX)
	target_compile_definitions(qpakman PRIVATE UNIX)
elseif(WIN32)
	target_compile_definitions(qpakman PRIVATE WIN32)
else()
	message(SEND_ERROR "You are building on unsupported platform")
endif()

target_compile_options(qpakman PRIVATE -fno-rtti)

target_link_libraries(qpakman PRIVATE -lm ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS qpakman RUNTIME DESTINATION bin)
