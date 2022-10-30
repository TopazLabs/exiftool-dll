cmake_minimum_required(VERSION 3.14)
project(exiftool VERSION 0.1.0 LANGUAGES C)
include(ExternalProject)

# Build perl with autotools
set(PERL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/perl5/lib/CORE")
set(PERL_COMMAND cd win32 && nmake CCTYPE=MSVC142 BUILD_STATIC=define)
set(PERL_STATIC_LIB "${PERL_INCLUDES}/perl536s.lib")

file(MAKE_DIRECTORY ${PERL_INCLUDES})
ExternalProject_Add(perl # v5.36.0
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/perl5"
    CONFIGURE_COMMAND "" INSTALL_COMMAND ""
    BUILD_COMMAND ${PERL_COMMAND} BUILD_IN_SOURCE true
    BUILD_BYPRODUCTS ${PERL_STATIC_LIB})

# Create imported target for perl
add_library(perl::perl INTERFACE IMPORTED GLOBAL)
target_include_directories(perl::perl INTERFACE ${PERL_INCLUDES})
target_link_libraries(perl::perl INTERFACE ${PERL_STATIC_LIB}
    oldnames.lib kernel32.lib user32.lib advapi32.lib
    comctl32.lib ws2_32.lib ucrt.lib vcruntime.lib libcmt.lib)
target_link_options(perl::perl INTERFACE /NODEFAULTLIB)
target_compile_definitions(perl::perl INTERFACE MULTIPLICITY PERL_IMPLICIT_SYS PERLDLL)

# Pack exiftool module sources
set(PACKER_COMMAND "perl ${CMAKE_CURRENT_SOURCE_DIR}/FatPacker.pm")
set(PACKER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/hello_world.pl")
set(PACKER_DST "${CMAKE_CURRENT_SOURCE_DIR}/packed.pl")
add_custom_command(OUTPUT ${PACKER_DST}
    COMMAND ${PACKER_COMMAND} ${PACKER_SRC} > ${PACKER_DST}
    DEPENDS perl::perl)
add_custom_target(modules DEPENDS ${PACKER_DST})

# Build exiftool-dll C interface
add_library(exiftool SHARED exiftool.h exiftool.c)
target_link_libraries(exiftool PRIVATE perl::perl)
target_compile_definitions(exiftool PRIVATE EXIFTOOL_EXPORTS)
add_dependencies(exiftool modules)

add_executable(test test.c)
target_link_libraries(test PRIVATE exiftool)


#set(FATLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fatlib")
#file(REMOVE_RECURSE ${FATLIB_DIR})
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/exiftool/lib/" DESTINATION ${FATLIB_DIR})
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/perl5/lib/" DESTINATION ${FATLIB_DIR})

#set(PERL_BINARY "${CMAKE_CURRENT_SOURCE_DIR}/perl5/perl.exe")
#file(GLOB_RECURSE PM_FILES "${FATLIB_DIR}/*.pm")
