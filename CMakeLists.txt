cmake_minimum_required(VERSION 3.14)
project(exiftool VERSION 0.1.0 LANGUAGES C)
include(ExternalProject)

# Build perl with autotools
set(PERL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/perl5)
if(MSVC)
    set(PERL_STATIC_LIB ${PERL_PREFIX}/perl536s.lib)
    set(PERL_BUILD cd win32 && nmake CCTYPE=MSVC142 BUILD_STATIC=define)
else()
    set(PERL_STATIC_LIB ${PERL_PREFIX}/libperl.a)
    set(PERL_BUILD ./configure -des -Dusemultiplicity
        -Dprivlib=${PERL_PREFIX}/lib && make -j)
endif()

file(MAKE_DIRECTORY ${PERL_PREFIX}/lib/CORE)
ExternalProject_Add(perl5 # v5.36.0
    SOURCE_DIR ${PERL_PREFIX} CONFIGURE_COMMAND "" INSTALL_COMMAND ""
    BUILD_COMMAND ${PERL_BUILD} BUILD_IN_SOURCE true
    BUILD_BYPRODUCTS ${PERL_STATIC_LIB} ${PERL_PREFIX}/perl)

# Create imported target for perl
add_library(perl INTERFACE IMPORTED GLOBAL)
target_include_directories(perl INTERFACE ${PERL_PREFIX} ${PERL_PREFIX}/lib/CORE)
target_compile_definitions(perl INTERFACE MULTIPLICITY)
if(MSVC)
    target_compile_definitions(perl INTERFACE PERL_IMPLICIT_SYS PERLDLL)
    target_link_libraries(perl INTERFACE ${PERL_STATIC_LIB}
        oldnames.lib kernel32.lib user32.lib advapi32.lib
        comctl32.lib ws2_32.lib ucrt.lib vcruntime.lib libcmt.lib)
    target_link_options(perl INTERFACE /NODEFAULTLIB)
else()
    target_compile_definitions(perl INTERFACE PERL_DARWIN PERL_USE_SFE_PUTENV)
    target_link_libraries(perl INTERFACE ${PERL_STATIC_LIB})
    target_link_options(perl INTERFACE -mmacosx-version-min=12.2 -fstack-protector-strong)
endif()

# Copy perl modules at configuration step
set(PERL_MODULES Carp.pm Config.pm DynaLoader.pm Exporter.pm Exporter
    Fcntl.pm integer.pm overload.pm overloading.pm overload strict.pm
    Time vars.pm warnings.pm warnings XSLoader.pm)
list(TRANSFORM PERL_MODULES PREPEND ${PERL_PREFIX}/lib/)
list(APPEND PERL_MODULES exiftool/lib/File exiftool/lib/Image)
file(COPY ${PERL_MODULES} DESTINATION fatlib)

# Pack perl sources into object file
add_executable(bin2obj bin2obj.c)
set(FATPACKER ${CMAKE_CURRENT_SOURCE_DIR}/FatPacker.pm)
set(LOADER ${CMAKE_CURRENT_SOURCE_DIR}/loader.pl)
add_custom_command(OUTPUT packed.obj
    COMMAND ${PERL_PREFIX}/perl ${FATPACKER} ${LOADER} > packed.pl
    COMMAND bin2obj packed.pl packed.obj
    DEPENDS perl bin2obj ${FATPACKER} ${LOADER})
set_source_files_properties(packed.obj PROPERTIES EXTERNAL_OBJECT true GENERATED true)

# Build exiftool-dll C interface
add_library(exiftool SHARED exiftool.h exiftool.c packed.obj)
target_link_libraries(exiftool PRIVATE perl)
target_compile_definitions(exiftool PRIVATE EXIFTOOL_EXPORTS)

add_executable(test test.c)
target_link_libraries(test PRIVATE exiftool)
