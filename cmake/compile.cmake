if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wpedantic
      #  -Weverything

#        -fno-omit-frame-pointer
#        -fsanitize=address
#        -fsanitize=undefined
#        -fsanitize=(un)signed-integer-overflow
        )
   add_link_options(
#        -fsanitize=address
#        -fsanitize=undefined
#        -fsanitize=(un)signed-integer-overflow
       )
endif()