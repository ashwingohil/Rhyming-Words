#include <cstdlib>

#ifndef DYNARR_H_
#define DYNARR_H_

//C++ casting required when calling main functions dynarr_alloc and dynarr_resize
void *dynarr_alloc(int elements, int szofelem);
void *dynarr_resize(void *da, int elements);
int dynarr_elements(void *da);
size_t dynarr_size(void *da);
bool dynarr_status(void *da);
void dynarr_headerinfo(void *da);
void dynarr_free(void *da);

#endif	/* DYNARR_H_ */
