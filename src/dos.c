// Parses the DOS header, validates MZ signature, extracts e_lfanew
#include "dos.h"
#include "pe.h"
#include <stdio.h>

int parse_dos_header(PEFile *pe) {
    // read buffer as DOS_HEADER
    DOS_HEADER *dos = (DOS_HEADER *) pe->buffer;

    // check magic
    if (dos->e_magic != 0x5A4D) {
        printf("Error: not a valid PE file (bad MZ signature)\n");
        return 1;
    }

    // check e_lfanew bounds
    if (dos->e_lfanew <= 0 || (long)dos->e_lfanew >= pe->size) {
        printf("Error: e_lfanew is out of bounds\n");
        return 1;
    }

    pe->dos_header = dos;
    return 0;
}