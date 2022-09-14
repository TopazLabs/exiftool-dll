#shopt -s globstar
#rm -rf fatlib
#cp -r ~/Downloads/exiftool-master/lib fatlib
#cp -r perl5/lib/* fatlib
#/d/Documents/.staticperl/perl/bin/perl /d/Documents/.staticperl/perl/bin/perltidy \
#    -b -bext=// -dac fatlib/*.pm fatlib/**/*.pm

perl FatPacker.pm file hello_world.pl >packed.pl
xxd -i packed.pl >prog.h

cl -LD -Z7 -W3 -DEXIFTOOL_EXPORTS -DMULTIPLICITY -DPERL_IMPLICIT_SYS -DPERLDLL \
    exiftool.c -Fo build -Iperl5/lib/CORE -link perl5/lib/CORE/perl536s.lib -nodefaultlib \
    oldnames.lib kernel32.lib user32.lib advapi32.lib ucrt.lib ws2_32.lib vcruntime.lib comctl32.lib libcmt.lib
cl -Z7 -W3 -Fo build test.c exiftool.lib
./build/test.exe
