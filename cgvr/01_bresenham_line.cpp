/*
 * 1) Bresenham line
 *
 * IMPORTANT NOTES
 * - Needs freeglut (GLUT) installed.
 * - Compile (Windows/MinGW): g++ 01_bresenham_line.cpp -o line -lfreeglut -lopengl32 -lglu32
 *   Compile (Linux):         g++ 01_bresenham_line.cpp -o line -lglut -lGL -lGLU
 * - Input is asked in the console BEFORE the window opens.
 * - Origin (0,0) is the CENTRE of the window; x and y range from -250 to 250.
 * - Choice 1 (keyboard): enter x1, y1, x2, y2 within -250..250.
 *     Sample: 1  then  -200 -100 200 150
 * - Choice 2 (mouse): left-click once for the start point, click again for the
 *   end point. You can keep drawing more lines (every 2 clicks = 1 line).
 *   Clicked coordinates are printed in the console.
 * - In mouse mode, resizing/minimising the window erases the drawn lines.
 */
#include<iostream>
#include<cmath>
#include<GL/glut.h>

using namespace std;

int xc1, xc2, yc1, yc2;

void drawPixel(int x, int y) {
    glColor3f(0, 0, 1);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void drawLine() {
    int dx = abs(xc2 - xc1), dy = abs(yc2 - yc1);

    int incx = xc1 <= xc2 ? 1 : -1;
    int incy = yc1 <= yc2 ? 1 : -1;

    int x = xc1, y = yc1;
    if (dx > dy) {
        int p = 2 * dy - dx;
        int inc1 = 2 * (dy - dx);
        int inc2 = 2 * dy;
        for (int i = 0; i <= dx; i++) {
            drawPixel(x, y);
            if (p > 0) {
                y += incy;
                p += inc1;
            } else {
                p += inc2;
            }
            x += incx;
        }
    } else {
        int p = 2 * dx - dy;
        int inc1 = 2 * (dx - dy);
        int inc2 = 2 * dx;
        for (int i = 0; i <= dy; i++) {
            drawPixel(x, y);
            if (p > 0) {
                x += incx;
                p += inc1;
            } else {
                p += inc2;
            }
            y += incy;
        }
    }
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    static bool flag = false;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!flag) {
            xc1 = x - 250;
            yc1 = 250 - y;
            cout << "x1, y1 : " << xc1 << " " << yc1 << " \n";
        } else {
            xc2 = x - 250;
            yc2 = 250 - y;
            cout << "x2, y2 : " << xc2 << " " << yc2 << " \n";
            drawLine();
        }
        flag = !flag;
    }
}

// Mouse mode: clear the window to white
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

// Keyboard mode: clear the window, then draw the line
void displayKeyboard() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawLine();
}

void myInit() {
    glClearColor(1, 1, 1, 1);
    gluOrtho2D(-250, 250, -250, 250);
}

int main(int argc, char** argv) {
    int choice;
    cout << "Enter 1 for keyboard and 2 for mouse \n";
    cin >> choice;
    if (choice == 1) {
        cout << "Enter x1 : ";
        cin >> xc1;
        cout << "Enter y1 : ";
        cin >> yc1;
        cout << "Enter x2 : ";
        cin >> xc2;
        cout << "Enter y2 : ";
        cin >> yc2;
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Line Drawing");

    if (choice == 2) {
        glutMouseFunc(mouse);
        glutDisplayFunc(display);
    } else {
        glutDisplayFunc(displayKeyboard);
    }
    myInit();
    glutMainLoop();
}
