#include "pch.h"
#include "ViewMatrixHook.h"

ViewMatrixHook::ViewMatrixHook()
{
	uintptr_t mod = (uintptr_t)GetModuleHandle(NULL);
	camParams = (CameraParams*)(mod + 0x3E806E0);
}

float ViewMatrixHook::GetFov()
{
	return camParams->FoV;
}

float ViewMatrixHook::GetFarPlane()
{
	return camParams->FarPlane;
}

float ViewMatrixHook::GetNearPlane()
{
	return camParams->NearPlane;
}
