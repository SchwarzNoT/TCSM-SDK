#include "engine.h"

uintptr_t UWorld;
uintptr_t Levels;
INT32 levelCount;
uintptr_t persistentLevel;
uintptr_t GameInstance;
uintptr_t GameState;
uintptr_t localPlayers;
uintptr_t localPlayer;
uintptr_t localPlayerController;
uintptr_t localPlayerState;
uintptr_t localPlayerPiece;

std::vector<AActor> targetPawns;
std::vector<AActor> itemActors;
uintptr_t kovGameState;
uintptr_t playerArray;


AActor localPiece;
int currAimPiecIndex;


ImVec2 boxDimensions;

uintptr_t playerCamManager;
CameraCacheEntry cameraCache;

clock_t worldUpdateTimer = -4000;
clock_t localPlayerUpdateTimer = -4000;
clock_t pieceBaseUpdateTimer = -4000;
clock_t pieceUpdateTimer = -2000;
clock_t itemUpdateTimer = -5000;


std::string GetNameFromFName(int key)
{
    unsigned int chunkOffset = (unsigned int)((int)(key) >> 16);
    unsigned short nameOffset = (unsigned short)key;
    
    UINT64 namePoolChunk = util::Read<UINT64>(GNames + ((chunkOffset + 2) * 8), sizeof(UINT64));
    UINT64 entryOffset = namePoolChunk + (unsigned long)(2 * nameOffset);

    INT16 nameEntry = util::Read<INT16>(entryOffset, sizeof(INT16));

    UINT16 nameLength = nameEntry >> 6;

    char* buf = new char[nameLength];
    SecureZeroMemory(buf, nameLength);

    ReadProcessMemory(hProc, (BYTE*)(entryOffset + 2), buf, nameLength, NULL);

    std::string name = buf;
    delete[] buf;
    name = name.substr(0, nameLength);
    return name;
}

