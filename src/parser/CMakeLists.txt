include_directories(../)
include_directories(../common)

set (LIBS
    ${LIBS}
    expression.cc
    expression.h
    parser.cc
    parser.h
    common.h
    )

add_library(parser ${LIBS})
