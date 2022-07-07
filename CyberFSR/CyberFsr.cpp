#include "pch.h"
#include "CyberFsr.h"
#include "Util.h"

#pragma region DX12

NVSDK_NGX_Result NVSDK_NGX_D3D12_Init_Ext(unsigned long long InApplicationId, const wchar_t* InApplicationDataPath,
	ID3D12Device* InDevice, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo, NVSDK_NGX_Version InSDKVersion,
	unsigned long long unknown0) 
{
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_D3D12_Init(unsigned long long InApplicationId, const wchar_t* InApplicationDataPath,
	ID3D12Device* InDevice, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo, NVSDK_NGX_Version InSDKVersion)
{
	IsVK = false;
	return NVSDK_NGX_D3D12_Init_Ext(InApplicationId, InApplicationDataPath, InDevice, InFeatureInfo, InSDKVersion, 0);
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_D3D12_Shutdown(void)
{
	CyberFsrContext::instance().Parameters.clear();
	CyberFsrContext::instance().Contexts.clear();
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_D3D12_Shutdown1(ID3D12Device* InDevice)
{
	CyberFsrContext::instance().Parameters.clear();
	CyberFsrContext::instance().Contexts.clear();
	return NVSDK_NGX_Result_Success;
}

//Deprecated Parameter Function - Internal Memory Tracking
NVSDK_NGX_Result NVSDK_NGX_D3D12_GetParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = CyberFsrContext::instance().AllocateParameter<Dx12ParameterImpl>();
	return NVSDK_NGX_Result_Success;
}

//TODO External Memory Tracking
NVSDK_NGX_Result NVSDK_NGX_D3D12_GetCapabilityParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = new Dx12ParameterImpl();
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_D3D12_AllocateParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = new Dx12ParameterImpl();
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_D3D12_DestroyParameters(NVSDK_NGX_Parameter* InParameters)
{
	delete InParameters;
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_D3D12_GetScratchBufferSize(NVSDK_NGX_Feature InFeatureId,
	const NVSDK_NGX_Parameter* InParameters, size_t* OutSizeInBytes)
{
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_D3D12_CreateFeature(ID3D12GraphicsCommandList* InCmdList, NVSDK_NGX_Feature InFeatureID,
	const NVSDK_NGX_Parameter* InParameters, NVSDK_NGX_Handle** OutHandle)
{
	const auto inParams = dynamic_cast<const Dx12ParameterImpl*>(InParameters);

	ID3D12Device* device;
	InCmdList->GetDevice(IID_PPV_ARGS(&device));
	auto deviceContext = CyberFsrContext::instance().CreateContext();
	deviceContext->ViewMatrix = std::make_unique<ViewMatrixHook>();

	*OutHandle = &deviceContext->Handle;

	FfxFsr2ContextDescription initParams = {};
	const size_t scratchBufferSize = ffxFsr2GetScratchMemorySizeDX12();
	void* scratchBuffer = malloc(scratchBufferSize);
	FfxErrorCode errorCode = ffxFsr2GetInterfaceDX12(&initParams.callbacks, device, scratchBuffer, scratchBufferSize);
	FFX_ASSERT(errorCode == FFX_OK);

	initParams.device = ffxGetDeviceDX12(device);
	initParams.maxRenderSize.width = inParams->Width;
	initParams.maxRenderSize.height = inParams->Height;
	initParams.displaySize.width = inParams->OutWidth;
	initParams.displaySize.height = inParams->OutHeight;
	initParams.flags = (inParams->DepthInverted) ? FFX_FSR2_ENABLE_DEPTH_INVERTED : 0
		| (inParams->AutoExposure) ? FFX_FSR2_ENABLE_AUTO_EXPOSURE : 0
		| (inParams->Hdr) ? FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE : 0
		| (inParams->JitterMotion) ? FFX_FSR2_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION : 0
		| (!inParams->LowRes) ? FFX_FSR2_ENABLE_DISPLAY_RESOLUTION_MOTION_VECTORS : 0;

	deviceContext->FsrContext = std::make_unique<FfxFsr2Context>();

	ffxFsr2ContextCreate(deviceContext->FsrContext.get(), &initParams);

	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_D3D12_ReleaseFeature(NVSDK_NGX_Handle* InHandle)
{
	auto deviceContext = CyberFsrContext::instance().Contexts[InHandle->Id];
	FfxErrorCode errorCode = ffxFsr2ContextDestroy(deviceContext->FsrContext.get());
	FFX_ASSERT(errorCode == FFX_OK);
	CyberFsrContext::instance().DeleteContext(InHandle);
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_D3D12_EvaluateFeature(ID3D12GraphicsCommandList* InCmdList, const NVSDK_NGX_Handle* InFeatureHandle, const NVSDK_NGX_Parameter* InParameters, PFN_NVSDK_NGX_ProgressCallback InCallback)
{
	ID3D12Device* device;
	InCmdList->GetDevice(IID_PPV_ARGS(&device));
	auto deviceContext = CyberFsrContext::instance().Contexts[InFeatureHandle->Id];

	const auto inParams = dynamic_cast<const Dx12ParameterImpl*>(InParameters);

	auto* fsrContext = deviceContext->FsrContext.get();

	FfxFsr2DispatchDescription dispatchParameters = {};
	dispatchParameters.commandList = ffxGetCommandListDX12(InCmdList);
	dispatchParameters.color = ffxGetResourceDX12(fsrContext, inParams->Color, (wchar_t*)L"FSR2_InputColor");
	dispatchParameters.depth = ffxGetResourceDX12(fsrContext, inParams->Depth, (wchar_t*)L"FSR2_InputDepth");
	dispatchParameters.motionVectors = ffxGetResourceDX12(fsrContext, inParams->MotionVectors, (wchar_t*)L"FSR2_InputMotionVectors");
	dispatchParameters.exposure = ffxGetResourceDX12(fsrContext, inParams->ExposureTexture, (wchar_t*)L"FSR2_InputExposure");
	dispatchParameters.reactive = ffxGetResourceDX12(fsrContext, inParams->InputBiasCurrentColorMask, (wchar_t*)L"FSR2_InputReactiveMap");
	dispatchParameters.transparencyAndComposition = ffxGetResourceDX12(fsrContext, inParams->TransparencyMask, (wchar_t*)L"FSR2_TransparencyAndCompositionMap");
	dispatchParameters.output = ffxGetResourceDX12(fsrContext, inParams->Output, (wchar_t*)L"FSR2_OutputUpscaledColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);

	dispatchParameters.jitterOffset.x = inParams->JitterOffsetX;
	dispatchParameters.jitterOffset.y = inParams->JitterOffsetY;

	dispatchParameters.motionVectorScale.x = (float)inParams->MVScaleX;
	dispatchParameters.motionVectorScale.y = (float)inParams->MVScaleY;

	dispatchParameters.reset = inParams->ResetRender;
	dispatchParameters.enableSharpening = inParams->EnableSharpening;
	dispatchParameters.sharpness = inParams->Sharpness;

	//deltatime hax
	static double lastFrameTime;
	double currentTime = Util::MillisecondsNow();
	double deltaTime = (currentTime - lastFrameTime);
	lastFrameTime = currentTime;

	dispatchParameters.frameTimeDelta = (float)deltaTime;
	dispatchParameters.preExposure = 1.0f;
	dispatchParameters.renderSize.width = inParams->Width;
	dispatchParameters.renderSize.height = inParams->Height;

	//Hax Zone
	dispatchParameters.cameraFar = deviceContext->ViewMatrix->GetFarPlane();
	dispatchParameters.cameraNear = deviceContext->ViewMatrix->GetNearPlane();
	dispatchParameters.cameraFovAngleVertical = DirectX::XMConvertToRadians(deviceContext->ViewMatrix->GetFov());
	FfxErrorCode errorCode = ffxFsr2ContextDispatch(fsrContext, &dispatchParameters);
	FFX_ASSERT(errorCode == FFX_OK);

	return NVSDK_NGX_Result_Success;
}

#pragma endregion

#pragma region VK

NVSDK_NGX_Result NVSDK_NGX_VULKAN_Init_Ext(unsigned long long InApplicationId, const wchar_t* InApplicationDataPath, VkInstance InInstance, VkPhysicalDevice InPD, VkDevice InDevice,
	const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo, NVSDK_NGX_Version InSDKVersion, 
	unsigned long long unknown0)
{
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_VULKAN_Init(unsigned long long InApplicationId, const wchar_t* InApplicationDataPath, VkInstance InInstance, VkPhysicalDevice InPD, VkDevice InDevice,
	const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo, NVSDK_NGX_Version InSDKVersion)
{
	IsVK = true;
	VK_Instance = InInstance;
	VK_PhysicalDevice = InPD;
	VK_Device = InDevice;
	return NVSDK_NGX_VULKAN_Init_Ext(InApplicationId, InApplicationDataPath, InInstance, InPD, InDevice, InFeatureInfo, InSDKVersion, 0);
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_VULKAN_Shutdown(void)
{
	CyberFsrContext::instance().Parameters.clear();
	CyberFsrContext::instance().Contexts.clear();
	VK_Instance = NULL;
	VK_PhysicalDevice = NULL;
	VK_Device = NULL;
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_VULKAN_Shutdown1(VkDevice* InDevice)
{
	CyberFsrContext::instance().Parameters.clear();
	CyberFsrContext::instance().Contexts.clear();
	VK_Instance = NULL;
	VK_PhysicalDevice = NULL;
	VK_Device = NULL;
	return NVSDK_NGX_Result_Success;
}

//Deprecated Parameter Function - Internal Memory Tracking
NVSDK_NGX_Result NVSDK_NGX_VULKAN_GetParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = CyberFsrContext::instance().AllocateParameter<VkParameterImpl>();	
	return NVSDK_NGX_Result_Success;
}

//TODO External Memory Tracking
NVSDK_NGX_Result NVSDK_NGX_VULKAN_GetCapabilityParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = new VkParameterImpl();
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_VULKAN_AllocateParameters(NVSDK_NGX_Parameter** OutParameters)
{
	*OutParameters = new VkParameterImpl();	
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_VULKAN_DestroyParameters(NVSDK_NGX_Parameter* InParameters)
{
	delete InParameters;
	return NVSDK_NGX_Result_Success;
}

//TODO
NVSDK_NGX_Result NVSDK_NGX_VULKAN_GetScratchBufferSize(NVSDK_NGX_Feature InFeatureId,
	const NVSDK_NGX_Parameter* InParameters, size_t* OutSizeInBytes)
{
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_VULKAN_CreateFeature(VkCommandBuffer InCmdBuffer, NVSDK_NGX_Feature InFeatureID, const NVSDK_NGX_Parameter* InParameters, NVSDK_NGX_Handle** OutHandle)
{
	
	const auto inParams = dynamic_cast<const VkParameterImpl*>(InParameters);
	
	auto deviceContext = CyberFsrContext::instance().CreateContext();
	deviceContext->ViewMatrix = std::make_unique<ViewMatrixHook>();

	*OutHandle = &deviceContext->Handle;

	FfxFsr2ContextDescription initParams = {};
	const size_t scratchBufferSize = ffxFsr2GetScratchMemorySizeVK(VK_PhysicalDevice);
	void* scratchBuffer = malloc(scratchBufferSize);
	
	FfxErrorCode errorCode = ffxFsr2GetInterfaceVK(&initParams.callbacks, scratchBuffer, scratchBufferSize, VK_PhysicalDevice, vkGetDeviceProcAddr);
	FFX_ASSERT(errorCode == FFX_OK);

	initParams.device = ffxGetDeviceVK(VK_Device);
	initParams.maxRenderSize.width = inParams->Width;
	initParams.maxRenderSize.height = inParams->Height;
	initParams.displaySize.width = inParams->OutWidth;
	initParams.displaySize.height = inParams->OutHeight;
	initParams.flags = (inParams->DepthInverted) ? FFX_FSR2_ENABLE_DEPTH_INVERTED : 0
		| (inParams->AutoExposure) ? FFX_FSR2_ENABLE_AUTO_EXPOSURE : 0
		| (inParams->Hdr) ? FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE : 0
		| (inParams->JitterMotion) ? FFX_FSR2_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION : 0
		| (!inParams->LowRes) ? FFX_FSR2_ENABLE_DISPLAY_RESOLUTION_MOTION_VECTORS : 0;

	deviceContext->FsrContext = std::make_unique<FfxFsr2Context>();

	ffxFsr2ContextCreate(deviceContext->FsrContext.get(), &initParams);
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_VULKAN_ReleaseFeature(NVSDK_NGX_Handle* InHandle)
{
	auto deviceContext = CyberFsrContext::instance().Contexts[InHandle->Id];
	FfxErrorCode errorCode = ffxFsr2ContextDestroy(deviceContext->FsrContext.get());
	FFX_ASSERT(errorCode == FFX_OK);
	CyberFsrContext::instance().DeleteContext(InHandle);
	return NVSDK_NGX_Result_Success;
}

NVSDK_NGX_Result NVSDK_NGX_VULKAN_EvaluateFeature(VkCommandBuffer InCmdList, const NVSDK_NGX_Handle* InFeatureHandle, const NVSDK_NGX_Parameter* InParameters, PFN_NVSDK_NGX_ProgressCallback InCallback)
{
	auto deviceContext = CyberFsrContext::instance().Contexts[InFeatureHandle->Id];
	
	const auto inParams = dynamic_cast<const VkParameterImpl*>(InParameters);
	auto* fsrContext = deviceContext->FsrContext.get();
	FfxFsr2DispatchDescription dispatchParameters = {};
	dispatchParameters.commandList = ffxGetCommandListVK(InCmdList);
	dispatchParameters.color = ffxGetTextureResourceVK(fsrContext, inParams->Color->Resource.ImageViewInfo.Image, inParams->Color->Resource.ImageViewInfo.ImageView, inParams->Color->Resource.ImageViewInfo.Width, inParams->Color->Resource.ImageViewInfo.Height, inParams->Color->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_InputColor");
	dispatchParameters.depth = ffxGetTextureResourceVK(fsrContext, inParams->Depth->Resource.ImageViewInfo.Image, inParams->Depth->Resource.ImageViewInfo.ImageView, inParams->Depth->Resource.ImageViewInfo.Width, inParams->Depth->Resource.ImageViewInfo.Height, inParams->Depth->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_InputDepth");
	dispatchParameters.motionVectors = ffxGetTextureResourceVK(fsrContext, inParams->MotionVectors->Resource.ImageViewInfo.Image, inParams->MotionVectors->Resource.ImageViewInfo.ImageView, inParams->MotionVectors->Resource.ImageViewInfo.Width, inParams->MotionVectors->Resource.ImageViewInfo.Height, inParams->MotionVectors->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_InputMotionVectors");
	dispatchParameters.exposure = ffxGetTextureResourceVK(fsrContext, inParams->ExposureTexture->Resource.ImageViewInfo.Image, inParams->ExposureTexture->Resource.ImageViewInfo.ImageView, inParams->ExposureTexture->Resource.ImageViewInfo.Width, inParams->ExposureTexture->Resource.ImageViewInfo.Height, inParams->ExposureTexture->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_InputExposure");
	dispatchParameters.reactive = ffxGetTextureResourceVK(fsrContext, inParams->InputBiasCurrentColorMask->Resource.ImageViewInfo.Image, inParams->InputBiasCurrentColorMask->Resource.ImageViewInfo.ImageView, inParams->InputBiasCurrentColorMask->Resource.ImageViewInfo.Width, inParams->InputBiasCurrentColorMask->Resource.ImageViewInfo.Height, inParams->InputBiasCurrentColorMask->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_InputReactiveMap");
	dispatchParameters.transparencyAndComposition = ffxGetTextureResourceVK(fsrContext, nullptr, nullptr, 1, 1, VK_FORMAT_UNDEFINED, (wchar_t*)L"FSR2_TransparencyAndCompositionMap");
	dispatchParameters.output = ffxGetTextureResourceVK(fsrContext, inParams->Output->Resource.ImageViewInfo.Image, inParams->Output->Resource.ImageViewInfo.ImageView, inParams->Output->Resource.ImageViewInfo.Width, inParams->Output->Resource.ImageViewInfo.Height, inParams->Output->Resource.ImageViewInfo.Format, (wchar_t*)L"FSR2_OutputUpscaledColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);
	
	dispatchParameters.jitterOffset.x = inParams->JitterOffsetX;
	dispatchParameters.jitterOffset.y = inParams->JitterOffsetY;
	dispatchParameters.motionVectorScale.x = (float)inParams->MVScaleX;
	dispatchParameters.motionVectorScale.y = (float)inParams->MVScaleY;

	dispatchParameters.reset = inParams->ResetRender;
	dispatchParameters.enableSharpening = inParams->EnableSharpening;
	dispatchParameters.sharpness = inParams->Sharpness;

	//deltatime hax
	static double lastFrameTime;
	double currentTime = Util::MillisecondsNow();
	double deltaTime = (currentTime - lastFrameTime);
	lastFrameTime = currentTime;

	dispatchParameters.frameTimeDelta = (float)deltaTime;
	dispatchParameters.preExposure = 1.0f;
	dispatchParameters.renderSize.width = inParams->Width;
	dispatchParameters.renderSize.height = inParams->Height;

	//Hax Zone
	dispatchParameters.cameraFar = deviceContext->ViewMatrix->GetFarPlane();
	dispatchParameters.cameraNear = deviceContext->ViewMatrix->GetNearPlane();
	dispatchParameters.cameraFovAngleVertical = DirectX::XMConvertToRadians(deviceContext->ViewMatrix->GetFov());
	FfxErrorCode errorCode = ffxFsr2ContextDispatch(fsrContext, &dispatchParameters);
	FFX_ASSERT(errorCode == FFX_OK);
	return NVSDK_NGX_Result_Success;
}

#pragma endregion

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_DLSS_GetOptimalSettingsCallback(NVSDK_NGX_Parameter* InParams)
{
	if (!IsVK) {
		auto* params = (Dx12ParameterImpl*)InParams;
		params->EvaluateRenderScale();
	}
	else {
		auto* params = (VkParameterImpl*)InParams;
		params->EvaluateRenderScale();
	}
	return NVSDK_NGX_Result_Success;
}

void CyberFsrContext::DeleteParameter(NVSDK_NGX_Parameter* parameter)
{
	auto it = std::find(Parameters.begin(), Parameters.end(), parameter);
	Parameters.erase(it);
}

FeatureContext* CyberFsrContext::CreateContext()
{
	auto dCtx = new FeatureContext();
	dCtx->Handle.Id = rand();
	Contexts[dCtx->Handle.Id] = dCtx;

	return dCtx;
}

void CyberFsrContext::DeleteContext(NVSDK_NGX_Handle* handle)
{
	auto handleId = handle->Id;

	auto it = std::find_if(Contexts.begin(), Contexts.end(),
		[&handleId](const auto& p) { return p.first == handleId; });
	Contexts.erase(it);
}
