/*
BASIC BMFont example implementation with Kerning, for C++ and OpenGL 2.0

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
--------------------------------------------------------------------------------
These editors can be used to generate BMFonts:
 • http://www.angelcode.com/products/bmfont/ (free, windows)
 • http://glyphdesigner.71squared.com/ (commercial, mac os x)
 • http://www.n4te.com/hiero/hiero.jnlp (free, java, multiplatform)
 • http://slick.cokeandcode.com/demos/hiero.jnlp (free, java, multiplatform)

Some code below based on code snippets from this gamedev posting:

http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/

Although I'm giving this away, I'd appreciate an email with fixes or better code!

aaedev@gmail.com 2012
*/
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include "bmfont.h"
#include <stdarg.h>

#include "texture.hpp"

#pragma warning (disable : 4996 )

//Todo: Add buffer overflow checking.

#define MAX_BUFFER 256

// This structure holds the vertices for rendering with glDrawArrays
typedef struct 
{
	float x,y;
    float texx,texy;
    GLubyte r,g,b,a;
} vlist;                      

vlist texlst[2048*4];

bool BMFont::ParseFont(simple_fs::file& file)
{
	auto content = simple_fs::view_contents(file);

	std::stringstream Stream(std::string(content.data, content.file_size));
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;

	KearningInfo K;
	CharDescriptor C;

	while( !Stream.eof() )
	{
		std::stringstream LineStream;
		std::getline( Stream, Line );
		LineStream << Line;

		//read the line's type
		LineStream >> Read;
		if( Read == "common" )
		{
			//this holds common data
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "lineHeight" )
				{Converter >> LineHeight;}
				else if( Key == "base" )
				{Converter >> Base;}
				else if( Key == "scaleW" )
				{Converter >> Width;}
				else if( Key == "scaleH" )
				{Converter >> Height;}
				else if( Key == "pages" )
				{Converter >> Pages;}
				else if( Key == "outline" )
				{Converter >> Outline;}
			}
		}
		
		else if( Read == "char" )
		{
			//This is data for each specific character.
			int CharID = 0;
	
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//Assign the correct value
				Converter << Value;
				if( Key == "id" )
				{Converter >> CharID;}
				else if( Key == "x" )
				{	Converter >> C.x;}      
				else if( Key == "y" )
				{	Converter >> C.y;}      
				else if( Key == "width" )
				{	Converter >> C.Width;}        
				else if( Key == "height" )
				{	Converter >> C.Height;}         
				else if( Key == "xoffset" )
				{	Converter >> C.XOffset;}         
				else if( Key == "yoffset" )
				{	Converter >> C.YOffset;}        
				else if( Key == "xadvance" )
				{	Converter >> C.XAdvance;}         
				else if( Key == "page" )
				{	Converter >> C.Page;}           
			}
			
         	Chars.insert(std::map<int,CharDescriptor>::value_type( CharID,C ));
			
		}
		else if( Read == "kernings" )
		{
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "count" )
				     {Converter >> KernCount; }
			}
		}

		else if( Read == "kerning" )
		{
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "first" )
				{Converter >> K.First;}
				
				else if( Key == "second" )
				{Converter >> K.Second;  }
				
				else if( Key == "amount" )
				{Converter >> K.Amount;}
 			}
			//wrlog("Done with this pass");
			Kearn.push_back(K);
		}
	}

	return true;
}


int BMFont::GetKerningPair(int first, int second)
{
		
	 if (KernCount ) //Only process if there actually is kerning information
	 {
	 //Kearning is checked for every character processed. This is expensive in terms of processing time.
	 	 for (int j = 0; j < KernCount;  j++ )
		 {
			if (Kearn[j].First == first && Kearn[j].Second == second)
			  { 
				 return Kearn[j].Amount;
			      //wrlog("Kerning Pair Found!!!");
				 // wrlog("FIRST: %d SECOND: %d offset %d",first,second,koffset);
			  }
		 }
	 }

return 0;
}


float BMFont::GetStringWidth(const char *string)
{
  float total=0;
  CharDescriptor  *f;

  for (uint8_t i = 0; i != strlen(string); i++)
   { 
	  f=&Chars[string[i]];
      total+=f->XAdvance;
   }

  return total * fscale;
}

void BMFont::LoadFontImage(simple_fs::file& file) {
	assert(ftexid == 0);

	ftexid = ogl::make_font_texture(file);
}

bool BMFont::LoadFontfile(simple_fs::file& file) {
	ParseFont(file);
	KernCount = (short)Kearn.size();

	return true;
}


void Render_String(int len)
{
   //Draw Text Array, with 2D, two coordinates per vertex.
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(2, GL_FLOAT, sizeof(vlist), &texlst[0].x);

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(vlist), &texlst[0].texx);

   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer (4, GL_UNSIGNED_BYTE , sizeof(vlist) , &texlst[0].r);

   glDrawArrays(GL_QUADS, 0, len*4);// 4 Coordinates for a Quad. Could use DrawElements here instead GL 3.X+ GL_TRIANGLE_STRIP? 
 
   //Finished Drawing, disable client states.
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
}

