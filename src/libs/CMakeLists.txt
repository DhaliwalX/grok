set(GROK_LIBS_SOURCE_FILES "")
add_subdirectory(./array)
add_subdirectory(./example)
add_subdirectory(./console)
add_subdirectory(./math)
add_subdirectory(./timer)
add_subdirectory(./string)
add_subdirectory(./regex)

set(GROK_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/library.cc
	${CMAKE_CURRENT_SOURCE_DIR}/library.h
        ${GROK_LIBS_SOURCE_FILES}
	${GROK_SOURCE_FILES}
	PARENT_SCOPE
)
