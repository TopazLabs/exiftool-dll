#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

/* adapted from mach-o/loader.h */
struct mach_header {
    uint32_t magic;
    int32_t cputype;
    int32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};

struct mach_segment {
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct mach_section {
    char sectname[16];
    char segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved[3];
};

struct mach_symtab {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t symoff;
    uint32_t nsyms;
    uint32_t stroff;
    uint32_t strsize;
};

struct mach_nlist {
    uint32_t n_strx;
    uint8_t n_type;
    uint8_t n_sect;
    uint16_t n_desc;
    uint64_t n_value;
};

static void write_mach(FILE *out, char *name, FILE *embed, uint32_t size) {
    /* calculate length of commands */
    uint32_t head_len = sizeof(struct mach_header);
    uint32_t cmd0_len = sizeof(struct mach_segment) + sizeof(struct mach_section);
    uint32_t cmd1_len = sizeof(struct mach_symtab);
    uint32_t name_len = (uint32_t)strlen(name) + 3;
    uint32_t strs_len = name_len * 2 + 8;

    uint32_t emb_offset = head_len + cmd0_len + cmd1_len;
    uint32_t sym_offset = emb_offset + sizeof(size) + size;
    uint32_t str_offset = sym_offset + sizeof(struct mach_nlist) * 2;

    /* write out header */
    struct mach_header header = {0};
    header.magic = 0xfeedfacf;
#ifdef __arm64__
    header.cputype = 0x0100000c;
#else
    header.cputype = 0x01000007;
    header.cpusubtype = 0x03;
#endif
    header.filetype = 1;
    header.ncmds = 2;
    header.sizeofcmds = cmd0_len + cmd1_len;
    fwrite(&header, sizeof(header), 1, out);

    /* map segment with section of given size */
    struct mach_segment segment = {0};
    segment.cmd = 0x19;
    segment.cmdsize = cmd0_len;
    segment.vmsize = sizeof(size) + size;
    segment.fileoff = emb_offset;
    segment.filesize = sizeof(size) + size;
    segment.maxprot = segment.initprot = 7;
    segment.nsects = 1;
    fwrite(&segment, sizeof(segment), 1, out);

    struct mach_section section = {0};
    strcpy(section.sectname, "__const");
    strcpy(section.segname, "__TEXT");
    section.size = sizeof(size) + size;
    section.offset = emb_offset;
    fwrite(&section, sizeof(section), 1, out);

    /* declare symbol table */
    struct mach_symtab symtab = {0};
    symtab.cmd = 0x02;
    symtab.cmdsize = cmd1_len;
    symtab.symoff = sym_offset;
    symtab.nsyms = 2;
    symtab.stroff = str_offset;
    symtab.strsize = strs_len;
    fwrite(&symtab, sizeof(symtab), 1, out);

    /* write out embed data */
    fwrite(&size, sizeof(size), 1, out);
    for (uint32_t i = 0; i < size - 1; ++i)
        fputc(fgetc(embed), out);
    fputc('\0', out);

    /* write out symbol table */
    struct mach_nlist nlist = {0};
    nlist.n_type = 0x0f;
    nlist.n_sect = 1;
    fwrite(&nlist, sizeof(nlist), 1, out);
    nlist.n_strx = name_len + 5;
    nlist.n_value = sizeof(size);
    fwrite(&nlist, sizeof(nlist), 1, out);

    /* write out string table */
    char *strtab = calloc(1, strs_len);
    sprintf(strtab, "__%s_size", name);
    sprintf(strtab + nlist.n_strx, "__%s", name);
    fwrite(strtab, strs_len, 1, out);
    free(strtab);
}

/* adapted from filehdr.h */
struct coff_header {
    uint16_t magic;
    uint16_t nscns;
    uint32_t timdat;
    uint32_t symptr;
    uint32_t nsyms;
    uint16_t optdhr;
    uint16_t flags;
};

struct coff_section {
    char name[8];
    uint32_t paddr;
    uint32_t vaddr;
    uint32_t size;
    uint32_t scnptr;
    uint32_t relptr;
    uint32_t lnnoptr;
    uint16_t nreloc;
    uint16_t nlnno;
    uint32_t flags;
};

struct coff_symtab {
    uint32_t reserved;
    uint32_t nameptr;
    uint32_t value;
    uint16_t scnum;
    uint16_t type;
    char sclass;
    char numaux;
};

static void write_coff(FILE *out, char *name, FILE *embed, uint32_t size) {
    /* calculate length of headers */
    uint32_t name_len = (uint32_t)strlen(name) + 2;
    uint32_t strs_len = 4 + ((name_len * 2 + 8) & ~3);
    uint32_t sym_offset = sizeof(struct coff_header) + sizeof(struct coff_section);
    uint32_t emb_offset = sym_offset + sizeof(struct coff_symtab) * 2 + strs_len;

    /* write header */
    struct coff_header header = {0};
    header.nscns = 1;
    header.symptr = sym_offset;
    header.nsyms = 2;
    header.flags = 0x04;
    fwrite(&header, sizeof(header), 1, out);

    /* map section of given size */
    struct coff_section section = {0};
    strcpy(section.name, ".rdata");
    section.size = sizeof(size) + size;
    section.scnptr = emb_offset;
    section.flags = 0x40300040;
    fwrite(&section, sizeof(section), 1, out);

    /* create symbol table */
    struct coff_symtab symtab = {0};
    symtab.nameptr = sizeof(strs_len);
    symtab.scnum = 1;
    symtab.sclass = 0x02;
    fwrite(&symtab, sizeof(symtab), 1, out);
    symtab.nameptr += name_len + 5;
    symtab.value = sizeof(size);
    fwrite(&symtab, sizeof(symtab), 1, out);

    /* write out string table */
    char *strtab = calloc(1, strs_len);
    memcpy(strtab, &strs_len, sizeof(strs_len));
    sprintf(strtab + sizeof(strs_len), "_%s_size", name);
    sprintf(strtab + sizeof(strs_len) + name_len + 5, "_%s", name);
    fwrite(strtab, strs_len, 1, out);
    free(strtab);

    /* write out embed data */
    fwrite(&size, sizeof(size), 1, out);
    for (uint32_t i = 0; i < size - 1; ++i)
        fputc(fgetc(embed), out);
    fputc('\0', out);
}

int main(int argc, char *argv[]) {
    /* check input file exists */
    if (argc != 3) return puts("Usage: bin2obj <in> <out>"), 1;
    FILE *embed = fopen(argv[1], "rb");
    if (!embed) return puts("Can't open input"), 1;
    FILE *out = fopen(argv[2], "wb+");
    if(!out) return puts("Can't open out"), 1;

    /* calculate file size */
    fseek(embed, 0, SEEK_END);
    uint32_t size = ftell(embed) + 1;
    fseek(embed, 0, SEEK_SET);

    /* symbol name from filename */
    char *name = strrchr(argv[1], '/');
    name = name ? name + 1 : argv[1];
    for (char *p = name; *p; ++p) {
        if (*p >= 'a' && *p <= 'z') continue;
        if (*p >= 'A' && *p <= 'Z') continue;
        if (*p >= '0' && *p <= '9') continue;
        *p = '_';
    }

    /* out to object file */
#ifdef _WIN32
    write_coff(out, name, embed, size);
#else
    write_mach(out, name, embed, size);
#endif
}
