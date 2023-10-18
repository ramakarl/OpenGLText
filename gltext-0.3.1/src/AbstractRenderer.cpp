/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: nil c-basic-offset: 3 -*- */
// vim:cindent:ts=3:sw=3:et:tw=80:sta:
/*************************************************************** gltext-cpr beg
 *
 * GLText - OpenGL TrueType Font Renderer
 * GLText is (C) Copyright 2002 by Ben Scott
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile: AbstractRenderer.cpp,v $
 * Date modified: $Date: 2003/03/11 02:57:08 $
 * Version:       $Revision: 1.12 $
 * -----------------------------------------------------------------
 *
 ************************************************************ gltext-cpr-end */
#include <algorithm>
#include <iostream>
#include "tga.h"

#include "AbstractRenderer.h"

   struct GlyphInfo
   {
       struct Pix // pixel oriented data
       {
           int u, v;
           int width, height;
           int advance;
           int offX, offY;
       };
       struct Norm // normalized data
       {
           float u, v; // position in the map in normalized coords
           float width, height;
           float advance;
           float offX, offY;
       };
       Pix  pix;
       Norm norm;
   };
   struct FileHeader
   {
       int texwidth, texheight;
       struct Pix
       {
           int ascent;
           int descent;
           int linegap;
       };
       struct Norm
       {
           float ascent;
           float descent;
           float linegap;
       };
       Pix  pix;
       Norm norm;
       GlyphInfo glyphs[256];
   };



