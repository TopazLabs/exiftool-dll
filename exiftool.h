#ifdef EXIFTOOL_EXPORTS
#    define DLLEXPORT __declspec(dllexport)
#else
#    define DLLEXPORT __declspec(dllimport)
#endif

// exifdata_t like json11::Json()??
typedef struct asdf *exifdata_t;
typedef struct interpreter *exiftool_t;

// creates new perl interpreter with exiftool
DLLEXPORT exiftool_t exiftool_init(void);

// freed perl interpreter created by exiftool_init
DLLEXPORT void exiftool_free(exiftool_t tool);

DLLEXPORT int exiftool_ExtractInfo(exiftool_t tool, const char *filename, exifdata_t tags);

DLLEXPORT exifdata_t exiftool_GetInfo(exiftool_t tool);

DLLEXPORT int exiftool_WriteInfo(exiftool_t tool, const char *filename);

DLLEXPORT int exiftool_SetNewValue(exiftool_t tool, 
DLLEXPORT exifdata_t exiftool_SetNewValuesFromFile(exiftool_t tool, 


DLLEXPORT const char *exiftool_GetValue(exiftool_t tool, const char *tag);

DLLEXPORT int exiftool_SaveNewValues(exiftool_t tool);
DLLEXPORT void exiftool_RestoreNewValues(exiftool_t tool);

// outputs scalar context, coerced to string
//DLLEXPORT const char *exiftool_GetTagID(exiftool_t tool, const char *tag);
//DLLEXPORT const char *exiftool_GetDescription(exiftool_t tool, const char *tag);
//DLLEXPORT const char *exiftool_GetGroup(exiftool_t tool, const char *tag);

//DLLEXPORT const char *exiftool_GetTagName(exiftool_t tool const char *tag);
DLLEXPORT const char *exiftool_GetFileType(exiftool_t tool, const char *filename);
//DLLEXPORT int exiftool_CanWrite(exiftool_t tool, const char *filename);
//DLLEXPORT int exiftool_CanCreate(exiftool_t tool, const char *filename);

// optional arguments are omitted
//
//

// simple wrapper over Perl dynamic types
#define EXIFDATA_DOUBLE 0
#define EXIFDATA_STRING 1
#define EXIFDATA_LIST 2
#define EXIFDATA_HASH 3

DLLEXPORT exifdata_t exifdata_InitNumber(double num);
DLLEXPORT exifdata_t exifdata_InitString(const char *str);
DLLEXPORT exifdata_t exifdata_InitList();
DLLEXPORT exifdata_t exifdata_InitHash();

DLLEXPORT int exifdata_Type(exifdata_t data);
DLLEXPORT double exifdata_Number(exifdata_t data);
DLLEXPORT const char *exifdata_String(exifdata_t data);

// requires type to be list
DLLEXPORT int exifdata_Length(exifdata_t data);
DLLEXPORT exifdata_t exifdata_Item(int idx);
DLLEXPORT void exifdata_Append(exifdata_t data, exifdata_t item);

DLLEXPORT const char *exifdata_Value(exifdata_t data, const char *key);
DLLEXPORT void exifdata_Set(exifdata_t data, const char *key, exifdata_t value);

DLLEXPORT void exifdata_Free(exifdata_t data);


exifdata_t tags = exifdata_InitHash();
exifdata_Set(tags, "Artist", exifdata_InitString("CQCumbers"));
exiftool_SetNewValue(tool, tags);
exifdata_Free(tags);

exiftool_ExtractInfo(tool, "test-coach.jpg", exifdata_InitNull());
exifdata_t tagInfo = exiftool_GetInfo(tool);
exifdata_t tagList = exiftool_GetFoundTags(tool);
for (int i = 0; i < exifdata_Length(tagList); ++i) {
    const char *key = exifdata_String(exifdata_Item(tagList, i));
    const char *value = exifdata_String(exifdata_Value(tagInfo, key));
    printf("%s: %s", key, value);
}
exifdata_Free(tagInfo);
exifdata_Free(tagList);
