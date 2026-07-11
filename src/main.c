// Entry point: calls each parsing step in order, then prints a report
#include <stdio.h>
#include "pe.h"
#include "file.h"
#include "dos.h"

int main(void) {
    PEFile pe;
    const char *target = "samples/putty.exe";

    printf("========================================\n");
    printf(" sco-pe - PE File Parser\n");
    printf("========================================\n");
    printf("Target: %s\n\n", target);

    // open file, read bytes into pe.buffer
    if (pe_open(target, &pe) != 0) {
        printf("Failed to open file\n");
        return 1;
    }

    // parse DOS header, get e_lfanew
    if (parse_dos_header(&pe) != 0) {
        printf("Failed to parse DOS header\n");
        pe_close(&pe);
        return 1;
    }

    printf("[ File ]\n");
    printf("  %-20s %ld bytes\n", "Size:", pe.size);
    printf("\n[ DOS Header ]\n");
    printf("  %-20s 0x%X\n", "e_lfanew:", pe.dos_header->e_lfanew);

    // free the buffer, we're done
    pe_close(&pe);
    printf("\n========================================\n");
    printf(" Done.\n");
    printf("========================================\n");

    return 0;
}