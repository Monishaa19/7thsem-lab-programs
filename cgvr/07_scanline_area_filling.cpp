/*
 * 7) Scanline area filling
 *
 * IMPORTANT NOTES
 * - Needs freeglut (GLUT) installed.
 * - Compile (Windows/MinGW): g++ 07_scanline_area_filling.cpp -o scanfill -lfreeglut -lopengl32 -lglu32
 *   Compile (Linux):         g++ 07_scanline_area_filling.cpp -o scanfill -lglut -lGL -lGLU
 * - Input is asked in the console BEFORE the window opens.
 * - Origin (0,0) is the BOTTOM-LEFT corner; x and y range from 0 to 500.
 * - Enter the number of vertices (3-100), then each vertex as "x y".
 *   Enter vertices IN ORDER around the polygon (clockwise or anticlockwise),
 *   otherwise the edges cross and the fill looks wrong.
 *     Sample (square):   4   100 100   400 100   400 400   100 400
 *     Sample (triangle): 3   250 450   50 50   450 50
 * - Fill is blue, boundary is green.
 */
#include <iostream>
#include <algorithm>
#include <GL/glut.h>

using namespace std;

int n;
float x[100], y[100];

void drawLine(float x1, float y1, float x2, float y2) {
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void edgeDetect(float x1, float y1, float x2, float y2,
                int scanline, float intx[], int &count) {

    if (y1 > y2) {
        swap(x1, x2);
        swap(y1, y2);
    }

    // Ignore horizontal edges and avoid double-counting vertices
    if (y1 != y2 && scanline >= y1 && scanline < y2) {
        float xi = x1 + (scanline - y1) *
                   (x2 - x1) / (y2 - y1);

        intx[count++] = xi;
    }
}

void scanFill() {
    for (int s = 0; s <= 500; s++) {
        float intx[100];
        int count = 0;

        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            edgeDetect(x[i], y[i], x[j], y[j],
                       s, intx, count);
        }

        sort(intx, intx + count);

        // Fill between pairs of intersections
        for (int i = 0; i + 1 < count; i += 2) {
            drawLine(intx[i], s, intx[i + 1], s);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Fill the polygon
    scanFill();

    // Draw the polygon boundary
    glColor3f(0, 1, 0);
    glLineWidth(2);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        glVertex2f(x[i], y[i]);
    }
    glEnd();

    glFlush();
}

void myInit() {
    glClearColor(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    cout << "Enter the number of sides (3-100): ";
    cin >> n;

    if (n < 3 || n > 100) {
        cout << "Invalid number of vertices.\n";
        return 1;
    }

    cout << "Enter the coordinates of vertices (x y):\n";
    for (int i = 0; i < n; i++) {
        cin >> x[i] >> y[i];
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Scan-Line Area Filling");

    myInit();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
