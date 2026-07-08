sco-pe Build Guide
===================

Folder Structure
-----------------
::

    include/    header files - declare what each module exposes
    src/        source files - the actual implementation
    samples/    real PE files for testing (putty.exe, libwinpthread-1.dll)
    tests/      one test file per module (later)
    docs/       this guide + structure notes
    build/      compiled .o files (gitignored)
    bin/        final executable (gitignored)
    Makefile

Reference Links
-----------------

- winnt.h - search "winnt.h online" - source of truth for struct field names/sizes
- Corkami PE diagram - https://raw.githubusercontent.com/corkami/pics/master/binary/pe102/pe102.svg
- 0xRick's PE series - https://0xrick.github.io/win-internals/pe8/
- rioasmara's offset walkthrough - https://rioasmara.com/2021/10/16/c-code-parsing-pe-with-corkami-offset/
- Makefile tutorial - https://makefiletutorial.com/

Build Order (header + source, paired)
---------------------------------------

Each step lists the ``.h`` first, then the ``.c`` - write the header's
declarations before filling in the source's implementation, so the source
always has a contract to build against.

1. **include/pe.h**
   Declares the shared ``PEFile`` struct: buffer pointer, buffer size,
   pointer to the DOS header, pointer to NT headers, section array/count.
   Also declares the top-level function signatures used by main.c.
   No matching .c file - this header has no implementation of its own,
   it's the shared contract every other module includes.

2. **include/file.h** -> **src/file.c**
   Header declares: a function to open a file into a PEFile (reads all
   bytes into pe->buffer), and a function to close/free it.
   Source implements: fopen, fseek/ftell for size, malloc, fread, fclose.

3. **include/dos.h** -> **src/dos.c**
   Header declares: a function that takes a PEFile* and parses the DOS
   header from pe->buffer.
   Source implements: cast buffer to IMAGE_DOS_HEADER*, check e_magic is
   "MZ" (0x5A4D), extract e_lfanew, bounds-check e_lfanew against buffer
   size before anything else uses it.

4. **include/nt.h** -> **src/nt.c**
   Header declares: a function that parses the NT headers using the
   e_lfanew offset found in step 3.
   Source implements: check Signature is "PE\0\0", parse File Header
   (machine, NumberOfSections, SizeOfOptionalHeader), parse Optional
   Header, check Magic field first to detect PE32 (0x10B) vs PE32+ (0x20B)
   since the struct layout differs between them.

5. **include/utils.h** -> **src/utils.c**
   Header declares: rva_to_offset(PEFile*, DWORD rva) -> DWORD offset.
   Source implements: loop over pe->sections, find which section's
   VirtualAddress range contains the RVA, convert using that section's
   PointerToRawData. Every module after this depends on this function.

6. **include/sections.h** -> **src/sections.c**
   Header declares: a function that parses the section table into
   pe->sections.
   Source implements: locate the section table right after the Optional
   Header, loop NumberOfSections times over IMAGE_SECTION_HEADER entries,
   store name/VirtualAddress/VirtualSize/PointerToRawData/Characteristics
   for each.

7. **include/imports.h** -> **src/imports.c**
   Header declares: a function that parses the import table.
   Source implements: get the import directory RVA from
   DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT], convert with
   rva_to_offset(), walk IMAGE_IMPORT_DESCRIPTOR entries until an
   all-zero entry, resolve each DLL name, walk each DLL's thunk array
   for imported function names/ordinals.

8. **include/exports.h** -> **src/exports.c**
   Header declares: a function that parses the export table.
   Source implements: same RVA pattern as imports, but reading
   IMAGE_EXPORT_DIRECTORY - walks AddressOfFunctions, AddressOfNames,
   and AddressOfNameOrdinals (three parallel arrays). Only relevant for
   DLLs - test against libwinpthread-1.dll, not putty.exe.

9. **src/main.c** (no matching header - it's the entry point, nothing
   else includes main.c)
   Calls file -> dos -> nt -> sections -> imports -> exports in order,
   then prints a report from the fully populated PEFile struct.

Testing
-----------------

- ``samples/putty.exe`` - real EXE, tests DOS/NT/sections/imports
- ``samples/libwinpthread-1.dll`` - real DLL, tests exports

Rule of Thumb
-----------------

Every ``.c`` gets a matching ``.h`` (except main.c, which nothing includes).
The ``.h`` declares what exists - struct fields, function signatures - so
other modules can use it without seeing the implementation. The ``.c``
implements it. Bounds-check every offset read from the file before
following it - malformed files are the norm, not the exception, in this
domain.