namespace gltext
{
   AbstractRenderer::AbstractRenderer(Font* font)
      : mFont(font)
   {
   }

 
   void GLTEXT_CALL AbstractRenderer::saveFonts(const char* file)
   {
      float fW = 128.0;
      float fH = 512.0;
      char fileName[100];
      int penX = 1;
      int penY = 1;
      int maxheight = 0;
      const char* text = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_\\`~=+[]{};':\"<>/?,. ";
      const int H  = mFont->getAscent() + mFont->getDescent() + mFont->getLineGap();

      // optimize the size of the map
      int texW = 64;
      do {
          texW <<= 1;
          penX = 1;
          penY = 1;
          maxheight = 0;
          for (const char* itr = text; *itr; ++itr)
          {
             Glyph* glyph = mFont->getGlyph(*itr);
             //GLGlyph* drawGlyph = makeGlyph(glyph);
              int width  = glyph->getWidth();
              int height = glyph->getHeight();
              if(height > maxheight)
                  maxheight = height;
              int offX   = glyph->getXOffset();
              int offY   = glyph->getYOffset();
              if(penX + (width+2) >= texW)
              {
                  penX = 0;
                  penY += maxheight+2;
                  maxheight = 0;
              }
              penX += width+2;
          }
      } while((penY > texW)&&(penY > (texW<<1)));
      fH = (float)(penY + maxheight*2);
      fW = (float)texW;

      // now write things
      errno_t err = strcpy_s(fileName, file);
      int i=(int)strlen(fileName)-1;
      while((i > 0)&&(file[i] != '.'))
          i--;
      fileName[i] = '\0';
      int fileSize = sizeof(FileHeader);
      FileHeader *fileHeader = (FileHeader *)malloc( fileSize );
      memset(fileHeader, 0, fileSize);

      fileHeader->pix.ascent = mFont->getAscent();
      fileHeader->norm.ascent = (float)mFont->getAscent()/fH;

      fileHeader->pix.descent = mFont->getDescent();
      fileHeader->norm.descent = (float)mFont->getDescent()/fH;

      fileHeader->pix.linegap = mFont->getLineGap();
      fileHeader->norm.linegap = (float)mFont->getLineGap()/fH;

      fileHeader->texwidth = (int)fW;
      fileHeader->texheight = (int)fH;

      FILE *fh;
      char fileNameBin[100];
      sprintf_s(fileNameBin, "%s_%d.bin", fileName, mFont->getSize());

      errno_t ferr = fopen_s( &fh, fileNameBin, "wb" );
      if( ferr != 0 )
        return;

      penX = 1;
      penY = 1;
      maxheight = 0;
      u8* data = new u8[fileHeader->texwidth * fileHeader->texheight];
      memset(data, 0, fileHeader->texwidth * fileHeader->texheight);
      for (const char* itr = text; *itr; ++itr)
      {
         Glyph* glyph = mFont->getGlyph(*itr);
         //GLGlyph* drawGlyph = makeGlyph(glyph);
          int width  = glyph->getWidth();
          int height = glyph->getHeight();
          if(height > maxheight)
              maxheight = height;
          int offX   = glyph->getXOffset();
          int offY   = glyph->getYOffset();
          if(penX + (width+2) >= fileHeader->texwidth)
          {
              penX = 0;
              penY += maxheight+2;
              maxheight = 0;
          }
          GlyphInfo &g = fileHeader->glyphs[*itr];

          g.pix.advance = glyph->getAdvance();
          g.norm.advance = (float)glyph->getAdvance()/fW;
          g.pix.u = penX;
          g.pix.v = penY;
          g.norm.u = (float)penX/fW;
          g.norm.v = (float)penY/fH;
          g.pix.width = width;
          g.pix.height = height;
          g.norm.width = (float)width/fW;
          g.norm.height = (float)height/fH;
          g.pix.offX = offX;
          g.pix.offY = offY;
          g.norm.offX = (float)offX/fW;
          g.norm.offY = (float)offY/fH;

          glyph->render(data, penX, penY, fileHeader->texwidth);
          penX += width+2;
      }
      fwrite( fileHeader, sizeof (unsigned char), fileSize, fh );
      fclose( fh );
      TGA tga;
      char fileNameTGA[100];
      sprintf_s (fileNameTGA, "%s_%d.tga", fileName, mFont->getSize());
      TGA::TGAError terr = tga.saveFromExternalData( fileNameTGA, fileHeader->texwidth, fileHeader->texheight, TGA::RGB, data );

   }


   void GLTEXT_CALL AbstractRenderer::render(const char* text)
   {
      const int ascent  = mFont->getAscent();
      const int descent = mFont->getDescent();
      const int height  = ascent + descent + mFont->getLineGap();

      int penX = 0;
      int penY = 0;

      unsigned char last_character = 0;

      // Run through each char and generate a glyph to draw
      for (const char* itr = text; *itr; ++itr)
      {
         // newline?
         if (*itr == '\n')
         {
            penX = 0;
            penY += height;
            continue;
         }

         // Get the glyph for the current character
         Glyph* fontGlyph = mFont->getGlyph(*itr);
         if (!fontGlyph)
         {
            continue;
         }

         // Check the cache first
         GLGlyph* drawGlyph = mCache.get(fontGlyph);
         if (!drawGlyph)
         {
            // Cache miss. Ask this renderer to create a new one
            drawGlyph = makeGlyph(fontGlyph);
            if (!drawGlyph)
            {
               // AAACK! Couldn't create the glyph. Fail silently.
               continue;
            }
            mCache.put(fontGlyph, drawGlyph);
         }

         int kerning = mFont->getKerning(last_character, *itr);
         last_character = *itr;
         int old_x = penX;
         penX += kerning;

         // Now tell the glyph to render itself.
         drawGlyph->render(penX, penY);
         penX += fontGlyph->getAdvance();

         // Kerning shouldn't make us draw farther and farther to the
         // left...  this fixes the "sliding dot problem".
         penX = std::max(penX, old_x);
      }
   }

   int GLTEXT_CALL AbstractRenderer::getWidth(const char* text)
   {
      if (! text)
      {
         return 0;
      }

      int max_width = 0;
      int width = 0;

      unsigned char last_character = 0;

      // Iterate over each character adding its width
      for (const char* itr = text; *itr != 0; ++itr)
      {
         if (*itr == '\n')
         {
            width = 0;
            continue;
         }

         // Get the glyph for the current character
         Glyph* fontGlyph = mFont->getGlyph(*itr);
         if (fontGlyph)
         {
            int kerning = mFont->getKerning(last_character, *itr);
            last_character = *itr;
            width += kerning;
         
            // Add this glyph's advance
            width += fontGlyph->getAdvance();
            max_width = std::max(width, max_width);
         }
      }

      return max_width;
   }

   int GLTEXT_CALL AbstractRenderer::getHeight(const char* text)
   {
      const int ascent = mFont->getAscent();
      const int descent = mFont->getDescent();
      const int height = ascent + descent + mFont->getLineGap();
      return int(std::count(text, text + strlen(text), '\n') + 1) * height;
   }

   Font* GLTEXT_CALL AbstractRenderer::getFont()
   {
      return mFont.get();
   }
}
