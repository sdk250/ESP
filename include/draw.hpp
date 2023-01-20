#pragma once

#ifndef __DRAW_H__
#define __DRAW_H__

// system headers file
#include <dlfcn.h>
#include <GLES3/gl3.h>
#include <sys/syscall.h>
#include <linux/uio.h>
#include <cstdint>
// user headers file
#include "global_value"
#include "init_egl.hpp"
#include "android_touch.hpp"
#include "ui/PixelFormat.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"

#include "libnwindow.so.h"
#include "ttf.h"

#define BUFFER_SIZE sizeof(uint64_t)
#define PACKAGE "com.tencent.ig"
#define LIBRARY "libUE4.so"
#define MAX_PLAYER 100

struct Vec4 {
	float x, y, z, w;
};
struct Vec3 {
	float x, y, z;
};
struct Vec2 {
	float x, y;
};
struct D3DMatrix
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
};
struct D2DVector
{
	float x, y;
};
struct uLevel
{
	uint64_t addr;
	int size;
};
struct Actors
{
	uint64_t
		Enc_1, Enc_2,
		Enc_3, Enc_4;
};
struct Chunk
{
	uint32_t
		val_1, val_2,
		val_3, val_4,
		val_5, val_6,
		val_7, val_8;
};
struct PlayerBone
{
	bool on = false;
	Vec2 neck;
	Vec2 cheast;
	Vec2 pelvis;
	Vec2 lSh;
	Vec2 rSh;
	Vec2 lElb;
	Vec2 rElb;
	Vec2 lWr;
	Vec2 rWr;
	Vec2 lTh;
	Vec2 rTh;
	Vec2 lKn;
	Vec2 rKn;
	Vec2 lAn;
	Vec2 rAn;
};
struct PlayerData
{
	char PlayerName[100];
	int TeamID;
	float Health;
	float Distance;
	bool isBot;
	struct Vec4 Pelvis;
	struct PlayerBone Bone {false};
};

// type define long long int = void * on 64-bit machine
uint64_t get_module_base(const char*);
int get_pid(const char*);
inline struct D3DMatrix ToMatrixWithScale(
	struct Vec4,
	struct Vec3,
	struct Vec3
);
template <typename T>
inline T getA(uint64_t);
inline void getB(void *, uint64_t, size_t);
inline struct Vec2 _World2Screen(
	struct D3DMatrix,
	struct Vec3
);
inline float getDistance(struct Vec3, struct Vec3);
std::string getNameByte(uint64_t);
inline bool isInvalid(uint64_t);
uint64_t DecryptActorsArray(uint64_t, int, int);
char* getText(uint64_t);
inline struct PlayerBone getPlayerBone(uint64_t, struct D3DMatrix);
inline struct Vec3 mat2Cord(struct D3DMatrix, struct D3DMatrix);
inline struct D3DMatrix getOMatrix(uint64_t);
inline struct Vec4 posCount(struct D3DMatrix, struct Vec3);

// Variable
bool shutdown = false;
uint32_t width = 300;
uint32_t height = 400;
unsigned short int orientation = 0;
static int PID = -1;

// Constant offset
namespace Offsets
{
	uint64_t Mesh = 0x420;
	uint64_t FixAttachInfoList = 0x1a0;
	uint64_t MinLOD = 0x7A8;
	uint64_t Children = 0x198;
	uint64_t DrawShootLineTime = 0x134;
	uint64_t UploadInterval = 0x170;
	uint64_t CurBulletNumInClip = 0xe28;
	uint64_t CurMaxBulletNumInOneClip = 0xe48;
	uint64_t bDead = 0xcec;
	uint64_t Health = 0xcd0;
	uint64_t TeamID = 0x8c8;
	uint64_t Role = 0x148;
	uint64_t ObserverCameraFPPMode = 0x4208;
	uint64_t isBot = 0x958;
	uint64_t PlayerName = 0x880;
	uint64_t Nation = 0x890;
	uint64_t PlayerUID = 0x8a8;
	uint64_t Controller = 0x3f8;
	uint64_t ControlRotation = 0x3f0;
	uint64_t bIsWeaponFiring = 0x1508;
	uint64_t bIsGunADS = 0xf71;
	uint64_t RootComponent = 0x1a8;
	uint64_t ParachuteEquipItems = 0x1b0;
	uint64_t CurrWeapon = 0x4f0;
	uint64_t ShootWeaponEntityComp = 0xf68;
    uint64_t WeaponManagerComponent = 0x2008;
	uint64_t AccessoriesVRecoilFactor = 0xaa8;
	uint64_t AccessoriesHRecoilFactor = 0xaac;
	uint64_t AccessoriesRecoveryFactor = 0xab0;
	uint64_t RecoilKickADS = 0xbb8;
	uint64_t AnimationKick = 0xbd4;
	uint64_t GameDeviationFactor = 0xb04;
	uint64_t BulletFireSpeed = 0x4e0;
	uint64_t ShootInterval = 0x510;
	uint64_t ExtraHitPerformScale = 0xbbc;
	uint64_t SwitchWeaponSpeedScale = 0x25d8;
	uint64_t ThirdPersonCameraComponent = 0x1918;
	uint64_t FieldOfView = 0x2cc;
};
#endif