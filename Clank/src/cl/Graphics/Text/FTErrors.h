#include "cl/types.h"

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };

const struct 
{
	s32          code;
	const char*  message;
} FT_Errors[] =
#include <freetype/fterrors.h>
