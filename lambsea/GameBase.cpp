#pragma once
#include "GameBase.h"
#include <list>
#include <vector>
#include <set>
#include "XorStrings.h"
#include "Offsets.h"


#ifdef D3D9RENDER
DWORD Color(int a, int r, int g, int b) {
	return D3DCOLOR_ARGB(a, r, g, b);
}
#endif // D3D9RENDER

namespace GameBase {

	//offset
	DWORD_PTR WorldOffset = 0;
	DWORD_PTR GNameOffset = 0;
	DWORD_PTR FinishCurrentFrameOffset = 0;

	//Required Global
	DWORD_PTR pUWorld = 0;
	DWORD_PTR pLocalPlayer = 0;
	FCameraCacheEntry Cameracache = { NULL };

	//Screen
	int screen_width = 0;
	int screen_height = 0;

	D3D9Render* render = NULL;
	//D3D11Renderer* render = NULL;
	Memory* mem = NULL;

	//Switch
	bool bPlayer = true;
	bool bVehicle = true;
	bool bLoot = true;
	bool bBot = false;
	bool bWeaponRemoval = true;
	bool bMenu = true;

	bool aimHead = true;

	int aimPlayerId = 0;
	bool streamerMode = false;
	float recoilReduceCoe = 0.45f;

}


int Actor_ID_Array[4];
int uaz_ID_Array[3];
int dacia_ID_Array[4];
int motorbike_ID_Array[7];
int buggy_ID_Array[6];
int boat_ID[2];
int itemType_ID_Array[2];
int lobbyHUD_ID = 0;
int ParachutePlayer_ID = 0;
int DeathPack_ID = 0;//DeathDropItemPackage_C
int AirDrop_ID = 0;//Carapackage_RedBox_C
int Van_ID_Array[3];
int PkTruck_ID_Array[10];

list<int> upper_part = { Bones::neck_01, Bones::Head, Bones::forehead };
list<int> right_arm = { Bones::neck_01, Bones::upperarm_r, Bones::lowerarm_r, Bones::hand_r };
list<int> left_arm = { Bones::neck_01, Bones::upperarm_l, Bones::lowerarm_l, Bones::hand_l };
list<int> spine = { Bones::neck_01, Bones::spine_01, Bones::spine_02, Bones::pelvis };
list<int> lower_right = { Bones::pelvis, Bones::thigh_r, Bones::calf_r, Bones::foot_r };
list<int> lower_left = { Bones::pelvis, Bones::thigh_l, Bones::calf_l, Bones::foot_l };
list<list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };

string GetNameStringFromId(int ID) {

	DWORD_PTR fNamePtr = GameBase::mem->read<DWORD_PTR>(GameBase::mem->read<DWORD_PTR>(GameBase::mem->GetProcBase() + GameBase::GNameOffset) + int(ID / 0x4000) * 8);
	DWORD_PTR fName = GameBase::mem->read<DWORD_PTR>(fNamePtr + 8 * int(ID % 0x4000));
	char name[64] = { NULL };

	if (GameBase::mem->readSTR(fName + 16, name, sizeof(name) - 2) != FALSE)
		return std::string(name);

	return std::string("NULL");

}

BOOL GameBase::SetupOffsets()
{

	XorS(WORLDSIG, "48 8B 1D ? ? ? ? ? ? ? 10 4C 8D 4D ? 4C");
	XorS(GNAMESIG, "48 8B 3D ? ? ? ? 48 85 FF 75 38");

	auto GWorldAddress = GameBase::mem->FindPattern(XorString(WORLDSIG));//48 8B 1D ? ? ? ? 74 40
	auto GWorldOffset = GameBase::mem->read<uint32_t>(GWorldAddress + 3) + 7;
	auto ppUWorld = (DWORD_PTR)((DWORD_PTR)GWorldAddress + GWorldOffset);
	auto UworldOffset = (DWORD_PTR)(ppUWorld - GameBase::mem->GetProcBase());

	auto GNameAddress = GameBase::mem->FindPattern(XorString(GNAMESIG));
	auto GNamesOffset = GameBase::mem->read<uint32_t>(GNameAddress + 3);
	GNameAddress += GNamesOffset + 7;
	auto GnameOffset = (DWORD_PTR)(GNameAddress - GameBase::mem->GetProcBase());


	if (GWorldAddress != FALSE && GNameAddress != FALSE) {
		wprintf_s(L"Found UWorld at 0x%I64X\n", UworldOffset);
		wprintf_s(L"Found GName  at 0x%I64X\n", GnameOffset);
	}
	else
	{
		wprintf_s(L"Found Offsets Error. Retrying.\n");
		return FALSE;
	}

	GameBase::WorldOffset = UworldOffset;
	GameBase::GNameOffset = GnameOffset;

	return TRUE;
}


void GameBase::DumpGNAME()
{
	XorS(PFA, "PlayerFemale_A");
	XorS(PFAC, "PlayerFemale_A_C");
	XorS(PMA, "PlayerMale_A");
	XorS(PMAC, "PlayerMale_A_C");
	XorS(UAZA, "Uaz_A_01_C");
	XorS(UAZB, "Uaz_B_01_C");
	XorS(UAZC, "Uaz_C_01_C");

	string name = "";
	int i = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, cBoat = 0, cVan = 0, cPickUpTruck = 0;
	while (true)
	{
		if (c1 == 4 && c2 == 3 && c3 == 4 && c4 == 7 && c5 == 6 && cBoat == 2 && c6 == 2 && lobbyHUD_ID != 0 && ParachutePlayer_ID != 0 && AirDrop_ID != 0 && DeathPack_ID != 0 && cVan == 3 && cPickUpTruck == 10)
		{
			//cout << "IDs retrieved" << endl;
			wprintf_s(L"Get ID Done.\n");
			return;
		}
		name = GetNameStringFromId(i);
		//if (name == "PlayerFemale_A" || name == "PlayerFemale_A_C" || name == "PlayerMale_A" || name == "PlayerMale_A_C")
		if (name == XorString(PFA) || name == XorString(PFAC) || name == XorString(PMA) || name == XorString(PMAC))
		{
			//cout << name.c_str() << " " << i << endl;
			Actor_ID_Array[c1++] = i;
		}
		if (name == XorString(UAZA) || name == XorString(UAZB) || name == XorString(UAZC))
		{
			uaz_ID_Array[c2++] = i;
			//cout << name.c_str() << " " << i << endl;
		}
		if (name == "Dacia_A_01_v2_C" || name == "Dacia_A_02_v2_C" || name == "Dacia_A_03_v2_C" || name == "Dacia_A_04_v2_C")
		{
			dacia_ID_Array[c3++] = i;
			//cout << name.c_str() << " " << i << endl;
		}
		if (name == "ABP_Motorbike_03_C" || name == "ABP_Motorbike_04_C" || name == "BP_Motorbike_03_C" || name == "BP_Motorbike_04_C" || name == "BP_Motorbike_04_SideCar_C" || name == "BP_Motorbike_04_SideCar_Desert_C" || name == "BP_Motorbike_04_Desert_C")
		{
			motorbike_ID_Array[c4++] = i;
			//cout << name.c_str() << " " << i << endl;
		}
		if (name == "Buggy_A_01_C" || name == "Buggy_A_02_C" || name == "Buggy_A_03_C" || name == "Buggy_A_04_C" || name == "Buggy_A_05_C" || name == "Buggy_A_06_C")
		{
			buggy_ID_Array[c5++] = i;
			//cout << name.c_str() << " " << i << endl;
		}
		if (name == "Boat_PG117_C" || name == "AquaRail_A_01_C")
		{
			boat_ID[cBoat++] = i;
			//cout << name.c_str() << " " << i << endl;
		}
		if (name == "DroppedItemInteractionComponent" || name == "DroppedItemGroup")
		{
			//cout << name.c_str() << " " << i << endl;
			itemType_ID_Array[c6++] = i;
		}
		if (name == "LobbyHUD_Default_C")
		{
			//cout << name.c_str() << " " << i << endl;
			lobbyHUD_ID = i;
		}
		if (name == "ParachutePlayer_C")
		{
			//cout << name.c_str() << " " << i << endl;
			ParachutePlayer_ID = i;
		}

		if (name == "DeathDropItemPackage_C")
		{
			//cout << name.c_str() << " " << i << endl;
			DeathPack_ID = i;
		}

		if (name == "Carapackage_RedBox_C")
		{
			//cout << name.c_str() << " " << i << endl;
			AirDrop_ID = i;
		}

		if (name == "BP_Van_A_01_C" || name == "BP_Van_A_02_C" || name == "BP_Van_A_03_C")
		{
			Van_ID_Array[cVan++] = i;
		}

		if (name == "BP_PickupTruck_A_01_C" || name == "BP_PickupTruck_A_02_C" || name == "BP_PickupTruck_A_03_C"
			|| name == "BP_PickupTruck_A_04_C" || name == "BP_PickupTruck_A_05_C"
			|| name == "BP_PickupTruck_B_01_C" || name == "BP_PickupTruck_B_02_C" || name == "BP_PickupTruck_B_03_C"
			|| name == "BP_PickupTruck_B_04_C" || name == "BP_PickupTruck_B_05_C")
		{
			PkTruck_ID_Array[cPickUpTruck++] = i;
		}

		i++;

		if (i > 400000) {
			wprintf(L"Cache Name Partialy Done..Retrying\n");
			i = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, cBoat = 0;
			Sleep(1000);

			delete GameBase::mem;
			DriverInterface::LoadDriver();
			GameBase::mem = new Memory();
			continue;
		}
	}
}

