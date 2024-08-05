#include "pch.h"
#include "libpwn.h"

#pragma region INTERNAL
FILE* logFile = stdout;

void ValidateExpected(size_t addr, const BYTE *expected, size_t sz) {
    if (memcmp((LPVOID)addr, expected, sz) != 0) {
        fprintf(logFile, "Mismatch with expected data, aborting!\n");
        fprintf(logFile, "Expected: ");
        PrintBytes((size_t)expected, sz);
        fprintf(logFile, "Found: ");
        PrintBytes(addr, sz);
        ExitProcess(1);
    }
}

void ValidateExpected(size_t addr, const BYTE expected) {
    ValidateExpected(addr, &expected, 1);
}

void ValidateExpected(size_t addr, const size_t expected) {
    ValidateExpected(addr, (LPBYTE)&expected, sizeof(expected));
}

void ValidateExpected(size_t addr, const char *expected) {
    ValidateExpected(addr, (LPBYTE)expected, strlen(expected));
}
#pragma endregion

void SetLogFile(FILE* file) {
    logFile = file;
}

void PrintBytes(size_t addr, size_t n, size_t bytesPerLine) {
    size_t i = 0;
    BYTE byte;
    BYTE* byteAddr;
    fprintf(logFile, "%p:\n", (LPVOID)addr);
    while (i < n) {
        byteAddr = (BYTE*)addr + i;
        byte = *byteAddr;
        fprintf(logFile, "%02x ", byte);
        i++;
        if (i % bytesPerLine == 0) {
            fprintf(logFile, "\n");
        }
    }
    fprintf(logFile, "\n");
}

size_t GetBaseAddress(const char *target) {
    size_t addr = (size_t)GetModuleHandleA(target);
    if (!addr) {
        fprintf(logFile, "Failed to find loaded module %s\n", target);
    }
    else {
        fprintf(logFile, "%s at %x\n", target, addr);
    }
    return addr;
}

size_t GetExportedFnAddress(size_t exeAddr, const char *fnName) {
    LPVOID fnPtr = GetProcAddress((HMODULE)exeAddr, fnName);
    if (!fnPtr) {
        fprintf(logFile, "Failed to find function %s\n", fnName);
    }
    return (size_t)fnPtr;
}

void WriteNops(size_t startAddr, size_t endAddr, const BYTE *expected) {
    const BYTE NOP = 0x90;
    size_t sz = endAddr - startAddr;
    if (expected) {
        ValidateExpected(startAddr, expected, sz);
    }
    RtlFillMemory((LPVOID)startAddr, sz, NOP);
    fprintf(logFile, "Replaced %d bytes at %p with NOP\n", sz, (LPVOID)startAddr);
}

void WriteByte(size_t addr, BYTE val, const BYTE *expected) {
    if (expected) {
        ValidateExpected(addr, *expected);
    }
    BYTE* byteAddr = (BYTE*)addr;
    BYTE oldByte = *byteAddr;
    *byteAddr = val;
    fprintf(logFile, "Replaced byte at %p (%02x -> %02x)\n", byteAddr, oldByte, val);
}

void WriteAddress(size_t atAddr, size_t addrVal, const size_t *expected) {
    if (expected) {
        ValidateExpected(atAddr, *expected);
    }
    size_t* destAddr = (size_t*)atAddr;
    *destAddr = addrVal;
    fprintf(logFile, "*%p = %p\n", destAddr, (LPVOID)addrVal);
}

void WriteString(size_t addr, const char *newStr, const char *expected) {
    if (expected) {
        ValidateExpected(addr, expected);
    }
    char* strAddr = (char*)addr;
    strcpy(strAddr, newStr);
}

void ReplaceString(size_t addr, const char *newStr, const char *expected) {
    char* strAddr = (char*)addr;
    fprintf(logFile, "Replacing %s with %s...\n", strAddr, newStr);
    size_t oldStrLen = strlen(strAddr);
    size_t newStrLen = strlen(newStr);
    if (newStrLen > oldStrLen) {
        fprintf(logFile, "Can't replace string; %d > %d\n", newStrLen, oldStrLen);
    } else {
        WriteString(addr, newStr, expected);
        fprintf(logFile, "String replaced!\n");
    }
}