void use_texture(GLuint* texture, GLboolean linear, GLboolean mipmapping)
{
	GLenum filter = linear ? GL_LINEAR : GL_NEAREST;
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mipmapping)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)filter);
}

void SetBlendMode(int mode)
{
	if (mode) {
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glAlphaFunc(GL_GREATER, 0.5f);
	}
	else {
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void BMFont::Print(float x, float y, const char *fmt, ...)
{
	if(!buffercreated) {
		glGenBuffers(1, &fbufid);
		glBindBuffer(GL_ARRAY_BUFFER, fbufid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texlst), &texlst[0], GL_STATIC_DRAW);
		buffercreated = true;
	}
	
	float CurX = (float) x;
	float CurY = (float) y;
	float DstX = 0.0;
	float DstY = 0.0;
	int Flen;
	
    float adv = (float) 1.0/Width;                      // Font texture atlas spacing. 
	char	text[512] = "";	                            // Holds Our String
	CharDescriptor  *f;									// Pointer to font.
	                                    
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						    // And Converts Symbols To Actual Numbers
	va_end(ap);		

	//Select and enable the font texture. (With mipmapping.)
  	//use_texture(&ftexid, 0,1);
    //Set type of blending to use with this font.
	//SetBlendMode(fblend);
   	//Set Text Color, all one color for now. 
	unsigned char *color = (unsigned char*)&fcolor;
	
	y= y + LineHeight;
    Flen = (int)strlen(text);

	for (int i = 0; i != Flen; ++i)
	{
  
		 f=&Chars[text[i]];

		 CurX = x + f->XOffset;
		 CurY = y - f->YOffset;
		 DstX = CurX + f->Width;
	     DstY = CurY - f->Height;
		 			    
         //0,1 Texture Coord
		 texlst[i*4].texx = adv*f->x;
		 texlst[i*4].texy = (float) 1.0 -(adv * (f->y + f->Height) );
         texlst[i*4].x = (float) fscale * CurX;
		 texlst[i*4].y = (float) fscale * DstY;
		 texlst[i*4].r = color[0];
		 texlst[i*4].g = color[1];
		 texlst[i*4].b = color[2];
		 texlst[i*4].a = color[3];

		 //1,1 Texture Coord
		 texlst[(i*4)+1].texx = adv*(f->x+f->Width);
		 texlst[(i*4)+1].texy = (float) 1.0 -(adv * (f->y + f->Height) );
         texlst[(i*4)+1].x = (float) fscale * DstX;
		 texlst[(i*4)+1].y = (float) fscale * DstY;
		 texlst[(i*4)+1].r = color[0];
		 texlst[(i*4)+1].g = color[1];
		 texlst[(i*4)+1].b = color[2];
		 texlst[(i*4)+1].a = color[3];

		 //1,0 Texture Coord
		 texlst[(i*4)+2].texx = adv*(f->x+f->Width);
		 texlst[(i*4)+2].texy = (float) 1.0 -(adv*f->y);
         texlst[(i*4)+2].x = (float) fscale * DstX;
		 texlst[(i*4)+2].y = (float) fscale * CurY;
		 texlst[(i*4)+2].r = color[0];
		 texlst[(i*4)+2].g = color[1];
		 texlst[(i*4)+2].b = color[2];
		 texlst[(i*4)+2].a = color[3];

		 //0,0 Texture Coord
		 texlst[(i*4)+3].texx = adv*f->x;
		 texlst[(i*4)+3].texy = (float) 1.0 -(adv*f->y);
         texlst[(i*4)+3].x = (float) fscale * CurX;
		 texlst[(i*4)+3].y = (float) fscale * CurY;
		 texlst[(i*4)+3].r = color[0];
		 texlst[(i*4)+3].g = color[1];
		 texlst[(i*4)+3].b = color[2];
		 texlst[(i*4)+3].a = color[3];

         //Only check kerning if there is greater then 1 character and 
		 //if the check character is 1 less then the end of the string.
		 if (Flen > 1 && i < Flen)
		 { 
			 x += GetKerningPair(text[i],text[i+1]);
		 }
		  
		 x +=  f->XAdvance;

		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, ftexid);

		 glBindVertexBuffer(0, fbufid, 0, sizeof(texlst));

		 glUniform4f(ogl::parameters::drawing_rectangle, x, y, 5, 5);
		 glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
   //Render_String((int)strlen(text));

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, ftexid);

	//glBindVertexBuffer(0, fbufid, 0, sizeof(texlst));

	//glUniform4f(ogl::parameters::drawing_rectangle, x, y, 50, 50);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, (int)strlen(text)*4);
}


void BMFont::PrintCenter( float y, const char *string)
{
	int x=0;
	CharDescriptor  *f;		 
	
	int window_width = 500;

		int len = (int)strlen(string);

		for (int i = 0; i != len; ++i)
		{
			f=&Chars[string[i]];

			if (len > 1 && i < len)
			 { 
			   x += GetKerningPair(string[i],string[i+1]);
			 }
			 x +=  f->XAdvance;
		}

	Print( (float)(500/2) - (x/2) , y, string);
}


BMFont::~BMFont()
{
	Chars.clear();
	Kearn.clear();
	if(ftexid)
		glDeleteTextures(1, &ftexid);
}
