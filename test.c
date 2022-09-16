#include "exiftool.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) puts("Usage: ./test <filename>");
    exiftool_t tool = exiftool_Create();
    const char *type = exiftool_GetFileType(tool, argv[1]);
    printf("File has type %s", type);

    int ok = exiftool_ExtractInfo(tool, argv[1], NULL);
    printf("ExtractInfo return %d", ok);

    exifdata_t tags = exifdata_CreateList(tool);
    exifdata_t tag0 = exifdata_CreateString(tool, "Error");
    exifdata_t tag1 = exifdata_CreateString(tool, "Warning");
    exifdata_Append(tool, tags, tag0);
    exifdata_Append(tool, tags, tag1);

    exifdata_t info = exiftool_GetInfo(tool, tags);

    exifdata_Destroy(tool, tags);
    exifdata_Destroy(tool, tag0);
    exifdata_Destroy(tool, tag1);

    exifdata_t foundTags = exiftool_GetTagList(tool, NULL);

    for (int i = 0; i < exifdata_Length(tool, foundTags); ++i) {
        exifdata_t key = exifdata_Item(tool, foundTags, i);
        const char *keyname = exifdata_String(tool, key);
        exifdata_t val = exifdata_Value(tool, info, keyname);
        const char *valname = exifdata_String(tool, val);

        printf("Found %s : %s", keyname, valname);
        exifdata_Destroy(tool, key);
        exifdata_Destroy(tool, val);
    }

    exifdata_Destroy(tool, foundTags);
    exifdata_Destroy(tool, info);

    exiftool_Destroy(tool);
    return 0;
}
