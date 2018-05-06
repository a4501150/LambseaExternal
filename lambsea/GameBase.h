#pragma once
#include "stdafx.h"
#include "Memory.h"
#include "D3D9Render.h"
//#include "D3D11Render\D3D11Renderer.h"


#include <iostream>
#include <vector>

#define M_PI	3.141592654

using namespace std;

//Vector3
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
};

//Rotator
class Rotator
{
public:
	float yaw;
	float pitch;
	float roll;

	Rotator() : yaw(0.f), pitch(0.f), roll(0.f)
	{

	}

	Rotator(float _x, float _y, float _z) : yaw(_x), pitch(_y), roll(_z)
	{

	}
	~Rotator()
	{

	}


};

//Required PUBG classes
struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

struct FMinimalViewInfo
{
	class Vector3                                      Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class Vector3                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0024(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              AspectRatio;                                              // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

struct FCameraCacheEntry
{
	float                                              TimeStamp;                                                // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0xC];                                       // 0x0004(0x000C) MISSED OFFSET
	struct FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530)
};

enum Bones : int
{
	Root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	face_root = 7,
	eyebrows_pos_root = 8,
	eyebrows_root = 9,
	eyebrows_r = 10,
	eyebrows_l = 11,
	eyebrow_l = 12,
	eyebrow_r = 13,
	forehead_root = 14,
	forehead = 15,
	jaw_pos_root = 16,
	jaw_root = 17,
	jaw = 18,
	mouth_down_pos_root = 19,
	mouth_down_root = 20,
	lip_bm_01 = 21,
	lip_bm_02 = 22,
	lip_br = 23,
	lip_bl = 24,
	jaw_01 = 25,
	jaw_02 = 26,
	cheek_pos_root = 27,
	cheek_root = 28,
	cheek_r = 29,
	cheek_l = 30,
	nose_side_root = 31,
	nose_side_r_01 = 32,
	nose_side_r_02 = 33,
	nose_side_l_01 = 34,
	nose_side_l_02 = 35,
	eye_pos_r_root = 36,
	eye_r_root = 37,
	eye_rot_r_root = 38,
	eye_lid_u_r = 39,
	eye_r = 40,
	eye_lid_b_r = 41,
	eye_pos_l_root = 42,
	eye_l_root = 43,
	eye_rot_l_root = 44,
	eye_lid_u_l = 45,
	eye_l = 46,
	eye_lid_b_l = 47,
	nose_pos_root = 48,
	nose = 49,
	mouth_up_pos_root = 50,
	mouth_up_root = 51,
	lip_ul = 52,
	lip_um_01 = 53,
	lip_um_02 = 54,
	lip_ur = 55,
	lip_l = 56,
	lip_r = 57,
	hair_root = 58,
	hair_b_01 = 59,
	hair_b_02 = 60,
	hair_l_01 = 61,
	hair_l_02 = 62,
	hair_r_01 = 63,
	hair_r_02 = 64,
	hair_f_02 = 65,
	hair_f_01 = 66,
	hair_b_pt_01 = 67,
	hair_b_pt_02 = 68,
	hair_b_pt_03 = 69,
	hair_b_pt_04 = 70,
	hair_b_pt_05 = 71,
	camera_fpp = 72,
	GunReferencePoint = 73,
	GunRef = 74,
	breast_l = 75,
	breast_r = 76,
	clavicle_l = 77,
	upperarm_l = 78,
	lowerarm_l = 79,
	hand_l = 80,
	thumb_01_l = 81,
	thumb_02_l = 82,
	thumb_03_l = 83,
	thumb_04_l_MBONLY = 84,
	index_01_l = 85,
	index_02_l = 86,
	index_03_l = 87,
	index_04_l_MBONLY = 88,
	middle_01_l = 89,
	middle_02_l = 90,
	middle_03_l = 91,
	middle_04_l_MBONLY = 92,
	ring_01_l = 93,
	ring_02_l = 94,
	ring_03_l = 95,
	ring_04_l_MBONLY = 96,
	pinky_01_l = 97,
	pinky_02_l = 98,
	pinky_03_l = 99,
	pinky_04_l_MBONLY = 100,
	item_l = 101,
	lowerarm_twist_01_l = 102,
	upperarm_twist_01_l = 103,
	clavicle_r = 104,
	upperarm_r = 105,
	lowerarm_r = 106,
	hand_r = 107,
	thumb_01_r = 108,
	thumb_02_r = 109,
	thumb_03_r = 110,
	thumb_04_r_MBONLY = 111,
	index_01_r = 112,
	index_02_r = 113,
	index_03_r = 114,
	index_04_r_MBONLY = 115,
	middle_01_r = 116,
	middle_02_r = 117,
	middle_03_r = 118,
	middle_04_r_MBONLY = 119,
	ring_01_r = 120,
	ring_02_r = 121,
	ring_03_r = 122,
	ring_04_r_MBONLY = 123,
	pinky_01_r = 124,
	pinky_02_r = 125,
	pinky_03_r = 126,
	pinky_04_r_MBONLY = 127,
	item_r = 128,
	lowerarm_twist_01_r = 129,
	upperarm_twist_01_r = 130,
	BackPack = 131,
	backpack_01 = 132,
	backpack_02 = 133,
	Slot_Primary = 134,
	Slot_Secondary = 135,
	Slot_Melee = 136,
	slot_throwable = 137,
	coat_l_01 = 138,
	coat_l_02 = 139,
	coat_l_03 = 140,
	coat_l_04 = 141,
	coat_fl_01 = 142,
	coat_fl_02 = 143,
	coat_fl_03 = 144,
	coat_fl_04 = 145,
	coat_b_01 = 146,
	coat_b_02 = 147,
	coat_b_03 = 148,
	coat_b_04 = 149,
	coat_r_01 = 150,
	coat_r_02 = 151,
	coat_r_03 = 152,
	coat_r_04 = 153,
	coat_fr_01 = 154,
	coat_fr_02 = 155,
	coat_fr_03 = 156,
	coat_fr_04 = 157,
	thigh_l = 158,
	calf_l = 159,
	foot_l = 160,
	ball_l = 161,
	calf_twist_01_l = 162,
	thigh_twist_01_l = 163,
	thigh_r = 164,
	calf_r = 165,
	foot_r = 166,
	ball_r = 167,
	calf_twist_01_r = 168,
	thigh_twist_01_r = 169,
	Slot_SideArm = 170,
	skirt_l_01 = 171,
	skirt_l_02 = 172,
	skirt_l_03 = 173,
	skirt_f_01 = 174,
	skirt_f_02 = 175,
	skirt_f_03 = 176,
	skirt_b_01 = 177,
	skirt_b_02 = 178,
	skirt_b_03 = 179,
	skirt_r_01 = 180,
	skirt_r_02 = 181,
	skirt_r_03 = 182,
	ik_hand_root = 183,
	ik_hand_gun = 184,
	ik_hand_r = 185,
	ik_hand_l = 186,
	ik_aim_root = 187,
	ik_aim_l = 188,
	ik_aim_r = 189,
	ik_foot_root = 190,
	ik_foot_l = 191,
	ik_foot_r = 192,
	camera_tpp = 193,
	ik_target_root = 194,
	ik_target_l = 195,
	ik_target_r = 196,
	VB_spine_03_spine_03 = 197,
	VB_upperarm_r_lowerarm_r = 198
};


// ScriptStruct TslGame.WeaponData
// 0x0098
struct FWeaponData
{
	float                                              TargetingFOV;                                             // 0x0000(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              HoldBreathFOV;                                            // 0x0004(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      Rarity[0x8];                                              // 0x0008(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     SocketOffset_Shoulder;                                    // 0x0010(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     SocketOffset_Hand;                                        // 0x001C(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	bool                                               bApplyGripPoseLeft;                                       // 0x0028(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      WeaponGripLeft;                                           // 0x0029(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      WeaponClass;                                              // 0x002A(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bUseDefaultScoreMultiplier;                               // 0x002B(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ScoreMultiplierByDamage;                                  // 0x002C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ScoreMultiplierByKill;                                    // 0x0030(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_Pitch;                                       // 0x0034(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_YawOffset;                                   // 0x0038(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_Movement;                                    // 0x003C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_Stand;                                       // 0x0040(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_Crouch;                                      // 0x0044(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SwayModifier_Prone;                                       // 0x0048(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              CameraDOF_Range;                                          // 0x004C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              CameraDOF_NearRange;                                      // 0x0050(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              CameraDOF_Power;                                          // 0x0054(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bUseDynamicReverbAK;                                      // 0x0058(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0059(0x0007) MISSED OFFSET
	unsigned char                                      IronSightZeroingDistances[0x10];                          // 0x0060(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
	float                                              AnimationKick;                                            // 0x0070(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x0074(0x0004) MISSED OFFSET
	class UAnimMontage*                                RecoilMontage;                                            // 0x0078(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               DestructibleDoor;                                         // 0x0080(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      ThrownType;                                               // 0x0081(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData02[0x2];                                       // 0x0082(0x0002) MISSED OFFSET
	float                                              WeaponEquipDuration;                                      // 0x0084(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              WeaponReadyDuration;                                      // 0x0088(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bForceFireAfterEquip;                                     // 0x008C(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData03[0x3];                                       // 0x008D(0x0003) MISSED OFFSET
	float                                              PhysicalBodyHitImpactPower;                               // 0x0090(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData04[0x4];                                       // 0x0094(0x0004) MISSED OFFSET
};

// ScriptStruct TslGame.RecoilInfo
// 0x0068
struct FRecoilInfo
{
	float                                              VerticalRecoilMin;                                        // 0x0000(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VerticalRecoilMax;                                        // 0x0004(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VerticalRecoilVariation;                                  // 0x0008(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VerticalRecoveryModifier;                                 // 0x000C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VerticalRecoveryClamp;                                    // 0x0010(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VerticalRecoveryMax;                                      // 0x0014(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              LeftMax;                                                  // 0x0018(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RightMax;                                                 // 0x001C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              HorizontalTendency;                                       // 0x0020(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x0024(0x0004) MISSED OFFSET
	DWORD_PTR*							               RecoilCurve;                                              // 0x0028(0x0008) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	int                                                BulletsPerSwitch;                                         // 0x0030(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              TimePerSwitch;                                            // 0x0034(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      bSwitchOnTime : 1;                                        // 0x0038(0x0001) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      UnknownData01[0x3];                                       // 0x0039(0x0003) MISSED OFFSET
	float                                              RecoilSpeed_Vertical;                                     // 0x003C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilSpeed_Horizontal;                                   // 0x0040(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoverySpeed_Vertical;                                   // 0x0044(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilValue_Climb;                                        // 0x0048(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilValue_Fall;                                         // 0x004C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilModifier_Stand;                                     // 0x0050(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilModifier_Crouch;                                    // 0x0054(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilModifier_Prone;                                     // 0x0058(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RecoilHorizontalMinScalar;                                // 0x005C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      UnknownData02[0x8];                                       // 0x0060(0x0008) MISSED OFFSET
};

// ScriptStruct TslGame.TrajectoryWeaponData
// 0x0080
struct FTrajectoryWeaponData
{
	float                                              WeaponSpread;                                             // 0x0000(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              AimingSpreadModifier;                                     // 0x0004(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ScopingSpreadModifier;                                    // 0x0008(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              FiringSpreadBase;                                         // 0x000C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              StandRecoveryTime;                                        // 0x0010(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              CrouchRecoveryTime;                                       // 0x0014(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ProneRecoveryTime;                                        // 0x0018(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RecoveryInterval;                                         // 0x001C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RecoilSpeed;                                              // 0x0020(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RecoilRecoverySpeed;                                      // 0x0024(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RecoilPatternScale;                                       // 0x0028(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              InitialSpeed;                                             // 0x002C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	int                                                HitDamage;                                                // 0x0030(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x0034(0x0004) MISSED OFFSET
	class UCurveVector*                                BallisticCurve;                                           // 0x0038(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RangeModifier;                                            // 0x0040(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReferenceDistance;                                        // 0x0044(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              TravelDistanceMax;                                        // 0x0048(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              SimulationSubstepTime;                                    // 0x004C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              VDragCoefficient;                                         // 0x0050(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              BDS;                                                      // 0x0054(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bUseAdvancedBallistics;                                   // 0x0058(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               IsPenetrable;                                             // 0x0059(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData01[0x6];                                       // 0x005A(0x0006) MISSED OFFSET
	class UClass*                                      DamageType;                                               // 0x0060(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char									   RecoilPatterns[0x10];                                     // 0x0068(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
	unsigned char                                      bIsTrajectoryReplicationEnabled : 1;                      // 0x0078(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      UnknownData02[0x7];                                       // 0x0079(0x0007) MISSED OFFSET
};

// ScriptStruct TslGame.WeaponDeviationData
// 0x0044
struct FWeaponDeviationData
{
	float                                              DeviationBase;                                            // 0x0000(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationBaseAim;                                         // 0x0004(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationBaseADS;                                         // 0x0008(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              MOA;                                                      // 0x000C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationRecoilGain;                                      // 0x0010(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationRecoilGainAim;                                   // 0x0014(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationRecoilGainADS;                                   // 0x0018(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMax;                                             // 0x001C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMinMove;                                         // 0x0020(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMaxMove;                                         // 0x0024(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMoveMultiplier;                                  // 0x0028(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMoveMinReferenceVelocity;                        // 0x002C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationMoveMaxReferenceVelocity;                        // 0x0030(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationStanceStand;                                     // 0x0034(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationStanceCrouch;                                    // 0x0038(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationStanceProne;                                     // 0x003C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              DeviationStanceJump;                                      // 0x0040(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
};

// ScriptStruct TslGame.WeaponGunData
// 0x0090
struct FWeaponGunData
{
	class UClass*                                      AmmoItemClass;                                            // 0x0000(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UParticleSystem*                             TracerClass;                                              // 0x0008(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	int                                                AmmoPerClip;                                              // 0x0010(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              TimeBetweenShots;                                         // 0x0014(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              NoAnimReloadDuration;                                     // 0x0018(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
	unsigned char                                      FiringModes[0x10];                                        // 0x0020(0x0010) (Edit, ZeroConstructor, DisableEditOnInstance)
	int                                                BurstShots;                                               // 0x0030(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              BurstDelay;                                               // 0x0034(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	int                                                BulletPerFiring;                                          // 0x0038(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              FiringBulletsSpread;                                      // 0x003C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bIsArrowProjectile;                                       // 0x0040(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bRotationFromBarrelWhenScoped;                            // 0x0041(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      ReloadMethod;                                             // 0x0042(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData01[0x1];                                       // 0x0043(0x0001) MISSED OFFSET
	float                                              ReloadDurationByOneInitial;                               // 0x0044(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationByOneLoop;                                  // 0x0048(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              MovementModifierAim;                                      // 0x004C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              MovementModifierScope;                                    // 0x0050(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              WeaponLength;                                             // 0x0054(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ShoulderLength;                                           // 0x0058(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              WeaponSuppressorLength;                                   // 0x005C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              TraceRadius;                                              // 0x0060(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              TraceAdditiveZ;                                           // 0x0064(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               DebugWeaponCollision;                                     // 0x0068(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData02[0x3];                                       // 0x0069(0x0003) MISSED OFFSET
	struct Vector3                                     HandWeaponOffset;                                         // 0x006C(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     HandWeaponOffsetFPP;                                      // 0x0078(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	bool                                               bManualCycleAfterShot;                                    // 0x0084(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bManualCycleInitialStateReady;                            // 0x0085(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData03[0x2];                                       // 0x0086(0x0002) MISSED OFFSET
	float                                              LongTailDelay;                                            // 0x0088(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bUseConstScope;                                           // 0x008C(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData04[0x3];                                       // 0x008D(0x0003) MISSED OFFSET
};

// 0x00F0
struct FWeaponGunAnim
{
	class UAnimMontage*                                Fire;                                                     // 0x0000(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      bLoopedFire : 1;                                          // 0x0008(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0009(0x0007) MISSED OFFSET
	class UAnimMontage*                                Reload;                                                   // 0x0010(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UBlendSpace1D*                               CharacterGripBlendspace;                                  // 0x0018(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UBlendSpaceBase*                             CharacterLHGripBlendspace;                                // 0x0020(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterFire;                                            // 0x0028(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterFireCycle;                                       // 0x0030(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterFireSelector;                                    // 0x0038(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterReloadTactical;                                  // 0x0040(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterReloadCharge;                                    // 0x0048(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterReloadByOneStart;                                // 0x0050(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterReloadByOneStop;                                 // 0x0058(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                CharacterReloadByOneSingle;                               // 0x0060(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                WeaponReloadTactical;                                     // 0x0068(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UAnimMontage*                                WeaponReloadCharge;                                       // 0x0070(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationTactical;                                   // 0x0078(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationCharge;                                     // 0x007C(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationStart;                                      // 0x0080(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationLoop;                                       // 0x0084(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              ReloadDurationMagOut;                                     // 0x0088(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bUseBoltAction;                                           // 0x008C(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData01[0x3];                                       // 0x008D(0x0003) MISSED OFFSET
	float                                              FireCycleDelay;                                           // 0x0090(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              FireCycleDuration;                                        // 0x0094(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              CycleAfterReloadDelay;                                    // 0x0098(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bCycleAfterLastShot;                                      // 0x009C(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	bool                                               bCycleDuringReload;                                       // 0x009D(0x0001) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData02[0x2];                                       // 0x009E(0x0002) MISSED OFFSET
	class UClass*                                      ShotCameraShake;                                          // 0x00A0(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UClass*                                      ShotCameraShakeIronsight;                                 // 0x00A8(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UClass*                                      ShotCameraShakeADS;                                       // 0x00B0(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	class UCameraAnim*                                 CycleCameraAnim;                                          // 0x00B8(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              RecoilKickADS;                                            // 0x00C0(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     RecoilADSSocketOffsetScale;                               // 0x00C4(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     MagDropLinearVelocity;                                    // 0x00D0(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	struct Vector3                                     MagDropAngularVelocity;                                   // 0x00DC(0x000C) (Edit, DisableEditOnInstance, IsPlainOldData)
	float                                              InertiaInterpMultiplier;                                  // 0x00E8(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	float                                              InertiaClampMultiplier;                                   // 0x00EC(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
};



struct EncryptionTableStruct {
	DWORD EncryptionKey[1024];
};

struct TSLEncrypted_Actor
{
	uint64_t ptr_table[43];
	uint16_t index;
	byte  pad1[6];
	uint16_t xor;
	byte  pad2[6];
};

struct BoneArrayStruct {
	FTransform BoneArray[198];
};

namespace GameBase {