inline void GameBase::UpdateActualWorld()
{
	if (!mem)
		return;

	///* initialize the decryption context, very important! */
	//init_decryption();

	////Get encryption block
	//auto WorldOffsetBlock = GameBase::mem->read<EncryptedBlock>(GameBase::mem->GetProcBase() + 0x37D7818);

	////get encrypted ptr
	//GameBase::pUWorld = decrypt_ptr<DWORD_PTR>(&WorldOffsetBlock);

	//GameBase::pUWorld = mem->read<DWORD_PTR>(mem->GetProcBase() + WorldOffset);

	GameBase::pUWorld = mem->read<DWORD_PTR>(mem->GetProcBase() + WorldOffset);
	GameBase::pUWorld = mem->read<DWORD_PTR>(GameBase::pUWorld);

	auto pGameInstance = mem->read<DWORD_PTR>(GameBase::pUWorld + GameInstanceOffset);
	auto pLocalPlayerArray = mem->read<DWORD_PTR>(pGameInstance + LocalPlayerArrayOffset);
	GameBase::pLocalPlayer = mem->read<DWORD_PTR>(pLocalPlayerArray + 0x0);
	auto pViewportClient = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + ViewPointOffset);
	GameBase::pUWorld = mem->read<DWORD_PTR>(pViewportClient + 0x80); // Actual World

}

inline void GameBase::UpdateAddresses()
{
	//if(!GameBase::pLocalPlayer)
	//	UpdateActualWorld();

	UpdateActualWorld();

	auto pGameInstance = mem->read<DWORD_PTR>(GameBase::pUWorld + GameInstanceOffset);
	auto pLocalPlayerArray = mem->read<DWORD_PTR>(pGameInstance + LocalPlayerArrayOffset);
	GameBase::pLocalPlayer = mem->read<DWORD_PTR>(pLocalPlayerArray + 0x0);
	GameBase::Cameracache = UpdateCameraCache();

}

inline FCameraCacheEntry GameBase::UpdateCameraCache()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + PlayerControllerOffset);
	return mem->read<FCameraCacheEntry>(mem->read<DWORD_PTR>(playercontroller + PlayerCameraManagerOffset) + CameraCacheOffset);
}

inline D3DXMATRIX Matrix(Vector3 &rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

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

inline D3DMATRIX MatrixMultiplication(D3DMATRIX &pM1, D3DMATRIX &pM2)
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

Vector3 GameBase::WorldToScreen(Vector3 &WorldLocation, FCameraCacheEntry &CameraCacheL)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCacheL.POV;
	Vector3 Rotation = POV.Rotation; // FRotator

	D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

	//Vector3 vAxisX, vAxisY, vAxisZ;

	auto vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	auto vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	auto vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = GameBase::screen_width / 2.0f;
	float ScreenCenterY = GameBase::screen_height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

FTransform GameBase::GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = mem->read<DWORD_PTR>(mesh + 0x790);
	return mem->read<FTransform>(bonearray + (index * 0x30));
}

Vector3 GameBase::GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = mem->read<FTransform>(mesh + 0x190);
	D3DMATRIX Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

//Pointer To EntityList
inline DWORD_PTR GameBase::GetEntityList()
{
	return mem->read<DWORD_PTR>(mem->read<DWORD_PTR>(GameBase::pUWorld + ULevelOffset) + AActorArrayOffset);
}

//Entity Count, return an int;
inline int GameBase::GetEntityCount()
{
	return mem->read<int>(mem->read<DWORD_PTR>(GameBase::pUWorld + ULevelOffset) + AActorArrayLengthOffset);
}

inline int GameBase::GetEntityID(DWORD_PTR entity)
{
	return mem->read<int>(entity + AActorIDoffset);
}

inline Vector3 GameBase::GetLocalPlayerPos()
{
	return Cameracache.POV.Location;
}

inline int GameBase::GetLocalPlayerTeamId()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + PlayerControllerOffset);
	auto pawn = mem->read<DWORD_PTR>(playercontroller + AcknowledgedPawnOffset);
	auto playerState = mem->read<DWORD_PTR>(pawn + PlayerStateOffset);
	return mem->read<int>(playerState + TeamIDoffset);
}

inline int GameBase::GetLocalPlayerHUDId()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + PlayerControllerOffset);
	auto hudClass = mem->read<DWORD_PTR>(playercontroller + AHUDOffset);
	return GetEntityID(hudClass);
}

inline int GameBase::GetLocalPlayerPawnId()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + PlayerControllerOffset);
	auto pawn = mem->read<DWORD_PTR>(playercontroller + AcknowledgedPawnOffset);
	return GetEntityID(pawn);
}

inline Vector3 GameBase::GetActorPos(DWORD_PTR entity)
{
	auto rootcomp = mem->read<DWORD_PTR>(entity + RootCompOffset);
	return mem->read<Vector3>(rootcomp + LocationOffset);
}

