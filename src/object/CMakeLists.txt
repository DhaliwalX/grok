include_directories(..)

set(LIBS
    object.h
    jsobject.cc
    jsobject.h
    jsbasicobject.h
    jsfunction.h
    jsarray.h
    ${LIBS}
)

add_library(object ${LIBS})
