#include <stdio.h>

int main(int argc, char *argv[]) {

    FILE *fileone = fopen(argv[1], "r");
    FILE *filetwo = fopen(argv[2], "w");
    if (fileone == NULL || filetwo == NULL) {
      printf("incorrect inputs");
      return 1;
    }

    // character by character
    int ch;
    while ((ch = fgetc(fileone)) != EOF) {
        fputc(ch, filetwo);
    }

    printf("done\n");
    fclose(fileone);
    fclose(filetwo);

    return 0;
}
