/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef BMFONT
#define BMFONT

#include <vector>
#include <map>

#include "simple_fs.hpp"

#ifndef MAKE_RGBA

#define MAKE_RGBA(r,g,b,a)  (r | (g << 8) | (b << 16) | (a << 24))

#endif

#ifndef GET_BLUE

#define GET_BLUE(rgba) (( (rgba)>>16 ) & 0xff )
#endif

#ifndef GET_GREEN

#define GET_GREEN(rgba) (( (rgba)>>8 ) & 0xff )
#endif

#ifndef GET_RED

#define GET_RED(rgba) ( rgba & 0xff )
#endif

#ifndef GET_ALPHA

#define GET_ALPHA(rgba) (( (rgba)>>24 ) & 0xff)
#endif

class KearningInfo {

public:
	short First;
	short Second;
	short Amount;

	KearningInfo() : First(0), Second(0), Amount(0) { }
};


class CharDescriptor {

public:
	short x, y;
	short Width;
	short Height;
	short XOffset;
	short YOffset;
	short XAdvance;
	short Page;

	CharDescriptor() : x(0), y(0), Width(0), Height(0), XOffset(0), YOffset(0),
		XAdvance(0), Page(0) { }
};

class BMFont {

public:

	void LoadFontImage(native_string file);
	bool LoadFontfile(native_string file);
	GLuint ftexid = 0;
	void SetColor(int r, int g, int b, int a) {
		fcolor = MAKE_RGBA(r, g, b, a);
	}
	void SetBlend(int b) {
		fblend = b;
	}
	void SetScale(float scale) {
		fscale = scale;
	}
	float GetHeight() {
		return LineHeight * fscale;
	}
	void Print(float, float, const char*, uint32_t*, sys::state&, ...);
	BMFont() {
		SetColor(255, 255, 255, 255);
		KernCount = 0;
		ftexid = GLuint(0);
		fblend = 0;
		fscale = 1.0;
	};
	BMFont(BMFont&& src) {
		ftexid = src.ftexid;
		imagefile = src.imagefile;
		fontfile = src.fontfile;
		loadingdone = src.loadingdone;
		SetColor(255, 255, 255, 255);
		fblend = 0;
		fscale = 1.0;

		Chars = src.Chars;
		Kearn = src.Kearn;

		KernCount = src.KernCount;

		src.ftexid = 0;
	}
	~BMFont();

private:
	short LineHeight;
	short Base;
	short Width;
	short Height;
	short Pages;
	short Outline;
	short KernCount;
	std::map<int, CharDescriptor> Chars;
	std::vector<KearningInfo> Kearn;
	int fcolor;
	native_string imagefile;
	native_string fontfile;
	bool loadingdone = false;
	float fscale;
	int fblend;

	bool ParseFont(native_string file, sys::state&);
	int GetKerningPair(int, int);
	float GetStringWidth(const char*);

};


#endif
