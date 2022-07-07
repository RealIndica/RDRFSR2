#pragma once
#include "pch.h"
#include "ViewMatrixHook.h"
#include "Dx12ParameterImpl.h"
#include "VkParameterImpl.h"

BOOL IsVK = false;
VkInstance VK_Instance;
VkPhysicalDevice VK_PhysicalDevice;
VkDevice VK_Device;

class FeatureContext;

//Global Context
class CyberFsrContext
{
public:
	std::vector<NVSDK_NGX_Parameter*> Parameters;
	template<class T> NVSDK_NGX_Parameter* AllocateParameter();
	void DeleteParameter(NVSDK_NGX_Parameter* parameter);

	std::unordered_map <unsigned int, FeatureContext*> Contexts;
	FeatureContext* CreateContext();
	void DeleteContext(NVSDK_NGX_Handle* handle);

	static CyberFsrContext& instance()
	{
		static CyberFsrContext INSTANCE;
		return INSTANCE;
	}

private:
	CyberFsrContext() {}
};

class FeatureContext
{
public:
	std::unique_ptr<ViewMatrixHook> ViewMatrix;
	NVSDK_NGX_Handle Handle;
	ID3D12Device* DxDevice;
	VkDevice* VkDevice;
	std::unique_ptr<FfxFsr2Context> FsrContext;

	unsigned int Width{}, Height{}, RenderWidth{}, RenderHeight{};
	NVSDK_NGX_PerfQuality_Value PerfQualityValue = NVSDK_NGX_PerfQuality_Value_Balanced;
	float Sharpness = 1.0f;
	float MVScaleX{}, MVScaleY{};
	float JitterOffsetX{}, JitterOffsetY{};
};

template<class T>
inline NVSDK_NGX_Parameter* CyberFsrContext::AllocateParameter()
{
	auto* instance = new T();
	Parameters.push_back((NVSDK_NGX_Parameter*)instance);
	return instance;
}
