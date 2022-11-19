#ifndef _IMAGE_LOADER_H
#define _IMAGE_LOADER_H

#include <stdint.h>

typedef enum _FileFormat {
    FILEFORMAT_TGA = 0,
    FILEFORMAT_COUNT
} FileFormat;

typedef enum _TgaColorMapType {
    TGA_COLOR_MAP_TYPE_NO_COLOR_MAP = 0,
    TGA_COLOR_MAP_TYPE_COLOR_MAP = 1,
} TgaColorMapType;

typedef enum _TgaImageType {
    TGA_IMAGE_TYPE_NO_IMAGE_DATA = 0,
    TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED_IMAGE = 1,
    TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR_IMAGE = 2,
    TGA_IMAGE_TYPE_UNCOMPRESSED_BLACK_AND_WHITE_IMAGE = 3,
    TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_COLOR_MAPPED_IMAGE = 9,
    TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_TRUE_COLOR_IMAGE = 10,
    TGA_IMAGE_TYPE_RUN_LENGTH_ENCODED_BLACK_AND_WHITE_IMAGE = 11,
} TgaImageType;

typedef struct _TgaColorMapSpecification {
    uint16_t first_entry_index;
    uint16_t color_map_length;
    uint8_t  color_map_entry_size;
} TgaColorMapSpecification;

typedef struct _TgaImageSpecification {
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;
    uint8_t pixel_depth;
    uint8_t image_descriptor;
} TgaImageSpecification;

typedef struct _TgaHeader {
    uint8_t id_length;
    TgaColorMapType color_map_type;
    TgaImageType image_type;
    TgaColorMapSpecification color_map_specification;
    TgaImageSpecification image_specification;
} TgaHeader;

typedef struct _Image {
    FileFormat file_format;
    uint32_t width;
    uint32_t height;
    uint32_t* pixels;
} Image;

Image read_image_from_file(const char* filepath);
void write_image_to_ppm(Image image, const char* filename);

#endif // _IMAGE_LOADER_H
