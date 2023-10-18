# OpenGLText: Efficient and Simple GL Text Rendering
## fork by [Rama Karl](http://ramakarl.com) for Windows
## original by Tristan Lorach, based on GLText 0.3.1 and Freetype2

This fork is for build convenience on Windows.<br>
OpenGLText and bakeFonts provide a simple utility to render font atlases for efficient OpenGL rendering.

Changes in this fork:<br>
- A copy of the correct versions of Freetype 2.3.5 and GLText 0.3.1 are distributed here for convenience, along with their GPLv2 licenses.
- Glut is also provided here for build convenience. 
- The patches by Tristan for GLText which implement baking have been applied. The GLText here is patched.
- A binary for **bakeFonts.exe** on Win64 is provided for ease of use.
- A VisualStudio 2019 solution for Freetype (x64) can be found in: F:\OpenGLText-baker\freetype-2.3.5\builds\win32\visualc
- A VisualStudio 2019 solution for bakeFonts (x64) which directly builds GLText 0.3.1 can be found in: 
F:\OpenGLText-baker\bakeFonts\

## Baking Fonts

A binary for **bakeFonts.exe** on Win64 is provided for ease of use.<br>
Fonts can be baked to .tga and .bin (glyph info) with the bakeFonts.exe binary:<br>
```
> bakeFonts {font_name}.ttf {size}
```
```
> bakeFonts arial.ttf 128
```
Example TrueType fonts can be found in \fonts<br>

## Rendering 

Notes on rendering:
- You don't strictly need OpenGLText in order to render the baked fonts.
- All you need is to be able to read the .bin and .tga files.
- Then perform your own OpenGL rendering as you like.
- The file \gltext-0.3.1\src\AbstractRender.cpp contains the GlyphInfo and FileHeader structures which are written to the .bin file. 
- The same file shows the saveFonts function, which shows the FileHeader includes all 256 glyphs, could be read in as a single fread.
- The .tga can be converted to any desired image format and loaded as a GL texture.

Here is the code I commonly use for rendering fonts:

```
void drawText ( Vec2F a, char* msg, Vec4F clr )
{
	int len = (int) strlen ( msg );
	if ( len == 0 ) 
		return;

	gxVert* v = ExpandBuffer ( len*6, &gx.m_font_img );

	// get current font
	gxFont& font = gx.getCurrFont ();	
	int glyphHeight = font.ascent + font.descent + font.linegap;
	float lX = a.x;
	float lY = a.y;
	float lLinePosX = a.x;
	float lLinePosY = a.y;
	const char* c = msg;
	int cnt = 0;
	
	// text_hgt = desired height of font in pixels
	// text_kern = spacing between letters

	float textSz = gx.m_text_hgt / glyphHeight; // glyph scale
	float textStartPy = textSz; // start location in pixels
	
	while (*c != '\0' && cnt < len ) {
	   if ( *c == '\n' ) {
		lX = lLinePosX;
		lLinePosY += gx.m_text_hgt;
		lY = lLinePosY;
	   } else if ( *c >=0 && *c <= 128 ) {
		gxGlyph& gly = font.glyphs[*c];
		float pX = lX + gly.offX * textSz;
		float pY = lY + gly.offY * textSz; 
		float pW = gly.width * textSz;
		float pH = gly.height * textSz;

                // triangle strip:
		// repeat first point (jump), zero alpha
		v->x = pX; v->y = pY+pH; v->z = 0; vclr(v,clr, 0); v->tx = gly.u; v->ty = gly.v; v++;
		// four corners of glyph
		v->x = pX; v->y = pY+pH; v->z = 0; vclr(v,clr, 1); v->tx = gly.u; v->ty = gly.v; v++;
		v->x = pX; v->y = pY ; v->z = 0; vclr(v,clr, 1); v->tx = gly.u; v->ty = gly.v + gly.dv;	v++;			
		v->x = pX+pW; v->y = pY+pH; v->z = 0; vclr(v,clr, 1); v->tx = gly.u + gly.du; v->ty = gly.v; v++;			
		v->x = pX+pW; v->y = pY; v->z = 0; vclr(v,clr, 1); v->tx = gly.u + gly.du; v->ty = gly.v + gly.dv; v++;
		// repeat last point (jump), zero alpha
		v->x = pX+pW; v->y = pY; v->z = 0; vclr(v,clr, 0); v->tx = gly.u + gly.du; v->ty = gly.v + gly.dv; v++;

		lX += (gly.advance + gx.m_text_kern) * textSz;
		lY += 0;
		cnt++;
	}
	c++;
	}
```

