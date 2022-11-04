git clone -b 12.44 --depth 1 https://github.com/exiftool/exiftool.git
cd ..

shopt -s globstar
rm -rf fatlib
cp -r exiftool/lib fatlib
cp -r perl5/lib/Exporter* fatlib
cp -r perl5/lib/overload* fatlib
cp -r perl5/lib/Time fatlib
cp -r perl5/lib/Win32* fatlib
cp -r perl5/lib/warnings* fatlib
cp perl5/lib/Carp.pm fatlib
cp perl5/lib/Config.pm fatlib
cp perl5/lib/DynaLoader.pm fatlib
cp perl5/lib/Fcntl.pm fatlib
cp perl5/lib/integer.pm fatlib
cp perl5/lib/strict.pm fatlib
cp perl5/lib/vars.pm fatlib
cp perl5/lib/XSLoader.pm fatlib
chmod -R 777 fatlib

perl5/perl.exe FatPacker.pm file loader.pl >packed.pl
wsl objcopy --input-target binary --output-target pe-x86-64 \
    --binary-architecture i386 packed.pl packed.obj

mkdir -p build
cd build
cl -LD -Z7 -W3 -DEXIFTOOL_EXPORTS -DMULTIPLICITY -DPERL_IMPLICIT_SYS -DPERLDLL \
    ../exiftool.c -I../perl5/lib/CORE -link ../packed.obj ../perl5/lib/CORE/perl536s.lib -nodefaultlib \
    oldnames.lib kernel32.lib user32.lib advapi32.lib ucrt.lib ws2_32.lib vcruntime.lib comctl32.lib libcmt.lib
cl -Z7 -W3 ../test.c exiftool.lib
test.exe
