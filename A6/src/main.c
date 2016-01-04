#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#define BITMAP_ID 0x4D42
#define CHECKHEIGHT 64
#define CHECKWIDTH 64

float fTranslate;
float fRotate;
float fScale = 1.0f;	// set inital scale value to 1.0f

bool bPersp = false;
bool bAnim = false;
bool bWire = false;
bool bLight = false;
bool bMultiTexture = false;

int wHeight = 0;
int wWidth = 0;

// 0: Monet
// 1: Crack
// 2: Spot
// 3: Check
GLuint texture[4];
GLubyte checkImage[CHECKHEIGHT][CHECKWIDTH][3];

void Draw_Leg();

typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned int bfReserved;
    unsigned int bfOffBytes;
}  BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

// Read texture
unsigned char *loadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader) {
    FILE *filePtr;
    BITMAPFILEHEADER bitmapFileHeader;
    unsigned char *bitmapImage;
    int imageIdx = 0;
    unsigned char tempRGB;

    // open file
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read bitmap file header
    fread(&bitmapFileHeader.bfType, 2, 1, filePtr);
    fread(&bitmapFileHeader.bfSize, 4, 1, filePtr);
    fread(&bitmapFileHeader.bfReserved, 4, 1, filePtr);
    fread(&bitmapFileHeader.bfOffBytes, 4, 1, filePtr);

    // verify bitmap id
    if (bitmapFileHeader.bfType != BITMAP_ID) {
        fclose(filePtr);
        printf("Not valid bmp file...\n");
        return NULL;
    }

    // read bitmap info header
    fread(&bitmapInfoHeader->biSize, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biWidth, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biHeight, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biPlanes, 2, 1, filePtr);
    fread(&bitmapInfoHeader->biBitCount, 2, 1, filePtr);
    fread(&bitmapInfoHeader->biCompression, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biSizeImage, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biXPelsPerMeter, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biYPelsPerMeter, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biClrUsed, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biClrImportant, 4, 1, filePtr);

    // print out the info header for debug
    /*
    printf("biSize: %d\n", bitmapInfoHeader->biSize);
    printf("biWidth: %d\n", bitmapInfoHeader->biWidth);
    printf("biHeight: %d\n", bitmapInfoHeader->biHeight);
    printf("biPlanes: %d\n", bitmapInfoHeader->biPlanes);
    printf("biBitCount: %d\n", bitmapInfoHeader->biBitCount);
    printf("biCompression: %hu\n", bitmapInfoHeader->biBitCount);
    printf("biSizeImage: %u\n", bitmapInfoHeader->biSizeImage);
    */

    // move pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBytes, SEEK_SET);

    // allocate memory
    bitmapImage = (unsigned char *) malloc(bitmapInfoHeader->biSizeImage);
    
    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read the image
    fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);

    // verify read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // swap the r and b
    for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    // close file and return
    fclose(filePtr);
    
    printf("Load image success.\n\n");
    return bitmapImage;
}

void loadTexture(int i, char *filename, bool light) {
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char *bitmapData;

    bitmapData = loadBitmapFile(filename, &bitmapInfoHeader);
    glBindTexture(GL_TEXTURE_2D, texture[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
            0,                          // mipmap layer
            GL_RGB,                     // texture color mode
            bitmapInfoHeader.biWidth,   // texture width
            bitmapInfoHeader.biHeight,  // texture height
            0,                          // border
            GL_RGB,                     // bitmap color mode
            GL_UNSIGNED_BYTE,           // bitmap data type
            bitmapData);                // bitmap data pointer
    if (light)
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
}

void makeCheckImage() {
    int i, j, c;

    for (i = 0; i < CHECKHEIGHT; i++) {
        for (j = 0; j < CHECKWIDTH; j++) {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) 0;
            checkImage[i][j][2] = (GLubyte) 0;
        }
    }
}

void initTexture() {
    glGenTextures(4, texture);
    loadTexture(0, "Monet.bmp", true);
    loadTexture(1, "Crack.bmp", false);
    loadTexture(2, "Spot.bmp", false);

    makeCheckImage();

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CHECKWIDTH, CHECKHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void drawQuadsWithTexture() {
    glPushMatrix();
    glBegin(GL_QUADS);
        glTexCoord2i(1, 1);
        glVertex3i(1, 1, 0);
        
        glTexCoord2i(1, 0);
        glVertex3i(1, -1, 0);

        glTexCoord2i(0, 0);
        glVertex3i(-1, -1, 0);
        
        glTexCoord2i(0, 1);
        glVertex3i(-1, 1, 0);
    glEnd();
    glPopMatrix();
}

void cubeWithTexture(GLfloat length, GLuint texture) {
    GLfloat halfLength = length / 2;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();
    glScalef(halfLength, halfLength, halfLength);

    // front
    glPushMatrix();
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // back
    glPushMatrix();
    glTranslatef(0, 0, -1);
    drawQuadsWithTexture();
    glPopMatrix();

    // left
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // right
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // upper
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // bottom
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void cubeWithMultiTexture(GLfloat length, GLuint texture0, GLuint texture1) {
    GLfloat halfLength = length / 2;

    glPushMatrix();
    glScalef(halfLength, halfLength, halfLength);

    // front
    glPushMatrix();
    glTranslatef(0, 0, 1);
    // only use multitexture on the front
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // mixture textures
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
    // compute method
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
    // determine argvs
    
    // Previous: texture 0
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

    // Current: texture 1
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

    // Environment
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);

    // Draw
    glBegin(GL_QUADS);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1);
        glVertex3i(1, 1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0);
        glVertex3i(1, -1, 0);

        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
        glVertex3i(-1, -1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1);
        glVertex3i(-1, 1, 0);
    glEnd();

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // use single texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture0);

    // back
    glPushMatrix();
    glTranslatef(0, 0, -1);
    drawQuadsWithTexture();
    glPopMatrix();

    // left
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // right
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // upper
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    // bottom
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    drawQuadsWithTexture();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawTable() {
    if (bMultiTexture)
        cubeWithMultiTexture(1.0, texture[1], texture[2]);
    else
        cubeWithTexture(1.0, texture[1]);
}

void Draw_Scene()
{
    GLfloat teapot_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, teapot_specular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 60);

	glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[3]);

	glTranslatef(0, 0, 4+1);
	glRotatef(90, 1, 0, 0);
	glutSolidTeapot(1);

    glDisable(GL_TEXTURE_2D);
	glPopMatrix();


    GLfloat table_specular[] = { 0, 0, 0, 0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, table_specular);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,0);

	glPushMatrix();
	glTranslatef(0, 0, 3.5);
	glScalef(5, 4, 1);
    drawTable();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();

}

void Draw_Leg()
{
	glScalef(1, 1, 3);
	cubeWithTexture(1.0, texture[1]);
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
    case 'l': {bLight = !bLight; break;}
    case 't': {bMultiTexture = !bMultiTexture; break;}

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
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = {0,0,5,1};

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    if (bLight)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);

	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);
	Draw_Scene();

	if (bAnim) fRotate += 0.5f;
	glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480,480);
	int windowHandle = glutCreateWindow("Simple GLUT App");

    initTexture();
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);	
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}