	//offset
	extern DWORD_PTR WorldOffset;
	extern DWORD_PTR GNameOffset;
	extern DWORD_PTR FinishCurrentFrameOffset;

	//Required Global
	extern DWORD_PTR pUWorld;
	extern DWORD_PTR pLocalPlayer;
	extern FCameraCacheEntry Cameracache;

	//Screen
	extern int screen_width;
	extern int screen_height;

	extern D3D9Render* render;
	//extern D3D11Renderer* render;
	extern Memory* mem;


	//Switch
	extern bool bPlayer;
	extern bool bVehicle;
	extern bool bLoot;
	extern bool bBot;
	extern bool bWeaponRemoval;
	extern bool bMenu;

	extern int aimPlayerId;
	extern bool aimHead;

	//cache gname
	void DumpGNAME();

	//update pov
	void UpdateActualWorld();
	void UpdateAddresses();
	FCameraCacheEntry UpdateCameraCache();

	//General Game Functions

	//Pre
	Vector3 WorldToScreen(Vector3 &WorldLocation, FCameraCacheEntry &CameraCacheL);
	FTransform GetBoneIndex(DWORD_PTR mesh, int index);
	Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id);

	//Section 01 - ESP Basic
	DWORD_PTR GetEntityList();
	int GetEntityCount();
	int GetEntityID(DWORD_PTR entity);

