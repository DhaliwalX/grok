include_directories(..)

set(PARSER_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/astvisitor.h
	${CMAKE_CURRENT_SOURCE_DIR}/blockstatement.cc
	${CMAKE_CURRENT_SOURCE_DIR}/blockstatement.h
	${CMAKE_CURRENT_SOURCE_DIR}/bytecode.h
	${CMAKE_CURRENT_SOURCE_DIR}/common.h
	${CMAKE_CURRENT_SOURCE_DIR}/expression.cc
	${CMAKE_CURRENT_SOURCE_DIR}/expression.h
	${CMAKE_CURRENT_SOURCE_DIR}/forstatement.cc
	${CMAKE_CURRENT_SOURCE_DIR}/forstatement.h
	${CMAKE_CURRENT_SOURCE_DIR}/functionstatement.cc
	${CMAKE_CURRENT_SOURCE_DIR}/functionstatement.h
	${CMAKE_CURRENT_SOURCE_DIR}/ifstatement.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ifstatement.h
	${CMAKE_CURRENT_SOURCE_DIR}/parser.cc
	${CMAKE_CURRENT_SOURCE_DIR}/parser.h
	${CMAKE_CURRENT_SOURCE_DIR}/returnstatement.h
)

set(PARSER_SOURCE_FILES_FOR_VM
	${CMAKE_CURRENT_SOURCE_DIR}/genexpression.cc
	${CMAKE_CURRENT_SOURCE_DIR}/genarray.cc
	${CMAKE_CURRENT_SOURCE_DIR}/genfunction.cc
	${CMAKE_CURRENT_SOURCE_DIR}/genobject.cc
)

add_library(parser ${PARSER_SOURCE_FILES} ${PARSER_SOURCE_FILES_FOR_VM})

if (GROK_JIT)
    add_library(parser_jit ${PARSER_SOURCE_FILES})
    set_target_properties(parser_jit PROPERTIES
    	COMPILE_FLAGS -DNO_EMIT_FUNCTION)
endif (GROK_JIT)

