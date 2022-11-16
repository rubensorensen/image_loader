#include "image_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

const char* TGA_IMAGE_TYPE_STRINGS[] = {
    "TGA_IMAGE_TYPE_NO_IMAGE_DATA",
    "TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED_IMAGE",
    "TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR_IMAGE",
    "TGA_IMAGE_TYPE_UNCOMPRESSED_BLACK_AND_WHITE_IMAGE",
    "TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_COLOR_MAPPED_IMAGE",
    "TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_TRUE_COLOR_IMAGE",
    "TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_BLACK_AND_WHITE_IMAGE"
};

const char* TGA_COLOR_MAP_TYPE_STRINGS[] = {
    "TGA_COLOR_MAP_TYPE_NO_COLOR_MAP",
    "TGA_COLOR_MAP_TYPE_COLOR_MAP",
    "TGA_COLOR_MAP_TYPE_COUNT"
};

static uint8_t*
slurp_file(const char* filename, uint32_t* bytes_read)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open file %s: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    fseek(f, 0L, SEEK_END);
    uint32_t file_size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    uint8_t* data = (uint8_t*)malloc(file_size * sizeof(uint8_t));
    if (!data) {
        fprintf(stderr, "Could not allocate %u bytes: %s\n", file_size, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (fread(data, 1, file_size, f) != file_size) {
        fprintf(stderr, "Could not read entire file for some reason\n");
        exit(EXIT_FAILURE);
    }

    fclose(f);

    *bytes_read = file_size;
    return data;
}

static uint8_t*
read_tga_header(uint8_t* data, TgaHeader* header)
{
    header->id_length = *data++;
    header->color_map_type = (TgaColorMapType)*data++;
    header->image_type = (TgaImageType)*data++;

    TgaColorMapSpecification cms = {0};
    cms.first_entry_index = *(uint16_t*)data;
    data += 2;
    cms.color_map_length = *(uint16_t*)data;
    data += 2;
    cms.color_map_entry_size = *data++;
    header->color_map_specification = cms;
    
    TgaImageSpecification is = {0};
    is.x_origin = *(uint16_t*)data;
    data += 2;
    is.y_origin = *(uint16_t*)data;
    data += 2;
    is.width = *(uint16_t*)data;
    data += 2;
    is.height = *(uint16_t*)data;
    data += 2;
    is.pixel_depth = *data++;
    is.image_descriptor = *data++;
    header->image_specification = is;
    
    return data;
}

/*static*/ void
print_tga_header(TgaHeader header)
{    
    printf("TGA HEADER:\n");
    printf("  Id length:      %u\n", header.id_length);
    printf("  Color map type: %s\n", TGA_COLOR_MAP_TYPE_STRINGS[header.color_map_type]);
    printf("  Image type:     %s\n", TGA_IMAGE_TYPE_STRINGS[header.image_type]);
    printf("  Color map specification:\n");
    printf("    First entry index:    %u\n", header.color_map_specification.first_entry_index);
    printf("    Color map length:     %u\n", header.color_map_specification.color_map_length);
    printf("    Color map entry size: %u\n", header.color_map_specification.color_map_entry_size);
    printf("  Image specification:\n");
    printf("    X-origin:         %u\n", header.image_specification.x_origin);
    printf("    Y-origin:         %u\n", header.image_specification.y_origin);
    printf("    Width:            %u\n", header.image_specification.width);
    printf("    Height:           %u\n", header.image_specification.height);
    printf("    Pixel depth:      %u\n", header.image_specification.pixel_depth);
    printf("    Image Descriptor: %u\n", header.image_specification.image_descriptor);
}

static Image
parse_tga_image(uint8_t* data)
{
    Image image = {0};
    
    TgaHeader tga_header = {0};
    data = read_tga_header(data, &tga_header);
    
    assert(tga_header.color_map_type == TGA_COLOR_MAP_TYPE_NO_COLOR_MAP &&
           "Color maps are not supported");
    assert(tga_header.image_type == TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR_IMAGE &&
           "Only uncompressed true color images are supported");
    
    image.file_format = FILEFORMAT_TGA;
    image.width = tga_header.image_specification.width;
    image.height = tga_header.image_specification.height;

    uint8_t* image_id = data;
    data += tga_header.id_length;
    (void)image_id;

    uint8_t* color_map_data = data;
    assert(tga_header.color_map_specification.color_map_entry_size % 8 == 0 &&
           "Color map entry size in bits must be divisible by 8");
    data += tga_header.color_map_specification.color_map_length * (tga_header.color_map_specification.color_map_entry_size / 8);
    (void)color_map_data;

    uint32_t* orig_pixels = (uint32_t*)data;
    image.pixels = (uint32_t*)malloc(image.width * image.height * sizeof(uint32_t));

    bool has_alpha = tga_header.image_specification.image_descriptor & 0xF;
    uint32_t pixel_width_in_bytes = has_alpha ? 4 : 3;
    for (uint32_t y = 0; y < image.height; ++y) {
        for (uint32_t x = 0; x < image.width; ++x) {
            uint32_t i = x + y * image.width;
            uint8_t* pixel = (((uint8_t*)orig_pixels) + i*pixel_width_in_bytes);
            image.pixels[i] = 0;
            uint32_t b = pixel[0];
            uint32_t g = pixel[1];
            uint32_t r = pixel[2];
            image.pixels[i] = ((r << 24) | (g << 16) | (b << 8));
            if (has_alpha) {
                uint32_t a = pixel[3];
                image.pixels[i] |= a;
            }
        }
    }

    return image;
}

Image
read_image_from_file(const char* filepath)
{
    uint32_t file_size;
    uint8_t* file_data = slurp_file(filepath, &file_size);

    Image image = parse_tga_image(file_data);

    free(file_data);
    
    return image;
}

void
write_image_to_ppm(Image image, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Could not open file %s for writing: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    sprintf(buf, "P3\n%u %u\n255\n", image.width, image.height);
    fwrite(buf, strlen(buf), 1, f);
 
    for (uint32_t y = 0; y < image.height; ++y) {
        for (uint32_t x = 0; x < image.width; ++x) {
            uint32_t i = x + y * image.width;
            uint32_t r = (image.pixels[i] >> 24) & 0xFF;
            uint32_t g = (image.pixels[i] >> 16) & 0xFF;
            uint32_t b = (image.pixels[i] >> 8) & 0xFF;

            sprintf(buf, "%u %u %u\n", r, g, b);
            fwrite(buf, strlen(buf), 1, f);
        }
    }

    fclose(f);
}
