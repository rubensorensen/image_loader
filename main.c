#include "image_loader.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr,
                "[ERROR] Incorrect amount of arguments given. Expects exactly 1 argument\n");
        fprintf(stderr, "Usage: image_loader filepath\n");
        exit(EXIT_FAILURE);
    }
    
    const char* filepath = argv[1];
    Image img = read_image_from_file(filepath);
    
    if (!window_create(img.width, img.height)) {
        exit(EXIT_FAILURE);
    }
    
    while (window_update(img.pixels, img.width, img.height));
    window_destroy();
    free(img.pixels);
    
    return EXIT_SUCCESS;
}
