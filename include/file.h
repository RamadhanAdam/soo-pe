// Declares file open/read/close functions

#ifndef FILE_H
#define FILE_H
#include "pe.h"

// Opens the file at 'filename' and reads its full contents into pe-> buffer
// Fills in the pe->size with the number of bytes read
// Returns 0 on success, non-zero on failure
int pe_open(const char *filename, PEFile *pe);

// Free memory allocated in pe->bufer by pe_open
void pe_close(PEFile *pe);

#endif // FILE_H
