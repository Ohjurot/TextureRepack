#pragma once

#ifdef TEXRP_BUILD
#define TEXRP_INTERFACE __declspec(dllexport)
#else
#define TEXRP_INTERFACE __declspec(dllimport)
#endif

#define TexRPAllocate(ttype, type, ...) (ttype*)new type(__VA_ARGS__)
#define TexRPDestroy(ptrObject) delete ptrObject

class ITexRPObject {
	public:
		virtual ~ITexRPObject() {};
};