wstring GameBase::GetPlayerName(DWORD_PTR entity)
{
	
	auto pawn = mem->read<DWORD_PTR>(entity + APawnOffset);
	auto playerState = mem->read<DWORD_PTR>(pawn + PlayerStateOffset);
	auto FStringPtr = mem->read<DWORD_PTR>(playerState + PlayerNameOffset);
	//auto FStringLength = mem->read<int32_t>(playerState + PlayerNameOffset + 8);

	wchar_t playername[64] = { NULL };
	auto ret = mem->readWSTR(FStringPtr, playername, sizeof(playername));

	if (ret)
		return wstring(playername);
	else
		return wstring();

}

inline int GameBase::GetActorTeamId(DWORD_PTR pAActor)
{
	auto pawn = mem->read<DWORD_PTR>(pAActor + APawnOffset);
	auto playerState = mem->read<DWORD_PTR>(pawn + PlayerStateOffset);
	return mem->read<int>(playerState + TeamIDoffset);
}

inline float GameBase::GetActorHealth(DWORD_PTR pAActor)
{
	return mem->read<float>(pAActor + HealthOffset);
}

//PUBG TSL CLASSES AND ENGINE CLASSES
inline int GameBase::GetPlayerID(DWORD_PTR pAActor)
{
	DWORD_PTR pawn = mem->read<DWORD_PTR>(pAActor + APawnOffset);
	DWORD_PTR playerState = mem->read<DWORD_PTR>(pawn + PlayerStateOffset);
	return mem->read<int>(playerState + PlayerIDOffset);
}

inline DWORD_PTR GameBase::GetMesh(DWORD_PTR pAActor)
{
	return mem->read<DWORD_PTR>(pAActor + MeshOffset);
}

bool GameBase::isVisible(DWORD_PTR mesh)
{
	bool isRecentlyRendered = false;
	auto byte = mem->read<unsigned char>(mesh+ 0x0A40);

	return GetBitFieldBool(byte, 2); //third

	//if ((byte & 0x4) > 0)
	//	isRecentlyRendered = true;

	//return isRecentlyRendered;
}

wstring GameBase::GetVehicleInfo(int id)
{

	for (int i = 0; i < 3; i++)
		if (id == uaz_ID_Array[i])
			return L"越野车\n距离: %0.2f";

	for (int i = 0; i < 4; i++)
		if (id == dacia_ID_Array[i])
			return L"小轿车\n距离: %0.2f";

	for (int i = 0; i < 6; i++)
		if (id == buggy_ID_Array[i])
			return L"蹦蹦车\n距离: %0.2f";

	for (int i = 0; i < 7; i++)
		if (id == motorbike_ID_Array[i])
			return L"摩托车\n距离: %0.2f";

	for (int i = 0; i < 3; i++)
		if (id == Van_ID_Array[i])
			return L"校车\n距离: %0.2f";

	for (int i = 0; i < 10; i++)
		if (id == PkTruck_ID_Array[i])
			return L"沙漠皮卡\n距离: %0.2f";

	for (int i = 0; i < 2; i++)
		if (id == boat_ID[i])
			return L"一艘船\n距离: %0.2f";

	return wstring(L"");

}

inline DWORD_PTR GameBase::GetDroppedItemGroupArray(DWORD_PTR entity)
{
	return mem->read<DWORD_PTR>(entity + DroppedItemGroupArrayOffset);
}

inline int GameBase::GetDroppedItemGroupArrayCount(DWORD_PTR entity)
{
	return mem->read<int>(entity + DroppedItemGroupArrayCountOffset);
}

inline bool GameBase::GetBitFieldBool(unsigned char bitFiled, int position)
{
	auto a = (bitFiled >> position) & 1;

	if (a == 0)
		return false;
	else
		return true; 
}

unsigned char GameBase::ToggleBitFiled(unsigned char bitFiled, int position)
{
	bitFiled ^= 1 << position;
	return bitFiled;
}

inline bool GameBase::GetSpectatedStatus()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + PlayerControllerOffset);
	auto cIsSpectated = mem->read<unsigned char>(playercontroller + 0x0858);
	return GetBitFieldBool(cIsSpectated, 1); //second
}

void GameBase::DisableHackerReport()
{
	auto playercontroller = mem->read<DWORD_PTR>(GameBase::pLocalPlayer + 0x30);
	auto pawn = mem->read<DWORD_PTR>(playercontroller + 0x418);
	auto pHackerReporter = (pawn + 0x0BC0);
	mem->write<DWORD_PTR>(pHackerReporter, DWORD_PTR(0));
}


inline BoneArrayStruct GameBase::GetBoneArray(DWORD_PTR mesh) {
	DWORD_PTR bonearray = mem->read<DWORD_PTR>(mesh + BoneArrayOffset);
	return  mem->read<BoneArrayStruct>(bonearray);//0x30
}

inline Vector3 GameBase::GetBoneWithRotationEx(BoneArrayStruct &bas, int id, FTransform &ComponentToWorld) {

	FTransform bone = bas.BoneArray[id];
	D3DMATRIX Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

inline void GameBase::DrawSkeleton(BoneArrayStruct &BAS, FTransform &ComponentToWorld, bool isVisible)
{
	Vector3 neckpos = GetBoneWithRotationEx(BAS, Bones::neck_01, ComponentToWorld);
	Vector3 pelvispos = GetBoneWithRotationEx(BAS, Bones::pelvis, ComponentToWorld);
	Vector3 previous(0, 0, 0);
	Vector3 current, p1, c1;
	for (auto a : skeleton) // a is one of 5 parts in skeleton
	{
		previous = Vector3(0, 0, 0);
		for (int bone : a) //bone is one of bones in particular part
		{
			current = bone == Bones::neck_01 ? neckpos : (bone == Bones::pelvis ? pelvispos : GetBoneWithRotationEx(BAS, bone, ComponentToWorld));
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}
			p1 = WorldToScreen(previous, Cameracache);
			c1 = WorldToScreen(current, Cameracache);

			if(isVisible)
				render->DrawLine(p1.x, p1.y, c1.x, c1.y, Color(255, 153, 220, 9));
			else
				render->DrawLine(p1.x, p1.y, c1.x, c1.y, Color(255, 96, 8, 220));

			previous = current;
		}
	}
}

inline void GameBase::DrawVehicle(DWORD_PTR entity, Vector3 &local, const wchar_t * info)
{
	Vector3 pos = GetActorPos(entity);
	Vector3 spos = WorldToScreen(pos, Cameracache);
	float distance = local.Distance(pos) / 100.f;
	render->DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 191, 215, 255), render->GetFont(), info, local.Distance(pos) / 100.f);
}

