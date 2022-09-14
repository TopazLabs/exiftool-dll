#include "exiftool.h"

int main(int argc, char *argv[]) {
    exiftool_t tool = exiftool_init();
    exiftool_GetFileType(tool);
    exiftool_free(tool);
    return 0;
}
