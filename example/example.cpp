
#if defined(__APPLE__)
    #include <GLUT/glut.h>
#else
    #include <stdlib.h>
    #include <GL/glew.h>
    #include <GL/glut.h>
#endif

#include <string>

#include "../OpenGLText.h"

#include "arial_10.h"
#include "arial_10_bitmap.h"

OpenGLText oglText;
OpenGLText oglText2;
OpenGLText oglText3;
OpenGLText oglText4;

float g_rotate[2] = {0, 0},
      g_prev_x = 0,
      g_prev_y = 0,
      g_dolly = 5,
      g_pan[2] = {0, 0},
      g_center[3] = {0, 0, 0},
      g_size = 0;

int   g_mbutton[3] = {0, 0, 0};

int   g_width = 800,
      g_height = 600;

//------------------------------------------------------------------------------
void
reshape(int width, int height) {

    g_width = width;
    g_height = height;
    oglText.changeSize(g_width, g_height);
    oglText.changeCanvas(g_width, g_height);
    oglText2.changeSize(g_width, g_height);
    oglText2.changeCanvas(g_width, g_height);
    oglText3.changeSize(g_width, g_height);
    oglText3.changeCanvas(g_width, g_height);
    oglText4.changeSize(g_width, g_height);
    oglText4.changeCanvas(g_width, g_height);
}
//------------------------------------------------------------------------------
void
display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, g_width, g_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-1, -1, 1);
    glVertex3f( 1, -1, 1);
    glColor3f(0, 0, 0);
    glVertex3f( 1,  1, 1);
    glVertex3f(-1,  1, 1);
    glEnd();

    double aspect = g_width/(double)g_height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.01, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-g_pan[0], -g_pan[1], -g_dolly);
    glRotatef(g_rotate[1], 1, 0, 0);
    glRotatef(g_rotate[0], 0, 1, 0);
    glTranslatef(-g_center[0], -g_center[1], -g_center[2]);
    glRotatef(-90, 1, 0, 0); // z-up model

    oglText.beginString();
    oglText2.beginString();

    oglText.drawString(10, g_height-100, "Hello World", 1, 0xFFFFFF80);

    oglText2.drawString(10, g_height-180, 
        "There is a tide in the affairs of men, \n"
        "Which taken at the flood, leads on to fortune.\n"
        "Omitted, all the voyage of their life is bound \n"
        "in shallows and in miseries. On such a full sea are we now afloat.\n"
        "And we must take the current when it serves, or lose our ventures.\n"
        "William Shakespeare"
        , 2, 0xE0E0FFFF);
    oglText.endString();
    oglText2.endString();

    oglText3.beginString();
    oglText3.drawString(10, g_height-350, 
        "Total : 3 drawcalls (1 Drawcall per font style)"
        , 1, 0xFFE0FFA0);
    oglText3.endString();

    oglText4.beginString();
    oglText4.drawString(10, g_height-370, 
        "(this small font was created from 2 .h files generated by the converter)"
        , 1, 0xFFE0FFA0);
    oglText4.endString();

    glutSwapBuffers();
}

//------------------------------------------------------------------------------
void motion(int x, int y) {

    if (g_mbutton[0] && !g_mbutton[1] && !g_mbutton[2]) {
        // orbit
        g_rotate[0] += x - g_prev_x;
        g_rotate[1] += y - g_prev_y;
    } else if (!g_mbutton[0] && g_mbutton[1] && !g_mbutton[2]) {
        // pan
        g_pan[0] -= g_dolly*(x - g_prev_x)/g_width;
        g_pan[1] += g_dolly*(y - g_prev_y)/g_height;
    } else if (g_mbutton[0] && g_mbutton[1] && !g_mbutton[2]) {
        // dolly
        g_dolly -= g_dolly*0.01f*(x - g_prev_x);
        if(g_dolly <= 0.01) g_dolly = 0.01f;
    }

    g_prev_x = float(x);
    g_prev_y = float(y);
}

//------------------------------------------------------------------------------
void mouse(int button, int state, int x, int y) {

    g_prev_x = float(x);
    g_prev_y = float(y);
    g_mbutton[button] = !state;
}

//------------------------------------------------------------------------------
void quit() {
    exit(0);
}

//------------------------------------------------------------------------------
void
keyboard(unsigned char key, int x, int y) {

    switch (key) {
        case 'q': quit();
    }
}

//------------------------------------------------------------------------------
void
idle() {
    //glutPostRedisplay();
}

//------------------------------------------------------------------------------
void
initGL() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_LIGHT0);
    glColor3f(1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if(!oglText.init("Candy Script_48", g_width, g_height))
        exit(1);
    if(!oglText2.init("Shelley Allegro BT_26", g_width, g_height))
        exit(1);
    if(!oglText3.init("baub_16", g_width, g_height))
        exit(1);
    if(!oglText4.init(font_10::image, (OpenGLText::FileHeader*)&font_10::font, g_width, g_height))
        exit(1);

}

//------------------------------------------------------------------------------
int main(int argc, char ** argv) {

    glutInit(&argc, argv);

    //glutSetWindowTitle( g_defaultShapes[m].name.c_str() );
    glutInitDisplayMode(GLUT_RGBA |GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 400);
    glutCreateWindow("Text test");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glewInit();
    initGL();

    glutIdleFunc(idle);
    glutMainLoop();

    quit();
}

//------------------------------------------------------------------------------
