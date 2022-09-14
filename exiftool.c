#include "exiftool.h"
#define _CRT_NONSTDC_NO_DEPRECATE
#include <string.h>

#include <EXTERN.h>
#include <perl.h>
#include "prog.h"

DLLEXPORT exiftool_t exiftool_init(void) {
    int argc = 3;
    char *argv[] = { "", "-e", "0" };

    static int sys_init = 0;
    if (!sys_init) {
        PERL_SYS_INIT(&argc, (char***)&argv);
	sys_init = 1;
    }

    exiftool_t tool = perl_alloc();
    dTHXa(tool);
    PERL_SET_CONTEXT(tool);
    perl_construct(tool);
    perl_parse(tool, NULL, argc, argv, NULL);

    SV *code = newSVpv((char*)packed_pl, packed_pl_len);
    sv_2mortal(code);
    eval_sv(code, G_SCALAR | G_RETHROW);
    return tool;
}

DLLEXPORT void exiftool_free(exiftool_t tool) {
    PERL_SET_CONTEXT(tool);

    perl_destruct(tool);
    perl_free(tool);
}

DLLEXPORT

DLLEXPORT const char *exiftool_GetFileType(exiftool_t tool, const char *filename) {
    dTHXa(tool);
    PERL_SET_CONTEXT(tool);
    dSP;

    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(get_sv("className", 0));
    SV *file = newSVpv(filename, 0);
    PUSHs(file);
    PUTBACK;

    call_method("GetFileType", G_SCALAR);
    SPAGAIN;
    char *type = strdup(SvPVx_nolen(POPs));
    SvREFCNT_dec(file);
    PUTBACK;
    return type;
}