- The function ExpandBuffer provides CPU memory for a triangle strip with space for 6 vertices per glyph (character), this memory can also be committed to the GPU as an OpenGL VBO.
- By allowing the buffer to expand, one can pack many calls to drawText into the **same** buffer.
- The vertices, and their texture coords, are then streamed into the buffer.
- This example uses GL_TRIANGLE_STRIP with two extra, transparent vertices per glyph. One could also use a regular GL_TRIANGLES buffer.
- With the VBO in memory, a shader selects a pixel from the GL texture using the provided texture coordinates, and writes a pixel with alpha blending and color if desired.
- The resulting, single VBO, can be rendered very efficiently with a single draw call.

## License Info

- If you write your own renderer, you can use any license you like.
- As the rendering example above demonstrates one can write a simple, independent GL text render for these baked .bin/.tga 
- It is not necessary to link OpenGLText, GLText or Freetype2 to your own game or application.
- You only need these libraries if you plan to rebuild bakeFonts or OpenGLText:
- Freetype2 is BSD or GPLv2 licensed. This work uses the GPLv2 license.
- GLText is GLPv2 licensed.
- bakeFonts is GPLv2 licensed (as a patch, 95% of its code is GLText)

## Building

Building:
- Review Tristan's README.md first (below)
- Patches have already been applied here.
- Build freetype-2.3.5 from VS2019 solution provided.
- Build bakeFonts from VS2019 solution provided, this also builds gltext-0.3.1
- Freetype2 must be built and linked with FREETYPE2_STATIC define,
specified in both Freetype2.sln and bakeFonts.sln
- Here are the important VS2019 options:<br>
Working Directory: $(ProjectDir)/..  launch from top-level<br>
Additional Includes: <br>
  \OpenGLText\freetype-2.3.5\include\;<br>
  \OpenGLText\gltext-0.3.1\src<br>
Preprocessor Defs: <br>
  FREETYPE2_STATIC;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)<br>
Linker Inputs:<br>
  opengl32.lib;<br>
  glu32.lib;<br>
  F:\OpenGLText\freetype-2.3.5\objs\freetype235_D.lib;<br>
  F:\OpenGLText\glut\glut64.lib; <br>
  %(AdditionalDependencies)<br>
- It is recommended to link Freetype 2.3.5 as a **static library**.<br>
- This is done using the FREETYPE2_STATIC.<br>
- Sorry I am unable to provide a CMake at this time. Others are welcome to do so.

------------------------------
## Tristan's ORIGINAL Readme
This pair of cpp and h file provides a very easy way to draw text in OpenGL, *using fonts generated from Truetype fonts*.

There are many tools and libraries available. But :
* I needed to have a really simple approach
* I certainly didn't want to link against any huge monster, such at Freetype library
* I never got satisfied by any existing tool on their way to batch primitives through OpenGL driver : most are rendering characters one after another (even using immediate mode). Some other might use the deprecated Display List...
* I did *not* need the whole flexibility that freetype offers
* I might want to use this implementation on tablets (Adroid and iOS)

This file proposes a very simple approach based on a texture Atlas containing all the necessary Fonts for a specific type and size. If more than one type or more than one size is needed, the application will have to work with many instances of the class and related resource.

The fonts are in a tga file; a binary file is associated, containing all the necessary offsets to work correctly with the texture-font from tga file.

## Example on how to use it.

init time:
````
        OpenGLText             oglText;
        ...
        if(!oglText.init(font_name, canvas_width, canvas_height))
            return false;
````

render time:
````
        oglText.beginString();
        float bbStr[2];
        char *tmpStr = "Hello world";
        oglText.stringSize(tmpStr, bbStr);
        oglText.drawString(posX - bbStr[0]*0.5, posY - bbStr[1], tmpStr, 0,0xF0F0F0F0);
        ...
        oglText.endString(); // will render the whole at once
````

# Baking Fonts

textBaker folder contains few details about this.

 For now, I got a bit 'hacky', here: baking Fonts is aschieved with patching GLText 0.3.1 ( http://gltext.sourceforge.net/home.php )

This folder contains the changes I made to finally save the data I needed.
Download freetype (I took 2.3.5-1) and download GLText. Then modify it with the data in this folder.

I have put a compiled version of it for Windows. Note this application would require to be better and therefore could crash or not work properly.

bakeFonts.exe < .ttf file > < size >

Example from the demo :
![Example](https://github.com/tlorach/OpenGLText/raw/master/example/example.png)

__Few comments:__

* I found this tool online, but I could have used anything that would use freetype library.
* One might argue that gltext is exactly doing what I want : display text onscreen with truetype fonts... **but**:
    * This project is rather big for what it does (isn't it ? :-)
    * the C++ design is overkill... at least to me
    * we must link against Freetype library to have it working. This is flexible, of course (becaude we can directly read any .ttf file...), but this is not appropriate for my goal : one might not want to link against freetype library if he targeted Android or iOS...
    * The rendering loop... is not the efficient way to do. Each font is rendered separately. Our approach is to render almost everything in one single drawcall.

The basic idea behind this patch is to allow me to save a tga file made of the character I need with the right size. a binary file will be created, in which structures of Glyphs are stored.
