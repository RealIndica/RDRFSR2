#pragma once

struct CameraParams
{
	float FoV;
	float NearPlane;
	float FarPlane;
};

class ViewMatrixHook
{
	CameraParams* camParams = nullptr;

public:
	ViewMatrixHook();

	float GetFov();
	float GetFarPlane();
	float GetNearPlane();
};

