#pragma once
#include "precomp.h"


namespace util {

    template<typename T> T Read(uintptr_t address, int size);

    template<typename T> bool Write(uintptr_t address, T toWrite);
    DWORD getProcID(const char* procName);

    uintptr_t GetModuleBaseAddress(DWORD ProcID, const char* procName);

    bool updateWindow();

    bool init();

    void toRead();

    int setAimKey();

    bool isActive();

    D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2);

  
}