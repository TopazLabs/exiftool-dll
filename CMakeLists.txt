cmake_minimum_required(VERSION 3.14)
project(exiftool VERSION 0.1.0 LANGUAGES C)
include(ExternalProject)

# Build perl with autotools
if(MSVC)
    set(PERL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/perl5/lib/CORE")
    set(PERL_STATIC_LIB "${PERL_INCLUDES}/perl536s.lib")
    set(PERL_EXECUTABLE "${PERL_INCLUDES}/perl")
    set(PERL_COMMAND cd win32 && nmake CCTYPE=MSVC142 BUILD_STATIC=define)
else()
    set(PERL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/perl5")
    set(PERL_STATIC_LIB "${PERL_INCLUDES}/libperl.a")
    set(PERL_EXECUTABLE "${PERL_INCLUDES}/perl")
    set(PERL_COMMAND ./configure -des -Dusemultiplicity -Dprivlib=${PERL_INCLUDES} && make)
endif()

file(MAKE_DIRECTORY ${PERL_INCLUDES})
ExternalProject_Add(perl5 # v5.36.0
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/perl5"
    CONFIGURE_COMMAND "" INSTALL_COMMAND ""
    BUILD_COMMAND ${PERL_COMMAND} BUILD_IN_SOURCE true
    BUILD_BYPRODUCTS ${PERL_STATIC_LIB} ${PERL_EXECUTABLE})

# Create imported target for perl
add_library(perl INTERFACE IMPORTED GLOBAL)
target_include_directories(perl INTERFACE ${PERL_INCLUDES})
target_compile_definitions(perl INTERFACE MULTIPLICITY)
if(MSVC)
    target_compile_definitions(perl INTERFACE PERL_IMPLICIT_SYS PERLDLL)
    target_link_libraries(perl INTERFACE ${PERL_STATIC_LIB}
        oldnames.lib kernel32.lib user32.lib advapi32.lib
        comctl32.lib ws2_32.lib ucrt.lib vcruntime.lib libcmt.lib)
    target_link_options(perl INTERFACE /NODEFAULTLIB)
else()
    target_compile_definitions(perl INTERFACE PERL_DARWIN PERL_USE_SFE_PUTENV)
    target_link_options(perl INTERFACE -mmacosx-version-min=12.2 -fstack-protector-strong)
endif()

# Pack exiftool module sources
set(PERL_MODULES Carp.pm Config.pm DynaLoader.pm Fcntl.pm
    integer.pm strict.pm vars.pm warnings.pm XSLoader.pm)
list(TRANSFORM PERL_MODULES PREPEND "perl5/lib/")

add_executable(bin2obj bin2obj.c)
add_custom_command(OUTPUT packed.obj
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory exiftool/lib fatlib
    COMMAND "${CMAKE_COMMAND}" -E copy ${PERL_MODULES} fatlib
    COMMAND "${PERL_EXECUTABLE}" FatPacker.pm hello_world.pl > packed.pl
    COMMAND bin2obj packed.pl > packed.obj
    COMMAND "${CMAKE_COMMAND}" -E rm -r fatlib packed.pl
    DEPENDS perl bin2obj)

# Build exiftool-dll C interface
add_library(exiftool SHARED exiftool.h exiftool.c packed.obj)
target_link_libraries(exiftool PRIVATE perl)
target_compile_definitions(exiftool PRIVATE EXIFTOOL_EXPORTS)

add_executable(test test.c)
target_link_libraries(test PRIVATE exiftool)
