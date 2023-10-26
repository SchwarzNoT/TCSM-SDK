#include "process.h"

DWORD procID;
uintptr_t modBase;
HANDLE hProc;
HWND targetWindow;
RECT windowRect;
windowProperties winProperties;




uintptr_t GNames = 0x5882D00;
uintptr_t GObjects = 0x58BF020;

clock_t winUpdateTimer = -6000;
clock_t checkProcActiveTime = -6000;

namespace util {


    

    DWORD getProcID(const char* procName) {

        DWORD procID = 0;
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(PROCESSENTRY32);
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (Process32First(hSnap, &procEntry)) {


            do {


                if (!strcmp(procName, procEntry.szExeFile)) {


                    procID = procEntry.th32ProcessID;
                    break;

                }


            } while (Process32Next(hSnap, &procEntry));


        }
        if (!procID) {


            printf("Failed To Locate Process With Name \"%s\" ", procName);
        }


        return procID;

    }

    uintptr_t GetModuleBaseAddress(DWORD ProcID, const char* procName) {

        uintptr_t modBaseAddr = 0;

        HANDLE hSnap = 0;

        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);

        MODULEENTRY32 modEntry;

        modEntry.dwSize = sizeof(modEntry);

        if (Module32First(hSnap, &modEntry)) {

            do {

                if (!strcmp(modEntry.szModule, procName)) {


                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;

                }
            } while (Module32Next(hSnap, &modEntry));


        }
        CloseHandle(hSnap);
        return modBaseAddr;

    }
    template<typename T> T Read(uintptr_t address, int size) {


        T buffer;

        ReadProcessMemory(hProc, (BYTE*)address, &buffer, size, NULL);

        return buffer;


    }

    template<typename T> bool Write(uintptr_t address, T toWrite) {


        return WriteProcessMemory(hProc, (BYTE*)address, &toWrite, sizeof(toWrite), NULL);

    }


    bool updateWindow() {

      //  targetWindow = FindWindow("UnrealWindow", "FPS Chess  ");

        clock_t tempTime = clock();
        if (tempTime >= winUpdateTimer + 5000) {

            if (targetWindow) {


                LONG_PTR windowStyle = GetWindowLongPtr(targetWindow, GWL_STYLE);

                GetWindowRect(targetWindow, &windowRect);

                if (windowStyle & WS_CAPTION) {

                    winProperties.bWindowed = true;
                    winProperties.width = (float)(windowRect.right - windowRect.left );
                    winProperties.height = float(windowRect.bottom - windowRect.top+19.5f);
                    winProperties.x = (float)(windowRect.left);
                    winProperties.y = (float)windowRect.top;
                }
                else {

                    winProperties.bWindowed = false;
                    winProperties.width = (float)(windowRect.right - windowRect.left);
                    winProperties.height = float(windowRect.bottom - windowRect.top);
                    winProperties.x = (float)(windowRect.left);
                    winProperties.y = (float)windowRect.top;

                }




                winUpdateTimer = tempTime;

                return true;
            }

        }
        return false;

    }


    bool init() {

        procID = getProcID("BBQClient-WinGDK-Shipping.exe");
        if (procID) {

            modBase = GetModuleBaseAddress(procID, "BBQClient-WinGDK-Shipping.exe");
            hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

            GNames += modBase;
            GObjects += modBase;
            targetWindow = FindWindow("UnrealWindow", "The Texas Chain Saw Massacre  ");



            return true;
        }
        else { return false; }
    }


    D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
    {
        D3DMATRIX pOut;
        pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
        pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
        pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
        pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
        pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
        pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
        pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
        pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
        pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
        pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
        pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
        pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
        pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
        pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
        pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
        pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

        return pOut;
    }


    void toRead() {

        Read<uintptr_t>(1, 0x0);
        Read<INT32>(1, 0x0);
        Read<int>(1, 0x0);
        Read<float>(1, 0x0);
        Read<FVector>(1, 0x0);
        Read<INT64>(1, 0x0);
        Read<short>(1, 0x0);
        Read<bool>(1, 0x0);
        Read<FMinimalViewInfo>(1, 0x0);
        Read<Vector3>(1, 0x0);
        Read<FTransform>(1, 0x0);

        Write<float>(0x0, 1);
        Write<int>(0x0, 1);



    }

    int setAimKey() {


        int key = -1;
        while (key == -1) {


            for (int i = 0; i < 256; i++) {


                if (GetAsyncKeyState(i) & 0x8000) {


                    return i;

                }



            }
        }
        return -1;


    }
    bool isActive() {

        clock_t tempTime = clock();

        if (tempTime >= checkProcActiveTime + 6000) {
                

            DWORD active;
            GetExitCodeProcess(hProc, &active);

            if (!active) {
                return false;
            }
            else {
                return true;
            }
        }
        else return true;

    }



}