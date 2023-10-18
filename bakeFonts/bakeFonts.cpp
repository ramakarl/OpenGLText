#include <iostream>
#include <windows.h>
#include "GL/glut.h"
#include <gltext.h>

using namespace gltext;


FontRendererPtr mipRenderer;


void drawText(const FontRendererPtr& renderer, std::string text)
{
   const int size   = renderer->getFont()->getSize();
   const int dpi    = renderer->getFont()->getDPI();
   const int width  = renderer->getWidth(text.c_str());
   const int height = renderer->getHeight(text.c_str());
   GLTEXT_STREAM(renderer) << text << "\nsize: " << size << " dpi: " << dpi;
}

void display()
{
   glClearColor(0, 0, 0, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glColor4f(1.f, 1.f, 1.f, 1);

   glTranslatef(50, 50, 0);

   drawText(mipRenderer, "Font Rendered !");

   glutSwapBuffers();
}

void reshape(int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, width, height, 0);
}

void keydown(unsigned char key, int x, int y)
{
   if (key == 27 || key == 'q')
   {
      exit(0);
   }
}

int
main(int argc, char** argv)
{
   glutInitWindowSize( 1800, 500 );
   glutInitWindowPosition(50, 50);
   int argc2 = 1;
   glutInit(&argc2, argv);//&argc, argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
   glutCreateWindow("bakeFonts");

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keydown);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   const char* ttfName = "fonts\\Shelley Allegro BT.ttf";
   int sz = 0;
   if(argc > 1)
       ttfName = argv[1];
   if(argc > 2)
       sz = atoi(argv[2]);
   if(sz <= 0)
       sz = 128;

   FontPtr font = OpenFont(ttfName, sz);
   if (! font)
   {
      std::cerr<<"Couldn't create font!"<<std::endl;
      return 1;
   }

   mipRenderer = CreateRenderer( gltext::MIPMAP, font);
   mipRenderer->saveFonts(ttfName);
   if (! mipRenderer)
   {
      std::cerr<<"Couldn't create mipmap font renderer!"<<std::endl;
      return 1;
   }

   glutMainLoop();
}
