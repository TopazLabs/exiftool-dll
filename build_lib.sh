git clone -b 12.44 --depth 1 https://github.com/exiftool/exiftool.git
git clone -b 20220613.02 --depth 1 https://github.com/perltidy/perltidy.git
cd perltidy && ../perl5/perl.exe pm2pl -o ../perltidy.pl
cd ..

shopt -s globstar
rm -rf fatlib
cp -r exiftool/lib fatlib
cp -r perl5/lib/* fatlib
chmod -R 777 fatlib
perl5/perl.exe perltidy.pl -b -bext=// -dac fatlib/*.pm fatlib/**/*.pm

perl5/perl.exe FatPacker.pm file hello_world.pl >packed.pl
wsl objcopy --input-target binary --output-target pe-x86-64 \
    --binary-architecture i386 packed.pl packed.obj

mkdir -p build
cd build
cl -LD -Z7 -W3 -DEXIFTOOL_EXPORTS -DMULTIPLICITY -DPERL_IMPLICIT_SYS -DPERLDLL \
    ../exiftool.c -I../perl5/lib/CORE -link ../packed.obj ../perl5/lib/CORE/perl536s.lib -nodefaultlib \
    oldnames.lib kernel32.lib user32.lib advapi32.lib ucrt.lib ws2_32.lib vcruntime.lib comctl32.lib libcmt.lib
cl -Z7 -W3 ../test.c exiftool.lib
test.exe
