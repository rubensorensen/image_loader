#include "image_loader.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
main(void)
{    
    Image img = read_image_from_file("test_images/football_seal.tga");
    
    if (!window_create(img.width, img.height)) {
        exit(EXIT_FAILURE);
    }
    
    while (window_update(img.pixels, img.width, img.height));
    window_destroy();
    free(img.pixels);
    
    return EXIT_SUCCESS;
}