	//Section 02 - Local Player Related
	Vector3 GetLocalPlayerPos();
	int GetLocalPlayerTeamId();
	int GetLocalPlayerHUDId();
	int GetLocalPlayerPawnId();

	//Section 03 - Player ESP 
	Vector3 GetActorPos(DWORD_PTR entity);
	wstring GetPlayerName(DWORD_PTR entity);
	int GetActorTeamId(DWORD_PTR pAActor);
	float GetActorHealth(DWORD_PTR pAActor);
	int GetPlayerID(DWORD_PTR pAActor);
	DWORD_PTR GetMesh(DWORD_PTR pAActor);
	bool isVisible(DWORD_PTR mesh);

	//Section 04 - Vehicle ESP
	wstring GetVehicleInfo(int id);

	//Section 05 - Loot ESP
#define IS_BIT_SET(var,pos) ((var) & (1<<(pos)));
	DWORD_PTR GetDroppedItemGroupArray(DWORD_PTR entity);
	int GetDroppedItemGroupArrayCount(DWORD_PTR entity);


	//Section 06 - Misc
	inline bool GetBitFieldBool(unsigned char bitFiled, int position);
	
	inline unsigned char ToggleBitFiled(unsigned char bitFiled, int position);


	//PlayerController + 0x0708
	//unsigned char    bIsSpectated : 1;    // 0x0708(0x0001) (CPF_Net, CPF_Transient)
	bool GetSpectatedStatus();
	void DisableHackerReport();


	BoneArrayStruct GetBoneArray(DWORD_PTR mesh);
	Vector3 GetBoneWithRotationEx(BoneArrayStruct &bas, int id, FTransform &ComponentToWorld);

	//Drawing
	//void DrawSkeleton(DWORD_PTR mesh);
	void DrawSkeleton(BoneArrayStruct &BAS, FTransform &ComponentToWorld, bool isVisible);
	void DrawVehicle(DWORD_PTR entity, Vector3 &local, const wchar_t* info);

	//AimBot
	void AimAtPosAngles(Vector3 &TargetLocation, int targetUID);
	bool inAimRange(Vector3 &targetInScreen);



	//EntityLoop
	DWORD_PTR GetDecryptedEntity(DWORD_PTR EncryptedEntityPtr, EncryptionTableStruct &EncryptionTableS);
	DWORD_PTR GetDecryptedEntityEx(TSLEncrypted_Actor* EntityArray, int index, EncryptionTableStruct &EncryptionTableS);
	void EntityLoop();

	//Menu
	void Menu();

	//Weapon Removal
	extern bool streamerMode;
	extern float recoilReduceCoe;

	void modifyWeapon();
	void modifyWeaponStreamer();

	BOOL SetupOffsets();

}