void GameBase::AimAtPosAngles(Vector3 &TargetLocation, int targetUID) {

	if (targetUID != aimPlayerId)
		return;

	FCameraCacheEntry CameraCacheL = Cameracache;
	auto pov = CameraCacheL.POV;

	Vector3 LocalHeadPosition = pov.Location;

	Vector3 vecDelta((LocalHeadPosition.x - TargetLocation.x), (LocalHeadPosition.y - TargetLocation.y), (LocalHeadPosition.z - TargetLocation.z));
	float hyp = (float)sqrt(vecDelta.x * vecDelta.x + vecDelta.y * vecDelta.y);

	Rotator ViewAngles;

	ViewAngles.pitch = -(float)atan(vecDelta.z / hyp) * (float)(180.f / M_PI);
	ViewAngles.yaw = (float)atan(vecDelta.y / vecDelta.x) * (float)(180.f / M_PI);

	ViewAngles.roll = (float)0.f;

	if (vecDelta.x >= 0.f)
		ViewAngles.yaw += 180.0f;


	while (ViewAngles.yaw < -180.0f)
		ViewAngles.yaw += 360.0f;
	while (ViewAngles.yaw > 180.0f)
		ViewAngles.yaw -= 360.0f;

	if (ViewAngles.pitch < -74.f) //The actual value for max pitch seems to be 74.9999 something, but 74 will work fine.
		ViewAngles.pitch = -74.f;
	if (ViewAngles.pitch > 74.f)
		ViewAngles.pitch = 74.f;

	auto  pLocalPlayerController = mem->read<DWORD_PTR>(pLocalPlayer + PlayerControllerOffset);

	mem->write<float>(pLocalPlayerController + ControlRotationOffset, ViewAngles.pitch);
	mem->write<float>(pLocalPlayerController + ControlRotationOffset + 0x4, ViewAngles.yaw);

}

inline bool  GameBase::inAimRange(Vector3 &targetInScreen) {

	auto centerx = screen_width / 2;
	auto centery = screen_height / 2;

	auto xUnit = screen_width / 10;
	auto yUnit = screen_height / 10;

	auto leftXboundry = centerx - xUnit / 2;
	auto rightXboundry = centerx + xUnit / 2;

	auto upperYboundry = centery - yUnit / 2;
	auto bottomYboundry = centery + yUnit / 2;


	bool a = leftXboundry <= targetInScreen.x && targetInScreen.x <= rightXboundry;
	bool b = upperYboundry <= targetInScreen.y && targetInScreen.y <= bottomYboundry;

	return a&&b;

}

inline DWORD_PTR GameBase::GetDecryptedEntity(DWORD_PTR EncryptedEntityPtr, EncryptionTableStruct &EncryptionTableS)
{
	//auto encrypted_actor = mem->read<TSLEncrypted_Actor>(EncryptedEntityPtr);
	//uint16_t decoded_xor = encrypted_actor.xor ^ 0xCBAC;
	//uint16_t decoded_index = _mm_crc32_u32(0x19D5D75Fu, encrypted_actor.index ^ 0x3185721C) % 0x2B;
	//return decoded_xor ^ encrypted_actor.ptr_table[decoded_index];

	auto encrypted_actor = mem->read<TSLEncrypted_Actor>(EncryptedEntityPtr);
	DWORD decoded_xor = encrypted_actor.xor ^ 0xCBAC;
	DWORD decoded_Index = encrypted_actor.index ^ 0xD7AF5ABC;

	//DWORD_PTR EncryptionTable = mem->GetProcBase() + 0x397B5A0;
	//EncryptionTableStruct EncryptionTableS = mem->read<EncryptionTableStruct>(EncryptionTable);

	////*(crctable + (byte)(dindex)+0x300)
	//auto Xor1 = mem->read<DWORD>(EncryptionTable + ((byte)(decoded_Index)+0x300) * sizeof(DWORD));

	////*(crctable + (byte)((ptr)dindex >> 0x8) + 0x200)
	//auto Xor2 = mem->read<DWORD>(EncryptionTable + ((byte)((DWORD_PTR)decoded_Index >> 0x8)  + 0x200) * sizeof(DWORD));

	////*(crctable + (byte)((ptr)dindex >> 0x10) + 0x100)
	//auto Xor3 = mem->read<DWORD>(EncryptionTable + ((byte)((DWORD_PTR)decoded_Index >> 0x10) + 0x100) * sizeof(DWORD));

	////*(crctable + (ptr)(dindex >> 0x18))
	//auto Xor4 = mem->read<DWORD>(EncryptionTable + ((DWORD_PTR)(decoded_Index >> 0x18)) * sizeof(DWORD));

	auto Xor1 = EncryptionTableS.EncryptionKey[((byte)(decoded_Index)+0x300)];
	auto Xor2 = EncryptionTableS.EncryptionKey[(byte)((DWORD_PTR)decoded_Index >> 0x8) + 0x200];
	auto Xor3 = EncryptionTableS.EncryptionKey[((byte)((DWORD_PTR)decoded_Index >> 0x10) + 0x100)];
	auto Xor4 = EncryptionTableS.EncryptionKey[(DWORD_PTR)(decoded_Index >> 0x18)];

	//(*(crctable + (byte)(dindex)+0x300) ^ *(crctable + (byte)((ptr)dindex >> 0x8) + 0x200) ^ *(crctable + (byte)((ptr)dindex >> 0x10) + 0x100) ^ ~*(crctable + (ptr)(dindex >> 0x18))) % 0x2B;
	auto Real_Index = (Xor1^Xor2^Xor3^~Xor4) % 0x2B;
	return encrypted_actor.ptr_table[Real_Index] ^ decoded_xor;

}

inline DWORD_PTR GameBase::GetDecryptedEntityEx(TSLEncrypted_Actor* EntityArray, int index, EncryptionTableStruct &EncryptionTableS)
{
	//auto encrypted_actor = mem->read<TSLEncrypted_Actor>(EncryptedEntityPtr);
	//uint16_t decoded_xor = encrypted_actor.xor ^ 0xCBAC;
	//uint16_t decoded_index = _mm_crc32_u32(0x19D5D75Fu, encrypted_actor.index ^ 0x3185721C) % 0x2B;
	//return decoded_xor ^ encrypted_actor.ptr_table[decoded_index];

	auto encrypted_actor = EntityArray[index];

	DWORD decoded_xor = encrypted_actor.xor ^ 0xCBAC;
	DWORD decoded_Index = encrypted_actor.index ^ 0xD7AF5ABC;

	auto Xor1 = EncryptionTableS.EncryptionKey[((byte)(decoded_Index)+0x300)];
	auto Xor2 = EncryptionTableS.EncryptionKey[(byte)((DWORD_PTR)decoded_Index >> 0x8) + 0x200];
	auto Xor3 = EncryptionTableS.EncryptionKey[((byte)((DWORD_PTR)decoded_Index >> 0x10) + 0x100)];
	auto Xor4 = EncryptionTableS.EncryptionKey[(DWORD_PTR)(decoded_Index >> 0x18)];

	///(*(crctable + (byte)(dindex)+0x300) ^ *(crctable + (byte)((ptr)dindex >> 0x8) + 0x200) ^ *(crctable + (byte)((ptr)dindex >> 0x10) + 0x100) ^ ~*(crctable + (ptr)(dindex >> 0x18))) % 0x2B;
	auto Real_Index = (Xor1^Xor2^Xor3^~Xor4) % 0x2B;
	return encrypted_actor.ptr_table[Real_Index] ^ decoded_xor;

}

