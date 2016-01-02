#include "GLUT/glut.h"
#include <math.h>

const double PI = acos(-1.0);

void rotate(double x1, double y1, double *x2, double *y2, double degree) {
    double x = *x2 - x1;
    double y = *y2 - y1;
    double xt = x * cos(degree) - y * sin(degree);
    double yt = x * sin(degree) + y * cos(degree);
    *x2 = x1 + xt;
    *y2 = y1 + yt;
}

void map(double *x, double *y) {
    *x /= 15;
    *y /= 10;
}

void drawStar(GLdouble cx, GLdouble cy, GLdouble outerRadius, double degree) {
    double radius = outerRadius / (sin(54 * PI / 180) + cos(54 * PI / 180) * tan(72 * PI / 180));
	double l1 = radius * sin(54 * PI / 180);
	double l2 = radius * cos(54 * PI / 180);
	double l3 = l2 * tan(72 * PI / 180);
	double l4 = l2 / cos(72 * PI / 180);
	double l5 = radius * cos(18 * PI / 180);
	double l6 = radius * sin(18 * PI / 180);
	double l7 = l4 * sin(54 * PI / 180);
	double l8 = l4 * cos(54 * PI / 180);

    double coord[11][2] = {
        cx, cy + l1 + l3,
        cx + l2, cy + l1,
        cx + l2 + l4, cy + l1,
        cx + l5, cy - l6,
        cx + l7, cy - radius - l8,
        cx, cy - radius,
        cx - l7, cy - radius - l8,
        cx - l5, cy - l6,
        cx - l2 - l4, cy + l1,
        cx - l2, cy + l1,
        cx, cy + l1 + l3
    };
    double x, y;


	glBegin(GL_POLYGON);
    
    // draw center point
    x = cx;
    y = cy;
    map(&x, &y);
	glVertex2f(x, y);

    // draw from x1 to x11
    for (int i = 0; i < 11; i++) {
        x = coord[i][0];
        y = coord[i][1];
        rotate(cx, cy, &x, &y, degree);
        map(&x, &y);
        glVertex2f(x, y);
    }

	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glVertex2f(-1, -1);
	glEnd();
	glColor3f(1, 1, 0);
	drawStar(-10, 5, 3, 0);
    drawStar(-5, 8, 1, PI - atan(5.0 / 3));
    drawStar(-3, 6, 1, PI - atan(7.0 / 1));
    drawStar(-3, 3, 1, atan(7.0 / 2));
    drawStar(-5, 1, 1, atan(5.0 / 4));
	glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(900, 600);
    glutCreateWindow("Flag");

	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}
