# AESDumpster
AES Dumpster 1.3 Online Version:  [https://illusory.dev/aesdumpster/](https://illusory.dev/aesdumpster/) <br>

Find Android AES Keys (IDA Pro Scripts):<br>
-arm64-v8a: https://github.com/GHFear/find-ue4-aes-key-arm64-v8a<br>
-armeabi-v7a: https://github.com/GHFear/find-ue4-aes-key-armeabi-v7a<br>
-armeabi-v7a-type2: https://github.com/GHFear/find-ue4-aes-key-armeabi-v7a-type2

## Linux
Build (g++ / make):
```
make
```
Usage (works on Windows PE executables, same as the Windows build):
```
./aesdumpster <path-to-exe> [more exes...]
```
Cross-compile a Windows exe with mingw-w64 installed: `make windows`.

## Windows
Build `AESDumpster/AESDumpster.sln` with Visual Studio 2022, or drag and drop Unreal Engine executables onto a release build of `AESDumpster.exe`.
