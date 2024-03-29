//
//  main.c
//  A4
//
//  Created by MForever78 on 15/12/2.
//  Copyright © 2015年 MForever78. All rights reserved.
//

#include <stdlib.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stanford_bunny.h"

float eye[] = {0, 4, 6};
float center[] = {0, 0, 0};
float fDistance = 0.2f;
float fRotate = 0;
int bunnyNumber;

bool bAnim = false;

bool bDrawList = false;
GLint tableLists[37];
int tableIndex = 0;

void DrawTable()
{
    glPushMatrix();
    glTranslatef(0, 3.5, 0);
    glScalef(8, 0.5, 8);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.5, 1.5, 1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-1.5, 1.5, 1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.5, 1.5, -1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-1.5, 1.5, -1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();
}

void DrawBunnyScale(float scale) {
    glPushMatrix();
    glScalef(scale, scale, scale);
    DrawBunny();
    glPopMatrix();
}


void DrawLotsOfBunnies() {
    int row, remainder;
    int i, j;
    row = bunnyNumber / 6;
    remainder = bunnyNumber % 6;
    
    glPushMatrix();
    // put the bunny on the table
    glTranslatef(0, 4.5, 0);
    glTranslatef(-2.5, 0, 3.5);
    
    for (i = 0; i < row; i++) {
        glTranslatef(6, 0, 0);
        glTranslatef(0, 0, -1);
        for (j = 0; j < 6; j++) {
            glTranslatef(-1, 0, 0);
            DrawBunnyScale(2);
        }
    }
    
    glTranslatef(6, 0, -1);
    for (i = 0; i < remainder; i++) {
        glTranslatef(-1, 0, 0);
        DrawBunnyScale(2);
    }

    glPopMatrix();
}

GLint GenTableList()
{
    GLint lid=glGenLists(1);
    glNewList(lid, GL_COMPILE);
    
    DrawLotsOfBunnies();
    DrawTable();
    
    glEndList();
    return lid;
}

void Draw_Table_List()
{

    glCallList(tableLists[tableIndex]);
}

void DrawScene()
{
    DrawLotsOfBunnies();
    DrawTable();
}

void reshape(int width, int height)
{
    if (height==0)										// Prevent A Divide By Zero By
    {
        height=1;										// Making Height Equal One
    }
    
    glViewport(0,0,width,height);						// Reset The Current Viewport
    
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    
    float whRatio = (GLfloat)width/(GLfloat)height;
    gluPerspective(45, whRatio, 1, 1000);
    
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void idle()
{
    glutPostRedisplay();
}

void key(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        case 'q': {exit(0); break; }
        case 'i': {
            if (bunnyNumber < 36) {
                bunnyNumber++;
                tableIndex++;
            }
            break;
        }
        case 'k': {
            if (bunnyNumber > 0) {
                bunnyNumber--;
                tableIndex--;
            }
            break;
        }
            
        case 'a':
        {
            eye[0] += fDistance;
            center[0] += fDistance;
            break;
        }
        case 'd':
        {
            eye[0] -= fDistance;
            center[0] -= fDistance;
            break;
        }
        case 'w':
        {
            eye[1] -= fDistance;
            center[1] -= fDistance;
            break;
        }
        case 's':
        {
            eye[1] += fDistance;
            center[1] += fDistance;
            break;
        }
        case 'z':
        {
            eye[2] *= 0.95;
            break;
        }
        case 'c':
        {
            eye[2] *= 1.05;
            break;
        }
        case 'l':
        {
            bDrawList = !bDrawList;
            break;
        }
        case ' ':
        {
            bAnim = !bAnim;
            break;
        }
        default: break;
    }
}

void getFPS()
{
    static int frame = 0, time, timebase = 0;
    static char buffer[256];
    
    char mode[64];
    if (bDrawList)
        strcpy(mode, "display list");
    else
        strcpy(mode, "naive");
    
    frame++;
    time=glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        sprintf(buffer,"FPS:%4.2f %s",
                frame*1000.0/(time-timebase), mode);
        timebase = time;
        frame = 0;
    }
    
    char *c;
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);  
    glPushMatrix();               
    glLoadIdentity();             
    glOrtho(0,480,0,480,-1,1);    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(10,10);
    for (c=buffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void redraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0.5, 0, 1);
    glLoadIdentity();									// Reset The Current Modelview Matrix
    
    gluLookAt(eye[0], eye[1], eye[2],
              center[0], center[1], center[2],
              0, 1, 0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    GLfloat gray[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat light_pos[] = {10, 10, 10, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gray);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
    glEnable(GL_LIGHT0);
    
    if (bAnim) 
        fRotate += 0.5f;
    glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
    
    glScalef(0.4, 0.4, 0.4);
    if(!bDrawList)
        DrawScene();						// old way
    else 
        Draw_Table_List();                  // new way
    
    getFPS();
    glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(480,480);
    int windowHandle = glutCreateWindow("Exercise 4");
    
    glutDisplayFunc(redraw);
    glutReshapeFunc(reshape);	
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    
    int i;
    for (i = 0; i < 37; i++) {
        bunnyNumber = i;
        tableLists[i] = GenTableList();
    }
    bunnyNumber = 0;
    
    glutMainLoop();
    return 0;
}

