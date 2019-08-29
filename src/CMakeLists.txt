include_directories(
	${GLAD_INCLUDE_DIR}
	${KHR_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
	${SPDLOG_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${BOWSHOCK_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)
        
set( BOWSHOCK_EDITOR_SRC
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)


add_executable(bowshock_editor
    ${BOWSHOCK_EDITOR_SRC}
)

#set_property(TARGET bowshock_editor PROPERTY )

add_compile_definitions(_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
add_compile_definitions(_WIN32_WINNT=0x0600)

target_link_libraries(bowshock_editor PRIVATE restinio::restinio 
											  nlohmann_json 
											  nlohmann_json::nlohmann_json
											  ${GLFW_LIBRARY_RELEASE} 
											  ${OPENGL_GL_LIBRARY} 
											  ${ASSIMP_LIBRARY_RELEASE} 
											  ${BOWSHOCK_LIBRARY_RELEASE} )

set_property(TARGET bowshock_editor PROPERTY CXX_STANDARD 17)

