if (DOXYGEN)

# disable DOT in doxygen if dot is not found
if(DOXYGEN_DOT_PATH)
    set(DOXYGEN_HAVE_DOT "YES")
else(DOXYGEN_DOT_PATH)
    set(DOXYGEN_HAVE_DOT "NO")
endif(DOXYGEN_DOT_PATH)

# configure the doxygen file
configure_file (${TPS_SOURCE_DIR}/dox/Doxyfile.in ${TPS_BINARY_DIR}/doc/Doxyfile)

add_custom_command (OUTPUT ${TPS_BINARY_DIR}/doc/html/index.html
                    COMMAND ${DOXYGEN} ${TPS_BINARY_DIR}/doc/Doxyfile
                    DEPENDS ${TPS_CPU_SRCS} ${TPS_GPU_SRCS}
                    ${TPS_SOURCE_DIR}/dox/*.dox
                    ${TPS_SOURCE_DIR}/src/*.h
                    ${TPS_SOURCE_DIR}/src/*.cpp
                    ${TPS_SOURCE_DIR}/extensions/*/plugin/*.cpp
                    ${TPS_SOURCE_DIR}/extensions/*/plugin/*.h
                    ${TPS_SOURCE_DIR}/dox/Doxyfile.in)
                    #${TPS_SOURCE_DIR}/dox/DoxygenLayout.xml)

add_custom_target (doc ALL DEPENDS ${TPS_BINARY_DIR}/doc/html/index.html)

endif (DOXYGEN)
