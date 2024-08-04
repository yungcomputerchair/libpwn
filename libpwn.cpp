#include "pch.h"
#include <stdio.h>

void PrintBytes(size_t addr, size_t n) {
    const size_t BYTES_PER_LINE = 16;

    size_t i = 0;
    BYTE byte;
    BYTE* byteAddr;
    printf("%p:\n", (LPVOID)addr);
    while (i < n) {
        byteAddr = (BYTE*)addr + i;
        byte = *byteAddr;
        printf("%02x ", byte);
        i++;
        if (i % BYTES_PER_LINE == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

size_t GetBaseAddress(const char* target) {
    size_t addr = (size_t)GetModuleHandleA(target);
    if (!addr) {
        printf("Failed to find loaded module %s\n", target);
    }
    else {
        printf("%s at %x\n", target, addr);
    }
    return addr;
}

size_t GetExportedFnAddress(size_t exeAddr, const char* fnName) {
    LPVOID fnPtr = GetProcAddress((HMODULE)exeAddr, fnName);
    if (!fnPtr) {
        printf("Failed to find function %s\n", fnName);
    }
    return (size_t)fnPtr;
}

void WriteNops(size_t startAddr, size_t endAddr) {
    const BYTE NOP = 0x90;
    size_t sz = endAddr - startAddr;
    RtlFillMemory((LPVOID)startAddr, sz, NOP);
    printf("Replaced %d bytes at %p with NOP\n", sz, (LPVOID)startAddr);
}

void WriteByte(size_t addr, BYTE val) {
    BYTE* byteAddr = (BYTE*)addr;
    BYTE oldByte = *byteAddr;
    *byteAddr = val;
    printf("Replaced byte at %p (%02x -> %02x)\n", byteAddr, oldByte, val);
}

void WriteAddress(size_t atAddr, size_t addrVal) {
    size_t* destAddr = (size_t*)atAddr;
    *destAddr = addrVal;
    printf("*%p = %p\n", destAddr, (LPVOID)addrVal);
}

void WriteString(size_t addr, const char* newStr) {
    char* strAddr = (char*)addr;
    strcpy(strAddr, newStr);
}

void ReplaceString(size_t addr, const char* newStr) {
    char* strAddr = (char*)addr;
    printf("Replacing %s with %s...\n", strAddr, newStr);
    size_t oldStrLen = strlen(strAddr);
    size_t newStrLen = strlen(newStr);
    if (newStrLen > oldStrLen) {
        printf("Can't replace string; %d > %d\n", newStrLen, oldStrLen);
    }
    else {
        WriteString(addr, newStr);
        printf("String replaced!\n");
    }
}
