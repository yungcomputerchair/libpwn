# libpwn

Static library for reading and modifying x86 process data at runtime.

## How to use

Compile the library and include the .lib and .h files in your project.

## Notes
- The functions provided by `libpwn` do not modify page protections. The uesr must set protections appropriately before calling them. See the demo project below for an example.
- You alone are responsible for anything you do with this library. Ensure you are not violating the terms of use of any software you use this library on.

## Demo

The included `TestHookDll` project shows an example DLL that uses the library to patch data & instructions from `TestTargetApp` (a simple Win32 console application, project also included).

![Screenshot 2024-08-04 155014](https://github.com/user-attachments/assets/4962d66d-323a-4b82-ae9a-fe32044f3e98)
