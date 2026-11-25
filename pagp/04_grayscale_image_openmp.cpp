/*
 * 4) Black and white (grayscale) image conversion - OpenMP scheduling
 *
 * IMPORTANT NOTES
 * - Run on LINUX (lab Ubuntu machine / WSL).
 * - Needs the GD image library:  sudo apt install libgd-dev
 * - Compile: g++ -fopenmp 04_grayscale_image_openmp.cpp -o image -lgd
 * - Run (3 file names are REQUIRED on the command line):
 *     ./image input.png output.png schedule.png
 *   input.png    = your colour image (must be a real .png file, NOT .jpg)
 *   output.png   = grayscale result (created by the program)
 *   schedule.png = shows which thread processed each pixel, one colour per
 *                  thread (red, green, blue, yellow, magenta, cyan, orange, purple)
 * - Input: threads (1-8), schedule (0-Default, 1-Static, 2-Dynamic, 3-Guided),
 *   chunk size (>= 1; ignored for schedule 0).
 *     Sample: 4  then  1  then  1000
 * - Any colour PNG image works (a screenshot is fine). To convert a JPG on Linux:
 *     sudo apt install imagemagick ; convert photo.jpg input.png
 * - Open schedule.png to compare how static/dynamic/guided split the work:
 *     static  -> equal stripes in fixed order,
 *     dynamic -> stripes in random-looking order,
 *     guided  -> big stripes first, then smaller ones.
 *   With a huge chunk size (bigger than width*height / threads) only some
 *   threads get work, so fewer colours appear - that is expected.
 * - Gray value = simple average (R+G+B)/3. Transparent areas of the PNG come
 *   out using their stored colour (often black).
 * - Fix applied vs. the PDF: the program now prints an error message when the
 *   input file cannot be opened / is not a PNG, or the output files cannot be
 *   written (the PDF version just exited silently).
 */
#include <iostream>
#include <cstdio>
#include <vector>
#include <gd.h>
#include <omp.h>

using namespace std;

int colors[8][3] = {
    {255,0,0}, {0,255,0}, {0,0,255}, {255,255,0},
    {255,0,255}, {0,255,255}, {255,128,0}, {128,0,255}
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: ./image input.png output.png schedule.png\n";
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        cout << "Cannot open input file: " << argv[1] << endl;
        return 1;
    }

    gdImagePtr img = gdImageCreateFromPng(fp);
    fclose(fp);
    if (!img) {
        cout << "Input file is not a valid PNG image: " << argv[1] << endl;
        return 1;
    }

    int w = gdImageSX(img), h = gdImageSY(img);
    int n = w * h, th, ch, s;

    cout << "Enter threads (1-8): ";
    cin >> th;
    cout << "Schedule: 0-Default, 1-Static, 2-Dynamic, 3-Guided: ";
    cin >> s;
    cout << "Enter chunk size: ";
    cin >> ch;

    if (th < 1 || th > 8 || s < 0 || s > 3 || ch < 1) {
        cout << "Invalid input\n";
        gdImageDestroy(img);
        return 1;
    }

    vector<int> gray(n), owner(n), input(n);

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            int p = gdImageGetPixel(img, x, y);
            input[y*w+x] = (gdImageRed(img,p) << 16) |
                           (gdImageGreen(img,p) << 8) |
                           gdImageBlue(img,p);
        }

    omp_set_num_threads(th);
    double t = omp_get_wtime();

    if (s == 0) {
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            int r=input[i]>>16, g=(input[i]>>8)&255, b=input[i]&255;
            int a=(r+g+b)/3, id=omp_get_thread_num();
            gray[i]=(a<<16)|(a<<8)|a;
            owner[i]=id;
        }
    }
    else if (s == 1) {
        #pragma omp parallel for schedule(static,ch)
        for (int i = 0; i < n; i++) {
            int r=input[i]>>16, g=(input[i]>>8)&255, b=input[i]&255;
            int a=(r+g+b)/3;
            gray[i]=(a<<16)|(a<<8)|a;
            owner[i]=omp_get_thread_num();
        }
    }
    else if (s == 2) {
        #pragma omp parallel for schedule(dynamic,ch)
        for (int i = 0; i < n; i++) {
            int r=input[i]>>16, g=(input[i]>>8)&255, b=input[i]&255;
            int a=(r+g+b)/3;
            gray[i]=(a<<16)|(a<<8)|a;
            owner[i]=omp_get_thread_num();
        }
    }
    else {
        #pragma omp parallel for schedule(guided,ch)
        for (int i = 0; i < n; i++) {
            int r=input[i]>>16, g=(input[i]>>8)&255, b=input[i]&255;
            int a=(r+g+b)/3;
            gray[i]=(a<<16)|(a<<8)|a;
            owner[i]=omp_get_thread_num();
        }
    }

    t = omp_get_wtime() - t;

    gdImagePtr out=gdImageCreateTrueColor(w,h);
    gdImagePtr vis=gdImageCreateTrueColor(w,h);

    for (int i=0; i<n; i++) {
        int x=i%w, y=i/w, id=owner[i];
        gdImageSetPixel(out,x,y,gray[i]);
        int c=(colors[id][0]<<16)|(colors[id][1]<<8)|colors[id][2];
        gdImageSetPixel(vis,x,y,c);
    }

    FILE *f=fopen(argv[2],"wb"), *v=fopen(argv[3],"wb");
    if (!f || !v) {
        cout << "Cannot create output files" << endl;
        if(f) fclose(f);
        if(v) fclose(v);
        gdImageDestroy(out);
        gdImageDestroy(vis);
        gdImageDestroy(img);
        return 1;
    }

    gdImagePng(out,f);
    gdImagePng(vis,v);
    fclose(f);
    fclose(v);

    cout << "Grayscale image: " << argv[2] << endl;
    cout << "Scheduling image: " << argv[3] << endl;
    cout << "Time: " << t << " seconds" << endl;

    gdImageDestroy(img);
    gdImageDestroy(out);
    gdImageDestroy(vis);

    return 0;
}
