include_directories(../)
set (LIBS
    instruction.cc
    instruction.h
    instruction-list.cc
    instruction-list.h
    instruction-builder.cc
    instruction-builder.h
    genexpression.cc
    genfunction.cc
    ${LIBS}
    )

add_library(vm ${LIBS})
