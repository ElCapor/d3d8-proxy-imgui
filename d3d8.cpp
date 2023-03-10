#include <windows.h>
#include <stdio.h>
#include "hook_macro.h"

HINSTANCE mHinst = 0, mHinstDLL = 0;

UINT_PTR mProcs[4] = {0};

LPCSTR mImportNames[] = {
  "DebugSetMute",
  "Direct3DCreate8",
  "ValidatePixelShader",
  "ValidateVertexShader",
};

#ifndef _DEBUG
inline void log_info(const char* info) {
}
#else
FILE* debug;
inline void log_info(const char* info) {
  fprintf(debug, "%s\n", info);
  fflush(debug);
}
#endif

#include "empty.h"

inline void _hook_setup() {
#ifdef DEBUGSETMUTE
  DebugSetMute_real = (DebugSetMute_ptr)mProcs[0];
  mProcs[0] = (UINT_PTR)&DebugSetMute_fake;
#endif
#ifdef DIRECT3DCREATE8
  Direct3DCreate8_real = (Direct3DCreate8_ptr)mProcs[1];
  mProcs[1] = (UINT_PTR)&Direct3DCreate8_fake;
#endif
#ifdef VALIDATEPIXELSHADER
  ValidatePixelShader_real = (ValidatePixelShader_ptr)mProcs[2];
  mProcs[2] = (UINT_PTR)&ValidatePixelShader_fake;
#endif
#ifdef VALIDATEVERTEXSHADER
  ValidateVertexShader_real = (ValidateVertexShader_ptr)mProcs[3];
  mProcs[3] = (UINT_PTR)&ValidateVertexShader_fake;
#endif
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  mHinst = hinstDLL;
  if (fdwReason == DLL_PROCESS_ATTACH) {
    mHinstDLL = LoadLibrary("real_d3d8.dll");
    if (!mHinstDLL) {
      return FALSE;
    }
    for (int i = 0; i < 4; ++i) {
      mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);
    }
    _hook_setup();
#ifdef _DEBUG
    debug = fopen("./debug.log", "a");
#endif
  } else if (fdwReason == DLL_PROCESS_DETACH) {
#ifdef _DEBUG
    fclose(debug);
#endif
    FreeLibrary(mHinstDLL);
  }
  return TRUE;
}

extern "C" __declspec(naked) void __stdcall DebugSetMute_wrapper(){
#ifdef _DEBUG
  log_info("calling DebugSetMute");
#endif
  __asm{jmp mProcs[0 * 4]}
}
extern "C" __declspec(naked) void __stdcall Direct3DCreate8_wrapper(){
#ifdef _DEBUG
  log_info("calling Direct3DCreate8");
#endif
  __asm{jmp mProcs[1 * 4]}
}
extern "C" __declspec(naked) void __stdcall ValidatePixelShader_wrapper(){
#ifdef _DEBUG
  log_info("calling ValidatePixelShader");
#endif
  __asm{jmp mProcs[2 * 4]}
}
extern "C" __declspec(naked) void __stdcall ValidateVertexShader_wrapper(){
#ifdef _DEBUG
  log_info("calling ValidateVertexShader");
#endif
  __asm{jmp mProcs[3 * 4]}
}
