/*
 * 5) Word search (word frequency) - OpenMP
 *
 * IMPORTANT NOTES
 * - This is a C program - keep the .c extension and compile with gcc.
 *   (As C++ it fails: string literals cannot be stored in "char *words[]".)
 * - Compile: gcc -fopenmp 05_word_search_openmp.c -o wordsearch
 * - Run:     ./wordsearch
 * - Input: name of a text file, then number of threads.
 *     Sample: input.txt  then  4
 *   A sample input.txt is included in this folder - keep it in the same folder
 *   you run the program from. With it the expected output is:
 *     lorem : 3,  ipsum : 3,  dolor : 2,  sit : 2,  amet : 2
 * - The words searched are fixed in the code: lorem, ipsum, dolor, sit, amet.
 *   To search other words, change the words[] array.
 * - Matching is case-insensitive and ignores punctuation ("Amet," = "amet").
 *   Only whole words match: "dolore" does NOT count as "dolor".
 * - File name must not contain spaces.
 * - If the file is not found, every count prints as -1.
 * - Each thread searches for a different word (only 5 words), so using more
 *   than 5 threads gives no extra speed-up. For a small file the time is ~0;
 *   use a big text file to compare thread counts.
 *
 * Fixes applied vs. the PDF:
 * - The PDF searched for "dolo" - a typo for "dolor" (the text is Lorem Ipsum),
 *   so it always printed 0. Changed to "dolor".
 * - The PDF compared words with punctuation attached, so "amet," or "sit."
 *   were NOT counted. Words read from the file are now cleaned with clean().
 * - Added a check that the number of threads is > 0.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define SIZE 5

char *words[SIZE] = {"lorem", "ipsum", "dolor", "sit", "amet"};
int counts[SIZE] = {0};

void lower(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = tolower((unsigned char)s[i]);
}

// Lower-case the word and remove punctuation (keep only letters and digits)
void clean(char *s) {
    int k = 0;
    for (int i = 0; s[i]; i++)
        if (isalnum((unsigned char)s[i]))
            s[k++] = tolower((unsigned char)s[i]);
    s[k] = '\0';
}

int getCount(char *fileName, char *key) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) return -1;

    char word[100];
    int count = 0;
    char search[100];

    strcpy(search, key);
    lower(search);

    while (fscanf(fp, "%99s", word) == 1) {
        clean(word);
        if (strcmp(word, search) == 0)
            count++;
    }

    fclose(fp);
    return count;
}

int main() {
    char fileName[100];
    int threads;

    printf("Enter file name: ");
    scanf("%99s", fileName);

    printf("Enter number of threads: ");
    scanf("%d", &threads);

    if (threads <= 0) {
        printf("Invalid number of threads\n");
        return 0;
    }

    omp_set_num_threads(threads);

    double t = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < SIZE; i++)
        counts[i] = getCount(fileName, words[i]);

    t = omp_get_wtime() - t;

    printf("\nWord frequencies:\n");
    for (int i = 0; i < SIZE; i++)
        printf("%s : %d\n", words[i], counts[i]);

    printf("Time taken: %f seconds\n", t);

    return 0;
}
