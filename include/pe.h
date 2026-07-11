// Declares the PEFile struct and top-level parsing API

#ifndef PE_H
#define PE_H

// Minimal DOS header - only the two fields we actually use
typedef struct {
    unsigned short e_magic;      // 2 bytes, offset 0x00 - should be "MZ" (0x5A4D)
    unsigned char  reserved[58]; // padding, offset 0x02 to 0x3B, unused
    unsigned int   e_lfanew;     // 4 bytes, offset 0x3C - offset to NT headers
} DOS_HEADER;

// Holds everything discovered about a PE file as parsing proceeds
typedef struct {
    unsigned char *buffer;       // raw file bytes
    long size;                    // number of bytes in buffer
    DOS_HEADER *dos_header;       // points into buffer, filled in by dos.c
} PEFile;

#endif // PE_H