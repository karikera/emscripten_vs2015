#pragma once


#ifdef CBSEASYV8_EXPORTS
#define CBS_EASYV8_DLLEXPORT __declspec(dllexport)
#else
#define CBS_EASYV8_DLLEXPORT __declspec(dllimport)
#endif

namespace kr
{

	CBS_EASYV8_DLLEXPORT void * CT_FASTCALL ezv8_malloc(size_t sz);
	CBS_EASYV8_DLLEXPORT void CT_FASTCALL ezv8_free(void * ptr);

}