#include "image_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
main(void)
{
    Image img = read_image_from_file("test_images/dice.tga");
    write_image_to_ppm(img, "output.ppm");
    
    return EXIT_SUCCESS;
}
