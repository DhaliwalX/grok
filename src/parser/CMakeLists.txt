include_directories(../)
include_directories(../common)

set (LIBS
    ${LIBS}
    expression.cc
    expression.h
    functionstatement.cc
    functionstatement.h
    blockstatement.h
    blockstatement.cc
    forstatement.cc
    forstatement.h
    ifstatement.h
    ifstatement.cc
    parser.cc
    parser.h
    common.h
    )

add_library(parser ${LIBS})
