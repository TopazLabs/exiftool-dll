#pragma once

#ifdef EXIFTOOL_EXPORTS
#    define DLLEXPORT __declspec(dllexport)
#else
#    define DLLEXPORT __declspec(dllimport)
#endif

typedef struct sv *exifdata_t;
typedef struct interpreter *exiftool_t;

/* for testing */
DLLEXPORT const char *exiftool_GetFileType(exiftool_t tool, const char *filename);

/* Simplified subset of exiftool methods */
DLLEXPORT exiftool_t exiftool_Create(void);
DLLEXPORT void exiftool_Destroy(exiftool_t tool);

DLLEXPORT void exiftool_Options(exiftool_t tool, exifdata_t options);
DLLEXPORT void exiftool_ClearOptions(exiftool_t tool);

DLLEXPORT int exiftool_ExtractInfo(exiftool_t tool, const char *filename, exifdata_t tags);
DLLEXPORT exifdata_t exiftool_GetInfo(exiftool_t tool, exifdata_t tags);
DLLEXPORT exifdata_t exiftool_GetTagList(exiftool_t tool, exifdata_t info);
DLLEXPORT exifdata_t exiftool_GetValue(exiftool_t tool, const char *tagname);

DLLEXPORT int exiftool_WriteInfo(exiftool_t tool, const char *filename);
DLLEXPORT int exiftool_SetNewValue(exiftool_t tool, const char *tagname, exifdata_t value);
DLLEXPORT exifdata_t exiftool_GetNewValue(exiftool_t tool, const char *tagname);

DLLEXPORT void exiftool_SetNewValuesFromFile(exiftool_t tool, const char *filename, exifdata_t tags);
DLLEXPORT int exiftool_CountNewValues(exiftool_t tool);
DLLEXPORT int exiftool_SaveNewValues(exiftool_t tool);
DLLEXPORT void exiftool_RestoreNewValues(exiftool_t tool);

DLLEXPORT exifdata_t exiftool_GetTagID(exiftool_t tool, const char *tagname);
DLLEXPORT exifdata_t exiftool_GetDescription(exiftool_t tool, const char *tagname);
DLLEXPORT exifdata_t exiftool_GetGroup(exiftool_t tool, const char *tagname);

/* Wrapper for perl scalar variables */
DLLEXPORT void exifdata_Destroy(exiftool_t tool, exifdata_t data);
DLLEXPORT exifdata_t exifdata_CreateUndef(exiftool_t tool);
DLLEXPORT int exifdata_Type(exifdata_t data);

DLLEXPORT exifdata_t exifdata_CreateNumber(exiftool_t tool, double num);
DLLEXPORT double exifdata_Number(exiftool_t tool, exifdata_t data);
DLLEXPORT exifdata_t exifdata_CreateString(exiftool_t tool, const char *str);
DLLEXPORT const char *exifdata_String(exiftool_t tool, exifdata_t data);

DLLEXPORT exifdata_t exifdata_CreateList(exiftool_t tool);
DLLEXPORT int exifdata_Length(exiftool_t tool, exifdata_t data);
DLLEXPORT exifdata_t exifdata_Item(exiftool_t tool, exifdata_t data, int idx);
DLLEXPORT void exifdata_Append(exiftool_t tool, exifdata_t data, exifdata_t item);

DLLEXPORT exifdata_t exifdata_CreateHash(exiftool_t tool);
DLLEXPORT exifdata_t exifdata_Value(exiftool_t tool, exifdata_t data, const char *key);
DLLEXPORT void exifdata_Set(exiftool_t tool, exifdata_t data, const char *key, exifdata_t value);
