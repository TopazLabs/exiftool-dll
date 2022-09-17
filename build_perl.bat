git clone -b v5.36.0 --depth 1 https://github.com/Perl/perl5.git
cd perl5\win32
nmake CCTYPE=MSVC142 BUILD_STATIC=define