std::vector<AActor> getAActors(uintptr_t levelBaseAddress) {

    uintptr_t pAActors = util::Read<uintptr_t>((levelBaseAddress + 0x98), sizeof(pAActors));

    int actorCount = util::Read<int>((levelBaseAddress + 0xA0), sizeof(actorCount));
    
    std::vector<AActor> AActors(actorCount);

    for (int i = 0; i < actorCount; i++) {

        AActors[i].baseAddress = util::Read<uintptr_t>((pAActors + (8 * i)), sizeof(AActors[i].baseAddress));

        AActors[i].FNameID = util::Read<INT64>((AActors[i].baseAddress + 0x18), sizeof(AActors[i].FNameID));

        AActors[i].Name = GetNameFromFName(AActors[i].FNameID);

        AActors[i].RootComponent.RelativeLocation = util::Read<Vector3>(((util::Read<uintptr_t>((AActors[i].baseAddress + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(AActors[i].RootComponent.RelativeLocation));


    }

    return AActors;

}

bool updateWorld(bool forceUpdate) {
    

    uintptr_t buffer = UWorld;

    clock_t tempTime = clock();
    if (tempTime >= worldUpdateTimer + 4000) {

        UWorld = util::Read<uintptr_t>(GWorld + modBase, sizeof(UWorld));


        if (UWorld != buffer && UWorld != 0x0 || forceUpdate) {

            Levels = util::Read<uintptr_t>(UWorld + offsets::UWorld::Levels, sizeof(Levels));

            levelCount = util::Read<INT32>(UWorld + 0x140, sizeof(levelCount));

            persistentLevel = util::Read<uintptr_t>((UWorld + offsets::UWorld::PersistentLevel), sizeof(persistentLevel));

            GameInstance = util::Read<uintptr_t>((UWorld + offsets::UWorld::OwningGameInstance), sizeof(GameInstance));

            GameState = util::Read<uintptr_t>((UWorld + offsets::UWorld::GameState), sizeof(GameState));

            localPlayers = util::Read<uintptr_t>((GameInstance + 0x38), sizeof(localPlayers));

            localPlayer = util::Read<uintptr_t>(localPlayers, sizeof(localPlayer));



            worldUpdateTimer = tempTime;

            return true;

        }
       
            
       
    }
    return false;
}



bool updateLocalPlayer() {

    clock_t tempTime = clock();

    if (tempTime >= localPlayerUpdateTimer + 3000) {

        localPlayerController = util::Read<uintptr_t>(localPlayer + 0x30, sizeof(localPlayerController));

        localPlayerState = util::Read<uintptr_t>((localPlayerController + 0x230), sizeof(localPlayerState));

        localPlayerPiece = util::Read<uintptr_t>((localPlayerState + 0x288), sizeof(localPlayerPiece));

        localPlayerUpdateTimer = tempTime;
        return true;
    }
    return false;

}



bool updateCameraCache() {

    playerCamManager = util::Read<uintptr_t>(localPlayerController + 0x2C0, sizeof(playerCamManager));

    uintptr_t cameraCachePrivate = playerCamManager + 0x1AF0;

    cameraCache.POV = util::Read<FMinimalViewInfo>(cameraCachePrivate + 0x10, sizeof(FMinimalViewInfo));

    return true;

}

float getDistance(Vector3 localPos, Vector3 enemyPos) {


    float xDiff = (localPos.x - enemyPos.x);
    float yDiff = (localPos.y - enemyPos.y);
    float zDiff = (localPos.z - enemyPos.z);
  
    return sqrt((xDiff * xDiff) + (yDiff * yDiff) + (zDiff * zDiff));



}

float get2DDistance(Vector3 W2S, Vector3 screenCenter) {


    float xDiff = (W2S.x - screenCenter.x);
    float yDiff = (W2S.y - screenCenter.y);
    return sqrt((xDiff * xDiff) + (yDiff * yDiff));

}

std::string getPlayerName(AActor player) {



    int nameLength = util::Read<int>((player.playerState + 0x310), sizeof(nameLength));

    if (nameLength <= 0) {

        return player.playerName;
    }

    wchar_t* buffer = new wchar_t[nameLength*2];
    SecureZeroMemory(buffer, nameLength*2);

    uintptr_t nameAddr = util::Read<uintptr_t>((player.playerState + 0x308), sizeof(nameAddr));

    ReadProcessMemory(hProc, (BYTE*)(nameAddr), buffer, nameLength*2, NULL);

    std::wstring ws(buffer);
    std::string str(ws.begin(), ws.end());
    delete[] buffer;
    
    return str;



}

void aimbot(int vKey, float smoothX, float smoothY, AActor enemy) {
    
   

    if (GetAsyncKeyState(vKey)) {
 /*
     

        float atanYX;
        float asinXY;
       

      



            atanYX = atan2(enemy.RootComponent.RelativeLocation.y - localPiece.RootComponent.RelativeLocation.y, enemy.RootComponent.RelativeLocation.x - localPiece.RootComponent.RelativeLocation.x);
            asinXY = asin((enemy.RootComponent.RelativeLocation.z - localPiece.RootComponent.RelativeLocation.z) / getDistance(localPiece.RootComponent.RelativeLocation, enemy.RootComponent.RelativeLocation));

            */

        float currValY = util::Read<float>(localPlayerController + 0x288 + 0x4, sizeof(currValY));
        float currValX = util::Read<float>(localPlayerController + 0x288, sizeof(currValX));
        float atanYX;
        float asinXY;





        atanYX = atan2(enemy.RootComponent.RelativeLocation.y - cameraCache.POV.Location.y, enemy.RootComponent.RelativeLocation.x - cameraCache.POV.Location.x);
        asinXY = asin((enemy.RootComponent.RelativeLocation.z - cameraCache.POV.Location.z) / getDistance(cameraCache.POV.Location, enemy.RootComponent.RelativeLocation));

        

        float yChange = atanYX * (180.f / 3.1415926535897932384626433832795);
        float xChange = asinXY * (180.f / 3.1415926535897932384626433832795);


        float bufferY = yChange - currValY;

        float bufferX = xChange - currValX;


        if (abs(bufferY) >= 180.f) {

            bufferY = 360 - abs(bufferY);

        }
        if (abs(bufferX) >= 250) {

            bufferX = 360 - abs(bufferX);
        }


        util::Write<float>(localPlayerController + 0x420, bufferX/smoothX);
        util::Write<float>(localPlayerController + 0x420 + 0x4, bufferY/smoothY);
    }

}

D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
    float radPitch = (rot.x * float(3.1415926535897932384626433832795) / 180.f);
    float radYaw = (rot.y * float(3.1415926535897932384626433832795) / 180.f);
    float radRoll = (rot.z * float(3.1415926535897932384626433832795) / 180.f);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);

    D3DMATRIX matrix;
    matrix.m[0][0] = CP * CY;
    matrix.m[0][1] = CP * SY;
    matrix.m[0][2] = SP;
    matrix.m[0][3] = 0.f;

    matrix.m[1][0] = SR * SP * CY - CR * SY;
    matrix.m[1][1] = SR * SP * SY + CR * CY;
    matrix.m[1][2] = -SR * CP;
    matrix.m[1][3] = 0.f;

    matrix.m[2][0] = -(CR * SP * CY + SR * SY);
    matrix.m[2][1] = CY * SR - CR * SP * SY;
    matrix.m[2][2] = CR * CP;
    matrix.m[2][3] = 0.f;

    matrix.m[3][0] = origin.x;
    matrix.m[3][1] = origin.y;
    matrix.m[3][2] = origin.z;
    matrix.m[3][3] = 1.f;

    return matrix;
}


Vector3 WorldToScreen(Vector3 WorldLocation, CameraCacheEntry CameraCacheL)
{
    Vector3 Screenlocation = Vector3(0, 0, 0);

    auto POV = CameraCacheL.POV;

    Vector3 Rotation = POV.Rotation; // FRotator

    D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

    Vector3 vAxisX, vAxisY, vAxisZ;

    vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
    vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
    vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

    Vector3 vDelta = WorldLocation - POV.Location;
    Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

    if (vTransformed.z < 1.f)
        vTransformed.z = 1.f;

    float FovAngle = POV.FOV;

    float ScreenCenterX =  (winProperties.width/ 2.0f);
    float ScreenCenterY =  (winProperties.height/ 2.0f);
  
    Screenlocation.x = (ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795/ 360.f)) / vTransformed.z)+winProperties.x;
    Screenlocation.y = (ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795 / 360.f)) / vTransformed.z)+winProperties.y;
    return Screenlocation;
}



