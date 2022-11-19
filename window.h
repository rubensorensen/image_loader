#ifndef _WINDOW_H
#define _WINDOW_H

#include "image_loader.h"

#include <stdint.h>
#include <stdbool.h>

bool window_create(uint32_t width, uint32_t height);
bool window_update(uint32_t* pixels, uint32_t width, uint32_t height);
void window_destroy(void);

#endif // _WINDOW_H
