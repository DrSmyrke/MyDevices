#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdint.h>
#include <stddef.h>

//-------------------------------------------------------------------------------
void handleRoot(void);
void handleGet(void);
void handleUpdate(void);
void writeProcess(uint8_t *data, const size_t size);

//-------------------------------------------------------------------------------



#endif // __HTTP_H__