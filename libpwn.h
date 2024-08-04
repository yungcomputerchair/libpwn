#pragma once
#include "pch.h"

// Pretty-prints n bytes
void PrintBytes(size_t addr, size_t n);

// Gets the base address of the specified module if it's loaded.
// Returns 0 if the module is not loaded
size_t GetBaseAddress(const char* target);

// Gets the address of an exported function from the module
// with base address exeAddr. Returns 0 if the function cannot be found
size_t GetExportedFnAddress(size_t exeAddr, const char* fnName);

// Replaces all bytes in [startAddr, endAddr) with one-byte NOP
void WriteNops(size_t startAddr, size_t endAddr);

// Replaces the byte at addr with the specified value
void WriteByte(size_t addr, BYTE val);

// Writes a C-string to addr
void WriteString(size_t addr, const char* newStr);

// Writes a C-string to addr, assuming there is a string there already.
// If the existing string is too small, a warning is printed and the string is not replaced.
void ReplaceString(size_t addr, const char* newStr);
