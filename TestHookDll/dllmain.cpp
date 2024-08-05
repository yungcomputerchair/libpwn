#include "pch.h"
#include "../libpwn.h"

/*
* N.B. all hardcoded offsets and sizes were determined using Ghidra
* on a RELEASE build of TestTargetApp. This example will NOT work in Debug mode!
*/

// Replaces the "Hello world!" string in the data section with "pwnd"
// so the target program prints "pwnd" on the first call to printf
void PatchHelloWorldString(size_t rdataAddr) {
    const size_t STR_OFFSET = 0x130;

    size_t strAddr = rdataAddr + STR_OFFSET;
    ReplaceString(strAddr, "pwnd\n", "Hello world!\n");
}

// Prints the bytes that make up the main function in the text section
// and returns the address of the function
size_t GetMainFnAddr(size_t textAddr) {
    const size_t MAIN_FN_OFFSET = 0x40;
    const size_t MAIN_FN_SIZE = 0x53;

    size_t mainFnAddr = textAddr + MAIN_FN_OFFSET;
    PrintBytes(mainFnAddr, MAIN_FN_SIZE);
    return mainFnAddr;
}

// Patch out the second call to printf by replacing the push and call instructions
// with a NOP sled, and adjusting the stack cleanup accordingly
void PatchOutHelloWorld2(size_t mainAddr) {
    const size_t CALL_START_OFFSET = 0x26;
    const size_t CALL_END_OFFSET = 0x30;
    const size_t STACK_RESTORE_SZ_OFFSET = 0x3c;
    const BYTE STACK_RESTORE_SZ_EXPECTED = 0xc;

    size_t startAddr = mainAddr + CALL_START_OFFSET;
    size_t endAddr = mainAddr + CALL_END_OFFSET;
    WriteNops(startAddr, endAddr);

    size_t stackRestoreSizeAddr = mainAddr + STACK_RESTORE_SZ_OFFSET;
    WriteByte(stackRestoreSizeAddr, 0x8, &STACK_RESTORE_SZ_EXPECTED);
}

// Replace the parameter in the third call to printf
// with the address of our own string
char NEW_EVIL_STRING[] = "Hello from evil DLL >:)\n";
void ChangeHelloWorld3Arg(size_t mainAddr) {
    const size_t PUSH_TARGET_OFFSET = 0x31;
    size_t pushTargetAddr = mainAddr + PUSH_TARGET_OFFSET;
    WriteAddress(pushTargetAddr, (size_t)NEW_EVIL_STRING);
}

// Replace the call to GetTickCount with a call to our
// own function with identical signature.
// The call instruction in the binary uses a level of
// indirection, so we need to point to a function table
// that holds the actual address of our evil function
DWORD EvilGetTickCount() {
    return 0xDEADBEEF;
}
size_t EVIL_FN_TABLE[] = { (size_t)EvilGetTickCount };
void HookGetTickCount(size_t mainAddr) {
    const size_t CALL_TARGET_OFFSET = 0x3f;
    size_t callTargetAddr = mainAddr + CALL_TARGET_OFFSET;
    WriteAddress(callTargetAddr, (size_t)EVIL_FN_TABLE);
}

void DoEvil() {
    const size_t RDATA_OFFSET = 0x2000;
    const size_t TEXT_OFFSET = 0x1000;
    //
    const size_t RDATA_SIZE = 0x1000;
    const size_t TEXT_SIZE = 0x1000;
    //
    size_t exeAddr;
    size_t rdataAddr;
    size_t textAddr;
    //
    size_t mainFnAddr;
    //
    DWORD oldPerms;

    // Get the real virtual address of the target binary and
    // use it to calculate the real virtual address of each section we care about
    exeAddr = GetBaseAddress("TestTargetApp.exe");
    rdataAddr = exeAddr + RDATA_OFFSET;
    textAddr = exeAddr + TEXT_OFFSET;

    // Make all sections we want to modify writable
    VirtualProtect((LPVOID)rdataAddr, RDATA_SIZE, PAGE_READWRITE, &oldPerms);
    VirtualProtect((LPVOID)textAddr, TEXT_SIZE, PAGE_EXECUTE_READWRITE, &oldPerms);

    // Do some evil
    PatchHelloWorldString(rdataAddr);
    mainFnAddr = GetMainFnAddr(textAddr);
    PatchOutHelloWorld2(mainFnAddr);
    ChangeHelloWorld3Arg(mainFnAddr);
    HookGetTickCount(mainFnAddr);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        printf("Dll init\n");
        DoEvil();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