bool refreshPawns() {

    clock_t tempTime = clock();
    if (tempTime >= pieceUpdateTimer + 4000) {

        SecureZeroMemory(&targetPawns, sizeof(targetPawns));

       
        INT32 playerSize = util::Read<INT32>(GameState + 0x248, sizeof(playerSize));
        playerArray = util::Read<uintptr_t>(GameState + 0x240, sizeof(playerArray));


        targetPawns = std::vector<AActor>(playerSize);


        for (int i = 0; i < targetPawns.size(); i++) {


            targetPawns[i].playerState = util::Read<uintptr_t>(playerArray + (i * 0x8), sizeof(targetPawns[i].playerState));

            targetPawns[i].baseAddress = util::Read<uintptr_t>(targetPawns[i].playerState + 0x288, sizeof(targetPawns[i].baseAddress));

            targetPawns[i].attributeSet = util::Read<uintptr_t>(targetPawns[i].baseAddress + 0x4D8, sizeof(targetPawns[i].attributeSet));

            targetPawns[i].meshAddress = util::Read<uintptr_t>(targetPawns[i].baseAddress + 0x288, sizeof(targetPawns[i].meshAddress));

            targetPawns[i].boneArrayAddr = util::Read<uintptr_t>(targetPawns[i].meshAddress + 0x4c0, sizeof(targetPawns[i].boneArrayAddr));

            targetPawns[i].health = util::Read<float>(targetPawns[i].attributeSet + 0x38+0xC, sizeof(targetPawns[i].health));
                
            targetPawns[i].teamID = util::Read<bool>(targetPawns[i].baseAddress + 0x5A0, sizeof(bool));

            targetPawns[i].FNameID = util::Read<INT64>((targetPawns[i].baseAddress + 0x18), sizeof(targetPawns[i].FNameID));

            targetPawns[i].Name = GetNameFromFName(targetPawns[i].FNameID);

            targetPawns[i].playerName = getPlayerName(targetPawns[i]);

            targetPawns[i].RootComponent.RelativeLocation = util::Read<Vector3>(((util::Read<uintptr_t>((targetPawns[i].baseAddress + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(targetPawns[i].RootComponent.RelativeLocation));

            targetPawns[i].W2S = WorldToScreen(targetPawns[i].RootComponent.RelativeLocation, cameraCache);




            if (targetPawns[i].baseAddress == localPlayerPiece) {

                localPiece = targetPawns[i];

            }

            targetPawns[i].Distance = getDistance(cameraCache.POV.Location, targetPawns[i].RootComponent.RelativeLocation);

            if (!targetPawns[i].skeleton.size()) {

                std::vector<BoneVector3> upper;
                std::vector<BoneVector3> right;

                std::vector<BoneVector3> left;
                std::vector<BoneVector3> spineBones;
                std::vector<BoneVector3> lowerright;
                std::vector<BoneVector3> lowerleft;

                if (!strcmp("BP_BBQPlayerVictimAna_C", targetPawns[i].Name.c_str()))
                {

                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };


                }
                else if (!strcmp("BP_BBQPlayerVictimLeland_C", targetPawns[i].Name.c_str()))
                {

                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };



                }
                else if (!strcmp("BP_BBQPlayerVictimConnie_C", targetPawns[i].Name.c_str()))
                {

                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };



                }
                else if (!strcmp("BP_BBQPlayerVictimAna_C", targetPawns[i].Name.c_str()))
                {
                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };



                }
                else if (!strcmp("BP_BBQPlayerVictimJulie_C", targetPawns[i].Name.c_str()))
                {
                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };



                }
                else if (!strcmp("BP_BBQPlayerVictimSonny_C", targetPawns[i].Name.c_str()))
                {

                    upper = { BoneVector3(0,0,0,8),		BoneVector3(0,0,0,7) };
                    right = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,78),		BoneVector3(0,0,0, 127), BoneVector3(0,0,0, 129) };

                    left = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,46),		BoneVector3(0,0,0, 126), BoneVector3(0,0,0, 37) };
                    spineBones = { BoneVector3(0,0,0,7),		BoneVector3(0,0,0,6),		BoneVector3(0,0,0, 4), BoneVector3(0,0,0, 2) };
                    lowerright = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,106),		BoneVector3(0,0,0, 125), BoneVector3(0,0,0, 123) };
                    lowerleft = { BoneVector3(0,0,0,2),		BoneVector3(0,0,0,93),		BoneVector3(0,0,0, 124), BoneVector3(0,0,0, 122) };



                }

                
               



                targetPawns[i].skeleton = std::vector<std::vector<BoneVector3>>{ upper, right, left, spineBones, lowerright, lowerleft };

            }
            targetPawns[i].skeleton = refreshSkeleton(targetPawns[i]);


        }



        pieceUpdateTimer = tempTime;
        return true;
    }
    else {


        float distBuffer = 999999999.f;
        int currBest = 0;
        for (int i = 0; i < targetPawns.size(); i++) {

            targetPawns[i].RootComponent.RelativeLocation = util::Read<Vector3>(((util::Read<uintptr_t>((targetPawns[i].baseAddress + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(targetPawns[i].RootComponent.RelativeLocation));

            targetPawns[i].health = util::Read<float>(targetPawns[i].attributeSet + 0x38 + 0xC, sizeof(targetPawns[i].health));

            targetPawns[i].W2S = WorldToScreen(targetPawns[i].RootComponent.RelativeLocation, cameraCache);

            targetPawns[i].skeleton = refreshSkeleton(targetPawns[i]);

            if (targetPawns[i].baseAddress == localPlayerPiece) {

                localPiece = targetPawns[i];
                
            }


            targetPawns[i].Distance = getDistance(cameraCache.POV.Location, targetPawns[i].RootComponent.RelativeLocation);
            targetPawns[i].closest = false;

           if (aimPriority == 0) {
                float tempDist = get2DDistance(targetPawns[i].W2S, Vector3(winProperties.width / 2 + winProperties.x, winProperties.height / 2 + winProperties.y, 0));

                if (tempDist < distBuffer) {

                    distBuffer = tempDist;
                    currBest = i;

                }

            }
            else if (aimPriority == 1) {

               if (targetPawns[i].baseAddress != localPiece.baseAddress) {
                   float tempDist = targetPawns[i].Distance;
                   if (tempDist < distBuffer) {
                       distBuffer = tempDist;
                       currBest = i;

                   }
               }


            }
            

        }
        if (currBest) {
            targetPawns[currBest].closest = true;
            currAimPiecIndex = currBest;

        }
        
        
        
       
    }
    return false;

}


void updateItems() {

    uintptr_t AActors = util::Read<uintptr_t>(persistentLevel + 0x98, sizeof(AActors));
    INT32 AActorsSize = util::Read<INT32>(persistentLevel + 0xA0, sizeof(AActorsSize)); \


    std::vector<AActor> tempActors;

    clock_t tempTime = clock();

    if (tempTime >= itemUpdateTimer + 5000) {


        SecureZeroMemory(&itemActors, sizeof(itemActors));
        std::vector<AActor> tempActors;




        for (int i = 0; i < AActorsSize; i++) {


            bool isOnList = false;

            uintptr_t baseAddress = util::Read<uintptr_t>(AActors + 0x8 * i, sizeof(baseAddress));



            INT64 FNameID = util::Read<INT64>((baseAddress + 0x18), sizeof(FNameID));

            std::string Name = GetNameFromFName(FNameID);


            if (!strcmp("BP_BoneShardDispenser_C", Name.c_str())) {

                isOnList = true;
                Name = "Bone Scrap";

            }
            else if (!strcmp("BP_CookLock_C", Name.c_str())) {

                isOnList = true;
                Name = "Cook Lock";

            }
            else if (!strcmp("BP_BloodBucket_Dressed_C", Name.c_str())) {

                isOnList = true;
                Name = "Blood Bucket";

            }
            else if (!strcmp("BP_LargeHealthPickup_Dressed_C", Name.c_str())) {

                isOnList = true;
                Name = "Large Health";

            }
            else if (!strcmp("BP_SmallHealthPickup_Dressed_C", Name.c_str())) {

                isOnList = true;
                Name = "Small Health";

            }
            else if (!strcmp("BP_ToolboxDressedOnly_C", Name.c_str()) || !strcmp("BP_ToolBoxDispenserDressedSml_Light_C", Name.c_str())) {

                isOnList = true;
                Name = "Unlock Tool";

            }
            else if (!strcmp("BP_NoiseMaker_BoneChimeDuo_C", Name.c_str())) {

                isOnList = true;
                Name = "Bone Chime";

            }
            else if (!strcmp("BP_NoiseMaker_Chicken_C", Name.c_str())) {

                isOnList = true;
                Name = "Chicken";

            }
            else if (!strcmp("BP_FusePickup_Dressed_C", Name.c_str())) {

                isOnList = true;
                Name = "Fuse";

            }
            else if (!strcmp("BP_BBQGrandpa_C", Name.c_str())) {

                isOnList = true;
                Name = "Grandpa";

            }
            else if (!strcmp("BP_WaterValvePickup_C", Name.c_str())) {

                isOnList = true;
                Name = "Valve";

            }


            if (isOnList) {

                AActor temp;
                temp.baseAddress = baseAddress;
                temp.Name = Name;
                Vector3 RelativeLocation = util::Read<Vector3>(((util::Read<uintptr_t>((baseAddress + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(RelativeLocation));
                Vector3 W2S = WorldToScreen(RelativeLocation, cameraCache);

                temp.RootComponent.RelativeLocation = RelativeLocation;
                temp.W2S = W2S; 
                temp.Distance = getDistance(cameraCache.POV.Location, temp.RootComponent.RelativeLocation)/100.f;

                tempActors.push_back(temp);




            }

        }
        itemActors = tempActors;
        itemUpdateTimer = tempTime;
    }
    else {

        for (int i = 0; i < itemActors.size(); i++) {

            itemActors[i].RootComponent.RelativeLocation = util::Read<Vector3>(((util::Read<uintptr_t>((itemActors[i].baseAddress + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(itemActors[i].RootComponent.RelativeLocation));
            itemActors[i].W2S = WorldToScreen(itemActors[i].RootComponent.RelativeLocation, cameraCache);
            itemActors[i].Distance = getDistance(cameraCache.POV.Location, itemActors[i].RootComponent.RelativeLocation)/100.f;
        }

    }


}

void printAllActorsInLevel() {


    if (UWorld && persistentLevel) {

      

            uintptr_t AActors = util::Read<uintptr_t>(persistentLevel + 0x98, sizeof(AActors));

            INT32 ActorCount = util::Read<INT32>(persistentLevel + 0x98 + 0x8, sizeof(ActorCount));


            for (int i = 0; i < ActorCount; i++) {


                uintptr_t ActorBase = util::Read<uintptr_t>(AActors + i * 0x8, sizeof(ActorBase));

                int FNameID = util::Read<int>(ActorBase + 0x18, sizeof(FNameID));

                std::string name = GetNameFromFName(FNameID);

                Vector3 worldLocation = util::Read<Vector3>(((util::Read<uintptr_t>((ActorBase + offsets::AActor::RootComponent), sizeof(uintptr_t))) + offsets::USceneComponent::RelativeLocation), sizeof(worldLocation));

                Vector3 W2S = WorldToScreen(worldLocation, cameraCache);

                overlay::drawTextSmall(W2S, 0.f, name.c_str());




            }
    }
}


FTransform GetBoneIndex(AActor player, int index)
{
    return util::Read<FTransform>(player.boneArrayAddr + (index * 0x30), sizeof(FTransform));
}

BoneVector3 GetBoneWithRotation(AActor player, BoneVector3 toRefresh)
{
    FTransform bone = GetBoneIndex(player, toRefresh.boneID);

    if (!bone.scale.x) {

        return toRefresh;
    }




    FTransform ComponentToWorld = util::Read<FTransform>(player.meshAddress + 0x1c0, sizeof(FTransform));

    D3DMATRIX Matrix;
    Matrix = util::MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
    return BoneVector3(Matrix._41, Matrix._42, Matrix._43, toRefresh.boneID);
}

std::vector<std::vector<BoneVector3>> refreshSkeleton(AActor player) {



    for (int i = 0; i < player.skeleton.size(); i++) {


        for (int j = 0; j < player.skeleton[i].size(); j++) {

            player.skeleton[i][j] = GetBoneWithRotation(player, player.skeleton[i][j]);
        }

    }


    return player.skeleton;

}

void drawSkeleton(AActor player) {


    BoneVector3 previous(0, 0, 0, 0);
    BoneVector3 current(0, 0, 0, 0);
    Vector3 p1, c1;
    for (int boneSet = 0; boneSet < player.skeleton.size(); boneSet++)
    {
        previous = BoneVector3(0, 0, 0, 0);
        for (int bone = 0; bone < player.skeleton[boneSet].size(); bone++)
        {
            current = player.skeleton[boneSet][bone];
            if (previous.x == 0.f)
            {
                previous = current;
                continue;
            }
            p1 = WorldToScreen(Vector3(previous.x, previous.y, previous.z), cameraCache);
            c1 = WorldToScreen(Vector3(current.x, current.y, current.z), cameraCache);
            overlay::drawLine(Vector3(p1.x, p1.y, 0), Vector3(c1.x, c1.y, 0));
            previous = current;
        }
    }


}


void drawBoneIDs(AActor player) {

    INT32 boneArrSize = util::Read<INT32>(player.meshAddress + 0x4c0 + 0x8, sizeof(boneArrSize));
    for (int i = 0; i < boneArrSize; i++) {


            FTransform bone = GetBoneIndex(player, i);

            FTransform ComponentToWorld = util::Read<FTransform>(player.meshAddress + 0x1c0, sizeof(FTransform));

            D3DMATRIX Matrix;
            Matrix = util::MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
            Vector3 boneWorld = Vector3(Matrix._41, Matrix._42, Matrix._43);

            overlay::drawText(WorldToScreen(boneWorld, cameraCache), 0.f, std::to_string(i).c_str());
        
    }


}

    void mainLoop(){

        

        util::updateWindow();
       

       

        if (updateWorld(false)) {


        }
        


        
            if (UWorld) {




                updateCameraCache();
                
                if (!GameState) {

                    updateWorld(true);

                }
                    if (refreshPawns()) {
                        updateLocalPlayer();

                    }
                    updateItems();

              //  setPriorityTarget();
                    if (itemESP) {
                        if (itemActors.size()) {


                            for (int i = 0; i < itemActors.size(); i++) {

                                if (itemActors[i].Distance <= itemDistance) {

                                    if (!strcmp("Bone Scrap", itemActors[i].Name.c_str())) {

                                        if (BoneScrap) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }


                                    }
                                    else if (!strcmp("Cook Lock", itemActors[i].Name.c_str())) {

                                        if (CookLock) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance);


                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }


                                    }
                                    else if (!strcmp("Blood Bucket", itemActors[i].Name.c_str())) {

                                        if (BloodBucket) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance);

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Large Health", itemActors[i].Name.c_str())) {

                                        if (LargeHealth) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance);

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Small Health", itemActors[i].Name.c_str())) {

                                        if (SmallHealth) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Unlock Tool", itemActors[i].Name.c_str())) {

                                        if (UnlockTool) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Bone Chime", itemActors[i].Name.c_str())) {

                                        if (BoneChime) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Chicken", itemActors[i].Name.c_str())) {

                                        if (Chicken) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Fuse", itemActors[i].Name.c_str())) {

                                        if (Fuse) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());
                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Grandpa", itemActors[i].Name.c_str())) {

                                        if (Grandpa) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance );

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                    else if (!strcmp("Valve", itemActors[i].Name.c_str())) {

                                        if (Valve) {

                                            overlay::drawTextSmall(itemActors[i].W2S, 0.f, itemActors[i].Name.c_str());

                                            char buf[50];

                                            sprintf_s(buf, "%i Meters\n", (int)itemActors[i].Distance);

                                            overlay::drawTextSmall(itemActors[i].W2S, 15.f, buf);
                                        }

                                    }
                                }


                            }
                        }
                    }

                    if (drawAllItems) {

                        printAllActorsInLevel();
                    }

           
               for (int i = 0; i < targetPawns.size(); i++) {

              

                   
                   if (targetPawns[i].baseAddress != localPiece.baseAddress) {




                       boxDimensions = { (60 / (targetPawns[i].Distance / 1000.f)) * (winProperties.width / 1920) , (110 / (targetPawns[i].Distance / 1000.f)) * (winProperties.height / 1080) };


                      
                      
                       if (boxESP) {

                           ImColor color = ImColor(1.f, 0.f, 0.f, 1.f);

                           if (targetPawns[i].teamID != localPiece.teamID) {

                                color = ImColor(1.f, 0.f, 0.f, 1.f);
                           }
                           else {

                                color = ImColor(0.f, 1.f, 0.f, 1.f);

                           }

                           if (bRounded) {

                               overlay::drawBox(targetPawns[i].W2S, rounding, color);
                           }
                           else {
                               overlay::drawBox(targetPawns[i].W2S, color);

                           }

                       }

                       if (SkeletonESP) {

                           drawSkeleton(targetPawns[i]);
                           drawSkeleton(localPiece);
                           overlay::drawText(targetPawns[i].W2S, 0.f, targetPawns[i].Name.c_str());
                           overlay::drawText(localPiece.W2S, 0.f, localPiece.Name.c_str());


                       }

                       if (lineESP) {

                           overlay::drawLine(targetPawns[i].W2S);
                           drawBoneIDs(targetPawns[i]);
                           drawBoneIDs(localPiece);



                       }

                       if (quishaESP) {

                           ImGui::GetBackgroundDrawList()->AddImage(quishaImg, { targetPawns[i].W2S.x - boxDimensions.x / 2, targetPawns[i].W2S.y - boxDimensions.y / 2 }, { targetPawns[i].W2S.x + boxDimensions.x / 2, targetPawns[i].W2S.y + boxDimensions.y / 2 });


                       }

                       if (distanceESP){
                           char buf[50];
                           SecureZeroMemory(buf, sizeof(buf));
                           sprintf_s(buf, "%i Meters\n", (int)targetPawns[i].Distance / 100);

                           overlay::drawText(targetPawns[i].W2S, boxDimensions.y + 15.f, buf);

                       }
                    
                       if (playerNameESP || teamESP) {
                           
                           std::string buffer;
                           if (teamESP && playerNameESP) {

                               if (targetPawns[i].teamID != localPiece.teamID) {


                                   buffer = targetPawns[i].playerName + "- Enemy";
                               }
                               else {

                                   buffer = targetPawns[i].playerName + "- Friendly";

                               }

                           }
                           else if (teamESP && !playerNameESP) {

                             

                                   if (targetPawns[i].teamID != localPiece.teamID) {


                                       buffer = "Enemy";
                                   }
                                   else {

                                       buffer = "Friendly";

                                   }

                           }
                           else if (playerNameESP && !teamESP) {

                               buffer = targetPawns[i].playerName;
                           }
                           

                           overlay::drawText(targetPawns[i].W2S, -boxDimensions.y-10.f, buffer.c_str());

                          

                       }

                       if (healthESP) {

                           overlay::drawHealth(targetPawns[i].W2S, targetPawns[i].health);

                      }

                       if (infiniteStamina) {

                           util::Write<float>(localPiece.attributeSet + 0x114, 0.f);

                       }

                   }
               }


            }
        }
       

    
       
        


        
    






        