std::set <std::wstring> dump_name = { L"Backpack (Lv.3)", L"4x Scope", L"8x Scope", L"AKM", L"Kar98k", L"Backpack (Lv.2)", L"Energy Drink", L"First Aid Kit", L"Helmet (Lv.3)", L"Med Kit", L"Military Vest (Level 3)", L"Painkiller", L"Pan", L"SCAR-L",L"M16A4",L"M416", L"Suppressor (AR, S12K)", L"Suppressor (SR)" };

void GameBase::EntityLoop()
{

	UpdateAddresses();

	auto EntityList = GetEntityList();
	auto EntityCount = GetEntityCount(); 
	auto health = 0.f;
	auto GroggyHealth = 0.f;
	auto distance = 0.f;
	DWORD color = 0;
	auto local = GetLocalPlayerPos();

	//DWORD_PTR EncryptionTable = mem->GetProcBase() + 0x397B5A0;
	//EncryptionTableStruct EncryptionTableS = mem->read<EncryptionTableStruct>(EncryptionTable);

	///* initialize the decryption context, very important! */
	//init_decryption();

	////Get encryption block
	//auto WorldOffsetBlock = GameBase::mem->read<EncryptedBlock>(GameBase::mem->GetProcBase() + 0x4054650);

	if ( EntityCount < 20000  && 0 < EntityCount)
	{
		DWORD_PTR* rawEntityArray = (DWORD_PTR*)HeapAlloc(GetProcessHeap(), 0, sizeof(DWORD_PTR)*EntityCount);
		DriverInterface::DriverReadMemory(EntityList, rawEntityArray, sizeof(DWORD_PTR) * EntityCount);

		//EncryptedBlock* rawEntityArray = (EncryptedBlock*)HeapAlloc(GetProcessHeap(), 0, sizeof(EncryptedBlock)*EntityCount);
		//DriverInterface::DriverReadMemory(EntityList, rawEntityArray, sizeof(EncryptedBlock) * EntityCount);

		for (int i = 0; i < EntityCount; ++i) {

			//auto Entity = GetDecryptedEntityEx(rawEntityArray, i, EncryptionTableS);

			auto Entity = rawEntityArray[i];

			//auto Entity = decrypt_ptr<DWORD_PTR>(&rawEntityArray[i]);

			if (!Entity)
				continue;

			if (Entity == mem->read<DWORD_PTR>(mem->read<DWORD_PTR>(pLocalPlayer + PlayerControllerOffset) + AcknowledgedPawnOffset)) //skip self
				continue;

			auto EntityID = GetEntityID(Entity);

			//player ESP
			if (bPlayer && (EntityID == Actor_ID_Array[0] || EntityID == Actor_ID_Array[1] || EntityID == Actor_ID_Array[2] || EntityID == Actor_ID_Array[3]))
			{

				health = GetActorHealth(Entity);

				if (health > 0.f)
				{
					auto pos = GetActorPos(Entity);
					auto spos = WorldToScreen(pos, Cameracache);
					distance = local.Distance(pos) / 100.f;

					if (distance > 1000.0f)
						continue;

					if (distance <= 150.f)
						color = D3DCOLOR_ARGB(255, 255, 229, 205); //color red, if less than 150m
					else if (distance > 150.f && distance <= 250.f)
						color = D3DCOLOR_ARGB(255, 255, 210, 0); //color yellow, if less than 250m and greater than 150m
					else
						color = D3DCOLOR_ARGB(255, 0, 210, 0); //color green, if greater than 250m

					auto mesh = GetMesh(Entity);

					if (!mesh)
						continue;

					auto varBoneArrayStruct = GetBoneArray(mesh);
					auto ComponentToWorld = mem->read<FTransform>(mesh + 0x280);
					//class USceneComponent : public UActorComponent
					auto IsVisible = isVisible(mesh);

					Vector3 headpos = GetBoneWithRotationEx(varBoneArrayStruct, Bones::Head, ComponentToWorld);
					Vector3 foreheadpos = GetBoneWithRotationEx(varBoneArrayStruct, Bones::forehead, ComponentToWorld);
					Vector3 neckPos = GetBoneWithRotationEx(varBoneArrayStruct, Bones::breast_l, ComponentToWorld);

					auto headpos_s = WorldToScreen(headpos, Cameracache);
					auto foreheadpos_s = WorldToScreen(foreheadpos, Cameracache);
					auto neckPos_s = WorldToScreen(neckPos, Cameracache);

					auto drawposy = spos.y + 8.5 * (headpos_s.y - foreheadpos_s.y);
					float healthbar_mig = headpos_s.y - foreheadpos_s.y;

					if(streamerMode)
						render->DrawString((int)spos.x, (int)drawposy, color, render->GetFont(), L"距离 %0.1f米 %ws", distance,GetPlayerName(Entity).c_str());
					else
						render->DrawString((int)spos.x, (int)drawposy, color, render->GetFont(), L"距离 %0.1f米", distance);

					render->DrawHealthBar(spos.x - (25.0f + 400.0f / distance) / 2, foreheadpos_s.y - healthbar_mig * 2.5f - 1.0f* (distance / 1000), 25.0f + 400.0f / distance, health, 100.f);

					if (bBot)
					{
						render->DrawBox(screen_width / 2 - screen_width / 20, screen_height / 2 - screen_width / 20, screen_width / 10, screen_width / 10, Color(255, 255, 210, 0));
						if (GetLocalPlayerTeamId() != GetActorTeamId(Entity) && inAimRange(headpos_s) && IsVisible)
						{
							if ((GetAsyncKeyState(VK_RBUTTON) & (1 << 16)) && (distance <= 900.f))
							{
								auto selectedAimTarget = GetPlayerID(Entity);

								if (aimPlayerId == 0)
									aimPlayerId = selectedAimTarget;

								if(aimHead == true)
									AimAtPosAngles(headpos, selectedAimTarget);
								else
									AimAtPosAngles(neckPos, selectedAimTarget);

							}
							else
							{
								aimPlayerId = 0;
							}
						}
					}

					//if (!mesh)
					//	continue;

					DrawSkeleton(varBoneArrayStruct, ComponentToWorld, IsVisible); //draw skeleton
				}
			}

			//Vehicle ESP
			if (bVehicle)
			{
				auto info = GetVehicleInfo(EntityID);
				if (info.size() >= 4) {
					DrawVehicle(Entity, local, info.c_str());
				}
			}

			//loot
			if (bLoot && (EntityID == itemType_ID_Array[0] || EntityID == itemType_ID_Array[1]))
			{

				wchar_t entityname[64] = { NULL };
				auto DroppedItemGroupArray = GetDroppedItemGroupArray(Entity);
				auto count = GetDroppedItemGroupArrayCount(Entity);

				if (!count || !DroppedItemGroupArray)
					continue;

				color = D3DCOLOR_ARGB(255, 249, 249, 244);

				for (int j = 0; j < count; j++)
				{
					DWORD_PTR pADroppedItemGroup = mem->read<DWORD_PTR>(DroppedItemGroupArray + j * 0x10);

					if (!pADroppedItemGroup)
						continue;

					Vector3 relative = mem->read<Vector3>(pADroppedItemGroup + DroppedItemGroupRelativeOffset);
					Vector3 spos = WorldToScreen(GetActorPos(Entity) + relative, Cameracache);
					DWORD_PTR pUItem = mem->read<DWORD_PTR>(pADroppedItemGroup + UItemOffset);
					DWORD_PTR pUItemFString = mem->read<DWORD_PTR>(pUItem + UItemFStringOffset);
					DWORD_PTR pItemName = mem->read<DWORD_PTR>(pUItemFString + 0x28);//DANGER
					ZeroMemory(entityname, sizeof(entityname));
					if (mem->readWSTR(pItemName, entityname, sizeof(entityname))) {
						if (dump_name.find(entityname) != dump_name.end())
							render->DrawString((int)spos.x, (int)spos.y, color, render->GetFont(), L"[%ws]", entityname);
					}
				}


			}

			//death pack & airdrop
			if (bLoot && (EntityID == DeathPack_ID || EntityID == AirDrop_ID)) {


				Vector3 pos = GetActorPos(Entity);
				Vector3 spos = WorldToScreen(pos, Cameracache);
				distance = local.Distance(pos) / 100.f;

				if (distance > 1000)
					continue;

				if (EntityID == DeathPack_ID && distance <= 200.f)
				{
					render->DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 214, 211, 89), render->GetFont(), L"死人包: 距离 %0.1f", distance);
				}
				else if (EntityID == AirDrop_ID)
				{
					render->DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 214, 211, 89), render->GetFont(), L"空投: 距离 %0.1f", distance);
				}


			}

		}

		HeapFree(GetProcessHeap(), 0, rawEntityArray);
	}

	if (GameBase::GetSpectatedStatus())
	{
		render->DrawString(screen_width / 2 - 400, 95, D3DCOLOR_ARGB(255, 191, 224, 141), render->GetFont(), L"注意!正在被观战！");
		render->FillRect(screen_width / 2 - 400 - 12, 95 - 8, 150, 15, Color(255, 25, 29, 36));
	}
		

	if (GameBase::bMenu)
		Menu();



}

