//
//  main.c
//  A5
//
//  Created by MForever78 on 15/12/12.
//  Copyright © 2015年 MForever78. All rights reserved.
//

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <stdbool.h>

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = false;
bool bAnim = false;
bool bWire = false;
bool bColor = false;
bool bLight = false;
bool bChandelier = false;

int wHeight = 0;
int wWidth = 0;

GLfloat light_pos[] = {5,5,5,1};
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat ugly[] = { 0.8, 0.2, 0.75, 1.0 };

GLfloat bigLightDir[4] = {0, -1, 0, 1};
GLfloat bigLightDeg = 30;

void Draw_Leg();

void Draw_Triangle() // This function draws a triangle with RGB colors
{
    GLfloat teapot_diffuse[4] = {0.85f, 0.65f, 0.2f, 1.0f};
    GLfloat teapot_specular[4] = {0.6f, 0.6f, 0.6f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, teapot_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, teapot_specular);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,50);
    
    glPushMatrix();
    glTranslatef(0, 0, 4+1);
    glRotatef(90, 1, 0, 0);
    glutSolidTeapot(1);
    glPopMatrix();
    
    GLfloat table_diffuse[4] = {1.0f, 0, 0, 1.0f};
    GLfloat table_specular[4] = {0, 0, 0, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, table_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, table_specular);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,0);
    glPushMatrix();
    glColor3f(1.0, 0, 0);
    glTranslatef(0, 0, 3.5);
    glScalef(5, 4, 1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    GLfloat leg_diffuse[4][4] = {
        {0, 1, 0, 1},
        {1, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 1, 1}
    };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leg_diffuse[0]);
    glPushMatrix();
    glTranslatef(1.5, 1, 1.5);
    Draw_Leg();
    glPopMatrix();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leg_diffuse[1]);
    glPushMatrix();
    glTranslatef(-1.5, 1, 1.5);
    Draw_Leg();
    glPopMatrix();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leg_diffuse[2]);
    glPushMatrix();
    glTranslatef(1.5, -1, 1.5);
    Draw_Leg();
    glPopMatrix();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leg_diffuse[3]);
    glPushMatrix();
    glTranslatef(-1.5, -1, 1.5);
    Draw_Leg();
    glPopMatrix();
    
}

void Draw_Leg()
{
    glScalef(1, 1, 3);
    glutSolidCube(1.0);
}

void updateView(int width, int height)
{
    glViewport(0,0,width,height);						// Reset The Current Viewport
    
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    
    float whRatio = (GLfloat)width/(GLfloat)height;
    if (bPersp) {
        gluPerspective(45.0f, whRatio,0.1f,100.0f);
        //glFrustum(-3, 3, -3, 3, 3,100);
    } else {
        glOrtho(-3 ,3, -3, 3,-100,100);
    }
    
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
    if (height==0)										// Prevent A Divide By Zero By
    {
        height=1;										// Making Height Equal One
    }
    
    wHeight = height;
    wWidth = width;
    
    updateView(wHeight, wWidth);
}

void idle()
{
    glutPostRedisplay();
}

float eye[] = {0, 0, 8};
float center[] = {0, 0, 0};

void key(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        case 'q': {exit(0); break; }
        case 'p': {bPersp = !bPersp; break; }
            
        case ' ': {bAnim = !bAnim; break;}
        case 'o': {bWire = !bWire; break;}
            
        case 'a': {
            eye[0] -= 0.2f;
            center[0] -= 0.2f;
            break;
        }
        case 'd': {
            eye[0] += 0.2f;
            center[0] += 0.2f;
            break;
        }
        case 'w': {
            eye[1] -= 0.2f;
            center[1] -= 0.2f;
            break;
        }
        case 's': {
            eye[1] += 0.2f;
            center[1] += 0.2f;
            break;
        }
        case 'z': {
            eye[2] -= 0.2f;
            center[2] -= 0.2f;
            break;
        }
        case 'c': {
            eye[2] += 0.2f;
            center[2] += 0.2f;
            break;
        }
        case 'i': {
            light_pos[1] += 0.2f;
            break;
        }
        case 'k': {
            light_pos[1] -= 0.2f;
            break;
        }
        case 'j': {
            light_pos[0] -= 0.2f;
            break;
        }
        case 'l': {
            light_pos[0] += 0.2f;
            break;
        }
        case ',': {
            light_pos[2] += 0.2f;
            break;
        }
        case '.': {
            light_pos[2] -= 0.2f;
            break;
        }
        case '/': {
            bColor = !bColor;
            break;
        }
        case '1': {
            bLight = !bLight;
            break;
        }
        case '2': {
            bChandelier = !bChandelier;
            break;
        }
        case '9': {
            bigLightDir[0] -= 0.2f;
            break;
        }
        case '0': {
            bigLightDir[0] += 0.2f;
            break;
        }
        case '=': {
            bigLightDeg += 1;
            break;
        }
        case '-': {
            bigLightDeg -= 1;
            break;
        }
    }
    
    updateView(wHeight, wWidth);
}


void redraw()
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();									// Reset The Current Modelview Matrix
    
    gluLookAt(eye[0], eye[1], eye[2],
              center[0], center[1], center[2],
              0, 1, 0);
    
    if (bWire) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    if (bColor)
        glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    else
        glLightfv(GL_LIGHT0, GL_AMBIENT, ugly);
    
    if (bChandelier)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);
    
    GLfloat bigLightPos[4] = {0, 5, 2, 1};
    
    glLightfv(GL_LIGHT1, GL_POSITION, bigLightPos);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, bigLightDeg);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, bigLightDir);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2);
    
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightfv(GL_LIGHT1, GL_AMBIENT, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    
    if (bLight)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);
    
    //	glTranslatef(0.0f, 0.0f,-6.0f);			// Place the triangle at Center
    glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
    glRotatef(-90, 1, 0, 0);
    glScalef(0.2, 0.2, 0.2);
    Draw_Triangle();						// Draw triangle
    
    if (bAnim) fRotate    += 0.5f;
    glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(480,480);
    int windowHandle = glutCreateWindow("Simple GLUT App");
    
    glutDisplayFunc(redraw);
    glutReshapeFunc(reshape);	
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}
