#include <stdio.h>
#include <stdlib.h>

void hide_data(const char *image, const char *data) {
    FILE *img = fopen(image, "rb+");
    FILE *dat = fopen(data, "rb");
    if (!img || !dat) {
        printf("Error opening files.\n");
        return;
    }

    fseek(img, 0, SEEK_END);
    size_t img_size = ftell(img);
    fseek(img, 0, SEEK_SET);

    unsigned char *buffer = malloc(img_size);
    fread(buffer, 1, img_size, img);

    fseek(img, 0, SEEK_SET);
    fwrite(buffer, 1, img_size, img);

    unsigned char ch;
    while (fread(&ch, 1, 1, dat)) {
        fwrite(&ch, 1, 1, img);
    }

    free(buffer);
    fclose(img);
    fclose(dat);
}

int main() {
    hide_data("image.bmp", "secret.txt");
    return 0;
}