void GameBase::Menu() {

	auto color = D3DCOLOR_ARGB(255, 191, 224, 141);
	auto pesp = GameBase::bPlayer ? L"On" : L"Off";
	auto vesp = GameBase::bVehicle ? L"On" : L"Off";
	auto lesp = GameBase::bLoot ? L"On" : L"Off";
	auto bot = GameBase::bBot ? L"On" : L"Off";
	auto stmdo = GameBase::streamerMode ? L"On" : L"Off";
	auto sAimHead = GameBase::aimHead ? L"头部" : L"胸部";

	if (GameBase::streamerMode)
	{
		render->FillRect(screen_width / 2 - 200 - 12, 90 - 8, 190, 265, Color(255, 25, 29, 36));
		render->DrawString(screen_width / 2 - 200, 90, color, render->GetFont(), L"重置辅助(F3) \n\n玩家探测(F4) %ws\n物品探测(F5) %ws\n车辆探测(F6) %ws\n自动瞄准(F7) %ws\n主播模式(F10) %ws\n刷新无后座(F8)\n\n后座系数\n(小键盘1/2调整)%.1f\n自瞄位置切换\n(小键盘3): %ws\n开关本菜单(F9)", pesp, lesp, vesp, bot, stmdo, recoilReduceCoe, sAimHead);
	}
	else 
	{
		render->FillRect(screen_width / 2 - 200 - 12, 90 - 8, 185, 215, Color(255, 25, 29, 36));
		render->DrawString(screen_width / 2 - 200, 90, color, render->GetFont(), L"重置辅助(F3) \n\n玩家探测(F4) %ws\n物品探测(F5) %ws\n车辆探测(F6) %ws\n自动瞄准(F7) %ws\n主播模式(F10) %ws\n刷新无后座(F8)\n自瞄位置切换\n(小键盘3): %ws\n开关本菜单(F9)", pesp, lesp, vesp, bot, stmdo, sAimHead);
	}


}

