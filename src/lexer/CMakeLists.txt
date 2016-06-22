set (LIBS 
    ${LIBS}
    lexer.cc
    lexer.h
    token.h
    )

add_library(lexer ${LIBS})