void GameBase::modifyWeapon()
{
	auto playercontroller = mem->read<DWORD_PTR>(pLocalPlayer + PlayerControllerOffset);
	auto pawn = mem->read<DWORD_PTR>(playercontroller + AcknowledgedPawnOffset);
	auto weaponProcessor = mem->read<DWORD_PTR>(pawn + WeaponProcessorOffset); //weapon processor

	auto EquippedWeappons = mem->read<DWORD_PTR>(weaponProcessor + EquippedWeaponArrayOffset); //TAAray EquippedWeappons
	auto CurrentWeaponIndex = mem->read<int>(weaponProcessor + CurrentWeaponIndexOffset);

	auto weapon = mem->read<DWORD_PTR>(EquippedWeappons + 0x0); //current weapon - ATslweapon class
	auto weapinID = mem->read<int>(weapon + AActorIDoffset);

	for (int i = 0; i < 3; i++)
	{

		weapon = mem->read<DWORD_PTR>(EquippedWeappons + 0x8 * i);

		FWeaponData WeaponConfig;
		WeaponConfig = mem->read<FWeaponData>(weapon + FWeaponDataOffset);
		auto pWeaponConfig = weapon + FWeaponDataOffset;

		FRecoilInfo RecoilInfo;
		RecoilInfo = mem->read<FRecoilInfo>(weapon + FRecoilInfoOffset);
		auto pRecoilInfo = weapon + FRecoilInfoOffset;

		FTrajectoryWeaponData TrajectoryConfig;
		TrajectoryConfig = mem->read<FTrajectoryWeaponData>(weapon + FTrajectoryWeaponDataOffset);
		auto pTWDate = weapon + FTrajectoryWeaponDataOffset;

		if (TrajectoryConfig.HitDamage == 0 || RecoilInfo.VerticalRecoilMax == 0 || WeaponConfig.SwayModifier_Pitch == 0)
			continue;

		FWeaponDeviationData WeaponDeviationConfig;
		WeaponDeviationConfig = mem->read<FWeaponDeviationData>(weapon + FWeaponDeviationDataOffset);
		auto pWeaponDeviationConfig = weapon + FWeaponDeviationDataOffset;

		FWeaponGunData WeaponGunConfig;
		WeaponGunConfig = mem->read<FWeaponGunData>(weapon + FWeaponGunDataOffset);
		auto pWeaponGunConfig = weapon + FWeaponGunDataOffset;

		FWeaponGunAnim WeaponGunAnim;
		WeaponGunAnim = mem->read<FWeaponGunAnim>(weapon + FWeaponGunAnimOffset);
		auto pWeaponGunAnim = weapon + FWeaponGunAnimOffset;

		if (WeaponGunAnim.RecoilKickADS == 0 || WeaponGunAnim.ShotCameraShake == nullptr || WeaponDeviationConfig.DeviationBase == 0 || WeaponGunConfig.FiringBulletsSpread == 0)
			continue;

		//class ATslWeapon_Trajectory : public ATslWeapon_Gun
		auto pTrajectoryGravityZ = weapon + pTrajectoryGravityZOffset;  //float 重力pointer

		auto pRecoilSpreadScale = weapon + pRecoilSpreadScaleOffset;  //float
		auto pWalkSpread = weapon + pWalkSpreadOffset;  //float
		auto pRunSpread = weapon + pRunSpreadOffset;  //float
		auto pJumpSpread = weapon + pJumpSpreadOffset;  //float

		if (!pJumpSpread || !pTrajectoryGravityZ)
			continue;

#pragma region InstanHit

		TrajectoryConfig.InitialSpeed = TrajectoryConfig.InitialSpeed * 25.0f;     //飞行速度
		TrajectoryConfig.TravelDistanceMax = TrajectoryConfig.TravelDistanceMax * 25.f; //距离
		//mem->write<float>(pTrajectoryGravityZ, 0.0f); //无重力
		TrajectoryConfig.VDragCoefficient = TrajectoryConfig.VDragCoefficient * 0.001;
		TrajectoryConfig.BallisticCurve = nullptr;

#pragma endregion


#pragma region NoSway

		WeaponConfig.SwayModifier_Crouch = 0;
		WeaponConfig.SwayModifier_Movement = 0;
		WeaponConfig.SwayModifier_Pitch = 0;
		WeaponConfig.SwayModifier_Prone = 0;
		WeaponConfig.SwayModifier_Stand = 0;
		WeaponConfig.SwayModifier_YawOffset = 0;

#pragma endregion

#pragma region NoRecoil

		//// TRAJECTORY CONFIG
		//TrajectoryConfig.RecoilPatternScale = 0;
		//TrajectoryConfig.RecoilRecoverySpeed = 0;
		//TrajectoryConfig.RecoilSpeed = 0;

		//// RECOIL INFO
		//RecoilInfo.VerticalRecoilMin = 0;
		//RecoilInfo.VerticalRecoilMax = 0;
		//RecoilInfo.RecoilValue_Climb = 0;
		//RecoilInfo.RecoilValue_Fall = 0;
		//RecoilInfo.RecoilModifier_Stand = 0;
		//RecoilInfo.RecoilModifier_Crouch = 0;
		//RecoilInfo.RecoilModifier_Prone = 0;
		//RecoilInfo.RecoilSpeed_Horizontal = 0;
		//RecoilInfo.RecoilSpeed_Vertical = 0;
		//RecoilInfo.RecoverySpeed_Vertical = 0;
		//RecoilInfo.VerticalRecoveryModifier = 0;
		//RecoilInfo.RecoilHorizontalMinScalar = 0;
		//RecoilInfo.RecoilCurve = DWORD_PTR(0);

		// WEAPON GUN ANIM
		WeaponGunAnim.ShotCameraShake = DWORD_PTR(0);
		WeaponGunAnim.ShotCameraShakeADS = DWORD_PTR(0);
		WeaponGunAnim.ShotCameraShakeIronsight = DWORD_PTR(0);
		WeaponGunAnim.RecoilKickADS = 0.0;


#pragma endregion

		
#pragma region NoSpread

		// TRAJECTORY CONFIG
		//TrajectoryConfig.WeaponSpread = 0;
		//TrajectoryConfig.AimingSpreadModifier = 0;
		//TrajectoryConfig.FiringSpreadBase = 0;
		//TrajectoryConfig.ProneRecoveryTime = 0;
		//TrajectoryConfig.ScopingSpreadModifier = 0;

		//// WEAPON GUN CONFIG
		//WeaponGunConfig.FiringBulletsSpread = 0;

		// WEAPON DEVIATION CONFIG
		WeaponDeviationConfig.DeviationBase = 0;
		WeaponDeviationConfig.DeviationBaseADS = 0;
		WeaponDeviationConfig.DeviationBaseAim = 0;
		WeaponDeviationConfig.DeviationMax = 0;
		WeaponDeviationConfig.DeviationMaxMove = 0;
		WeaponDeviationConfig.DeviationMinMove = 0;
		WeaponDeviationConfig.DeviationMoveMaxReferenceVelocity = 0;
		WeaponDeviationConfig.DeviationMoveMinReferenceVelocity = 0;
		WeaponDeviationConfig.DeviationMoveMultiplier = 0;
		WeaponDeviationConfig.DeviationRecoilGain = 0;
		WeaponDeviationConfig.DeviationRecoilGainADS = 0;
		WeaponDeviationConfig.DeviationRecoilGainAim = 0;
		WeaponDeviationConfig.DeviationStanceCrouch = 0;
		WeaponDeviationConfig.DeviationStanceJump = 0;
		WeaponDeviationConfig.DeviationStanceProne = 0;
		WeaponDeviationConfig.DeviationStanceStand = 0;


		//offsets from class ATslWeapon_Trajectory : public ATslWeapon_Gun
		mem->write<float>(pRecoilSpreadScale, 0.0f);
		mem->write<float>(pWalkSpread, 0.0f);
		mem->write<float>(pRunSpread, 0.0f);
		mem->write<float>(pJumpSpread, 0.0f);
#pragma endregion
		


#pragma region WriteStuctToGame

		mem->write<FWeaponData>(pWeaponConfig, WeaponConfig);
		//mem->write<FRecoilInfo>(pRecoilInfo, RecoilInfo);
		mem->write<FTrajectoryWeaponData>(pTWDate, TrajectoryConfig);
		mem->write<FWeaponDeviationData>(pWeaponDeviationConfig, WeaponDeviationConfig);
		mem->write<FWeaponGunData>(pWeaponGunConfig, WeaponGunConfig);
		mem->write<FWeaponGunAnim>(pWeaponGunAnim, WeaponGunAnim);

#pragma endregion

	}

}

void GameBase::modifyWeaponStreamer() {

	auto playercontroller = mem->read<DWORD_PTR>(pLocalPlayer + PlayerControllerOffset);
	auto pawn = mem->read<DWORD_PTR>(playercontroller + AcknowledgedPawnOffset);
	auto weaponProcessor = mem->read<DWORD_PTR>(pawn + WeaponProcessorOffset); //weapon processor

	auto EquippedWeappons = mem->read<DWORD_PTR>(weaponProcessor + EquippedWeaponArrayOffset); //TAAray EquippedWeappons
	auto CurrentWeaponIndex = mem->read<int>(weaponProcessor + CurrentWeaponIndexOffset);

	auto weapon = mem->read<DWORD_PTR>(EquippedWeappons + 0x0); //current weapon - ATslweapon class
	auto weapinID = mem->read<int>(weapon + AActorIDoffset);

	//class UHackReporterComponent*  HackReporterComponent; // 0x0BC0(0x0008) (CPF_Edit, CPF_ExportObject, CPF_ZeroConstructor, CPF_EditConst, CPF_InstancedReference, CPF_IsPlainOldData)


	for (int i = 0; i < 3; i++)
	{

		weapon = mem->read<DWORD_PTR>(EquippedWeappons + 0x8 * i);

		FWeaponData WeaponConfig;
		WeaponConfig = mem->read<FWeaponData>(weapon + FWeaponDataOffset);
		auto pWeaponConfig = weapon + FWeaponDataOffset;

		FRecoilInfo RecoilInfo;
		RecoilInfo = mem->read<FRecoilInfo>(weapon + FRecoilInfoOffset);
		auto pRecoilInfo = weapon + FRecoilInfoOffset;

		FTrajectoryWeaponData TrajectoryConfig;
		TrajectoryConfig = mem->read<FTrajectoryWeaponData>(weapon + FTrajectoryWeaponDataOffset);
		auto pTWDate = weapon + FTrajectoryWeaponDataOffset;

		if (TrajectoryConfig.HitDamage == 0 || RecoilInfo.VerticalRecoilMax == 0 || WeaponConfig.SwayModifier_Pitch == 0)
			continue;

		FWeaponDeviationData WeaponDeviationConfig;
		WeaponDeviationConfig = mem->read<FWeaponDeviationData>(weapon + FWeaponDeviationDataOffset);
		auto pWeaponDeviationConfig = weapon + FWeaponDeviationDataOffset;

		FWeaponGunData WeaponGunConfig;
		WeaponGunConfig = mem->read<FWeaponGunData>(weapon + FWeaponGunDataOffset);
		auto pWeaponGunConfig = weapon + FWeaponGunDataOffset;

		FWeaponGunAnim WeaponGunAnim;
		WeaponGunAnim = mem->read<FWeaponGunAnim>(weapon + FWeaponGunAnimOffset);
		auto pWeaponGunAnim = weapon + FWeaponGunAnimOffset;

		if (WeaponGunAnim.RecoilKickADS == 0 || WeaponGunAnim.ShotCameraShake == nullptr || WeaponDeviationConfig.DeviationBase == 0 || WeaponGunConfig.FiringBulletsSpread == 0)
			continue;

		//class ATslWeapon_Trajectory : public ATslWeapon_Gun
		auto pTrajectoryGravityZ = weapon + pTrajectoryGravityZOffset;  //float 重力pointer

		auto pRecoilSpreadScale = weapon + pRecoilSpreadScaleOffset;  //float
		auto pWalkSpread = weapon + pWalkSpreadOffset;  //float
		auto pRunSpread = weapon + pRunSpreadOffset;  //float
		auto pJumpSpread = weapon + pJumpSpreadOffset;  //float

		if (!pJumpSpread || !pTrajectoryGravityZ)
			continue;

		float coefficientSpread = 0.5f;

#pragma region NoSway

		WeaponConfig.SwayModifier_Crouch *= recoilReduceCoe;
		WeaponConfig.SwayModifier_Movement *= recoilReduceCoe;
		WeaponConfig.SwayModifier_Pitch *= recoilReduceCoe;
		WeaponConfig.SwayModifier_Prone *= recoilReduceCoe;
		WeaponConfig.SwayModifier_Stand *= recoilReduceCoe;
		WeaponConfig.SwayModifier_YawOffset *= recoilReduceCoe;

#pragma endregion

#pragma region NoRecoil

		//// TRAJECTORY CONFIG
		//TrajectoryConfig.RecoilPatternScale *= recoilReduceCoe;
		//TrajectoryConfig.RecoilRecoverySpeed *= recoilReduceCoe;
		//TrajectoryConfig.RecoilSpeed *= recoilReduceCoe;

		//// RECOIL INFO
		//RecoilInfo.VerticalRecoilMin *= recoilReduceCoe;
		//RecoilInfo.VerticalRecoilMax *= recoilReduceCoe;
		//RecoilInfo.RecoilValue_Climb *= recoilReduceCoe;
		//RecoilInfo.RecoilValue_Fall *= recoilReduceCoe;
		//RecoilInfo.RecoilModifier_Stand *= recoilReduceCoe;
		//RecoilInfo.RecoilModifier_Crouch *= recoilReduceCoe;
		//RecoilInfo.RecoilModifier_Prone *= recoilReduceCoe;
		//RecoilInfo.RecoilSpeed_Horizontal *= recoilReduceCoe;
		//RecoilInfo.RecoilSpeed_Vertical *= recoilReduceCoe;
		//RecoilInfo.RecoverySpeed_Vertical *= recoilReduceCoe;
		//RecoilInfo.VerticalRecoveryModifier *= recoilReduceCoe;
		//RecoilInfo.RecoilHorizontalMinScalar *= recoilReduceCoe;
		//RecoilInfo.RecoilCurve = DWORD_PTR(0);

		// WEAPON GUN ANIM
		WeaponGunAnim.ShotCameraShake = DWORD_PTR(0);
		WeaponGunAnim.ShotCameraShakeADS = DWORD_PTR(0);
		WeaponGunAnim.ShotCameraShakeIronsight = DWORD_PTR(0);
		WeaponGunAnim.RecoilKickADS *= recoilReduceCoe;


		//mem->write<float>(pVerticalRecoilMin, 0.0f);
		//mem->write<float>(pVerticalRecoilMax, 0.0f);

#pragma endregion


#pragma region NoSpread

		//// TRAJECTORY CONFIG
		//TrajectoryConfig.WeaponSpread *= coefficientSpread;
		//TrajectoryConfig.AimingSpreadModifier *= coefficientSpread;
		//TrajectoryConfig.FiringSpreadBase *= coefficientSpread;
		//TrajectoryConfig.ProneRecoveryTime *= coefficientSpread;
		//TrajectoryConfig.ScopingSpreadModifier *= coefficientSpread;

		//// WEAPON GUN CONFIG
		//WeaponGunConfig.FiringBulletsSpread *= coefficientSpread;

		// WEAPON DEVIATION CONFIG
		//WeaponDeviationConfig.DeviationBase *= coefficientSpread;
		WeaponDeviationConfig.DeviationBaseADS *= coefficientSpread;
		WeaponDeviationConfig.DeviationBaseAim *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMax *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMaxMove *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMinMove *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMoveMaxReferenceVelocity *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMoveMinReferenceVelocity *= coefficientSpread;
		//WeaponDeviationConfig.DeviationMoveMultiplier *= coefficientSpread;
		WeaponDeviationConfig.DeviationRecoilGain *= coefficientSpread;
		WeaponDeviationConfig.DeviationRecoilGainADS *= coefficientSpread;
		WeaponDeviationConfig.DeviationRecoilGainAim *= coefficientSpread;
		WeaponDeviationConfig.DeviationStanceCrouch *= coefficientSpread;
		WeaponDeviationConfig.DeviationStanceJump *= coefficientSpread;
		WeaponDeviationConfig.DeviationStanceProne *= coefficientSpread;
		WeaponDeviationConfig.DeviationStanceStand *= coefficientSpread;


		//offsets from class ATslWeapon_Trajectory : public ATslWeapon_Gun
		mem->write<float>(pRecoilSpreadScale, 0.0f);
		mem->write<float>(pWalkSpread, 0.0f);
		mem->write<float>(pRunSpread, 0.0f);
		mem->write<float>(pJumpSpread, 0.0f);
#pragma endregion



#pragma region WriteStuctToGame

		mem->write<FWeaponData>(pWeaponConfig, WeaponConfig);
		//mem->write<FRecoilInfo>(pRecoilInfo, RecoilInfo);
		mem->write<FTrajectoryWeaponData>(pTWDate, TrajectoryConfig);
		mem->write<FWeaponDeviationData>(pWeaponDeviationConfig, WeaponDeviationConfig);
		mem->write<FWeaponGunData>(pWeaponGunConfig, WeaponGunConfig);
		mem->write<FWeaponGunAnim>(pWeaponGunAnim, WeaponGunAnim);

#pragma endregion

	}


}