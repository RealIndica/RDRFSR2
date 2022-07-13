#include "pch.h"
#include "Config.h"
#include "Util.h"
#include "NvParameter.h"
#include "CyberFsr.h"

void NvParameter::Set(const char* InName, unsigned long long InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvULL);
}

void NvParameter::Set(const char* InName, float InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvFloat);
}

void NvParameter::Set(const char* InName, double InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvDouble);
}

void NvParameter::Set(const char* InName, unsigned int InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvUInt);
}

void NvParameter::Set(const char* InName, int InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvInt);
}

void NvParameter::Set(const char* InName, ID3D11Resource* InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvD3D11Resource);
}

void NvParameter::Set(const char* InName, ID3D12Resource* InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvD3D12Resource);
}

void NvParameter::Set(const char* InName, void* InValue)
{
	auto value = (unsigned long long*) & InValue;
	Set_Internal(InName, *value, NvVoidPtr);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, unsigned long long* OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvULL);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, float* OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvFloat);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, double* OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvDouble);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, unsigned int* OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvUInt);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, int* OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvInt);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, ID3D11Resource** OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvD3D11Resource);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, ID3D12Resource** OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvD3D12Resource);
}

NVSDK_NGX_Result NvParameter::Get(const char* InName, void** OutValue) const
{
	return Get_Internal(InName, (unsigned long long*)OutValue, NvVoidPtr);
}

void NvParameter::Reset()
{
}

void NvParameter::Set_Internal(const char* InName, unsigned long long InValue, NvParameterType ParameterType)
{
	auto inValueFloat = (float*)&InValue;
	auto inValueInt = (int*)&InValue;
	auto inValueDouble = (double*)&InValue;
	auto inValueUInt = (unsigned int*)&InValue;
	//Includes DirectX Resources
	auto inValuePtr = (void*)InValue;

	switch (Util::NvParameterToEnum(InName))
	{
	case Util::NvParameter::MV_Scale_X:
		MVScaleX = *inValueFloat;
		break;
	case Util::NvParameter::MV_Scale_Y:
		MVScaleY = *inValueFloat;
		break;
	case Util::NvParameter::Jitter_Offset_X:
		JitterOffsetX = *inValueFloat;
		break;
	case Util::NvParameter::Jitter_Offset_Y:
		JitterOffsetY = *inValueFloat;
		break;
	case Util::NvParameter::Sharpness:
		Sharpness = *inValueFloat;
		break;
	case Util::NvParameter::Width:
		Width = *inValueInt;
		break;
	case Util::NvParameter::Height:
		Height = *inValueInt;
		break;
	case Util::NvParameter::DLSS_Render_Subrect_Dimensions_Width:
		Width = *inValueInt;
		break;
	case Util::NvParameter::DLSS_Render_Subrect_Dimensions_Height:
		Height = *inValueInt;
		break;
	case Util::NvParameter::PerfQualityValue:
		PerfQualityValue = static_cast<NVSDK_NGX_PerfQuality_Value>(*inValueInt);
		break;
	case Util::NvParameter::RTXValue:
		RTXValue = *inValueInt;
		break;
	case Util::NvParameter::Reset:
		ResetRender = *inValueInt;
		break;
	case Util::NvParameter::OutWidth:
		OutWidth = *inValueInt;
		break;
	case Util::NvParameter::OutHeight:
		OutHeight = *inValueInt;
		break;
	case Util::NvParameter::DLSS_Feature_Create_Flags:
		Hdr = *inValueInt & NVSDK_NGX_DLSS_Feature_Flags_IsHDR;
		EnableSharpening = *inValueInt & NVSDK_NGX_DLSS_Feature_Flags_DoSharpening;
		DepthInverted = *inValueInt & NVSDK_NGX_DLSS_Feature_Flags_DepthInverted;
		JitterMotion = *inValueInt & NVSDK_NGX_DLSS_Feature_Flags_MVJittered;
		LowRes = *inValueInt & NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;
		break;
	case Util::NvParameter::DLSS_Input_Bias_Current_Color_Mask:
		InputBiasCurrentColorMask = inValuePtr;
		if (InputBiasCurrentColorMask && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)InputBiasCurrentColorMask)->SetName(L"Color");
		break;
	case Util::NvParameter::Color:
		Color = inValuePtr;
		if (Color && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)Color)->SetName(L"Color");
		break;
	case Util::NvParameter::Depth:
		Depth = inValuePtr;
		if (Depth && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)Depth)->SetName(L"Depth");
		break;
	case Util::NvParameter::MotionVectors:
		MotionVectors = inValuePtr;
		if (MotionVectors && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)MotionVectors)->SetName(L"MotionVectors");
		break;
	case Util::NvParameter::Output:
		Output = inValuePtr;
		if (Output && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)Output)->SetName(L"Output");
		break;
	case Util::NvParameter::TransparencyMask:
		TransparencyMask = inValuePtr;
		if (TransparencyMask && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)TransparencyMask)->SetName(L"TransparencyMask");
		break;
	case Util::NvParameter::ExposureTexture:
		ExposureTexture = inValuePtr;
		if (ExposureTexture && ParameterType == NvParameterType::NvD3D12Resource)
			((ID3D12Resource*)ExposureTexture)->SetName(L"ExposureTexture");
		break;
	}
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_DLSS_GetOptimalSettingsCallback(NVSDK_NGX_Parameter* InParams);

NVSDK_NGX_Result NvParameter::Get_Internal(const char* InName, unsigned long long* OutValue, NvParameterType ParameterType) const
{
	auto outValueFloat = (float*)OutValue;
	auto outValueInt = (int*)OutValue;
	auto outValueDouble = (double*)OutValue;
	auto outValueUInt = (unsigned int*)OutValue;
	//Includes DirectX Resources
	auto outValuePtr = (void**)OutValue;

	switch (Util::NvParameterToEnum(InName))
	{
	case Util::NvParameter::Sharpness:
		*outValueFloat = Sharpness;
		break;
	case Util::NvParameter::SuperSampling_Available:
		*outValueInt = 1;
		break;
	case Util::NvParameter::SuperSampling_FeatureInitResult:
		*outValueInt = 1;
		break;
	case Util::NvParameter::SuperSampling_NeedsUpdatedDriver:
		*outValueInt = FALSE;
		break;
	case Util::NvParameter::SuperSampling_MinDriverVersionMinor:
	case Util::NvParameter::SuperSampling_MinDriverVersionMajor:
		*outValueInt = 0;
		break;
	case Util::NvParameter::DLSS_Render_Subrect_Dimensions_Width:
		*outValueInt = Width;
		break;
	case Util::NvParameter::DLSS_Render_Subrect_Dimensions_Height:
		*outValueInt = Height;
		break;
	case Util::NvParameter::OutWidth:
		*outValueInt = OutWidth;
		break;
	case Util::NvParameter::OutHeight:
		*outValueInt = OutHeight;
		break;
	case Util::NvParameter::DLSS_Get_Dynamic_Max_Render_Width:
		*outValueInt = OutWidth;
		break;
	case Util::NvParameter::DLSS_Get_Dynamic_Max_Render_Height:
		*outValueInt = OutHeight;
		break;
	case Util::NvParameter::DLSS_Get_Dynamic_Min_Render_Width:
		*outValueInt = OutWidth;
		break;
	case Util::NvParameter::DLSS_Get_Dynamic_Min_Render_Height:
		*outValueInt = OutHeight;
		break;
	case Util::NvParameter::DLSSOptimalSettingsCallback:
		*outValuePtr = NVSDK_NGX_DLSS_GetOptimalSettingsCallback;
		break;
	default:
		return NVSDK_NGX_Result_Fail;
	}

	return NVSDK_NGX_Result_Success;
}

void NvParameter::EvaluateRenderScale()
{
	FfxFsr2QualityMode fsrQualityMode;

	auto config = CyberFsrContext::instance()->MyConfig;

	if (config->UpscaleRatioOverrideEnabled.has_value() && config->UpscaleRatioOverrideEnabled && config->UpscaleRatioOverrideValue.has_value())
	{
		auto ratio = config->UpscaleRatioOverrideValue.value();
		OutHeight = (uint32_t)((float)Height / ratio);
		OutWidth = (uint32_t)((float)Width / ratio);

		return;
	}

	switch (PerfQualityValue)
	{
	case NVSDK_NGX_PerfQuality_Value_MaxPerf:
		fsrQualityMode = FFX_FSR2_QUALITY_MODE_PERFORMANCE;
		break;
	case NVSDK_NGX_PerfQuality_Value_Balanced:
		fsrQualityMode = FFX_FSR2_QUALITY_MODE_BALANCED;
		break;
	case NVSDK_NGX_PerfQuality_Value_MaxQuality:
		fsrQualityMode = FFX_FSR2_QUALITY_MODE_QUALITY;
		break;
	case NVSDK_NGX_PerfQuality_Value_UltraPerformance:
		fsrQualityMode = FFX_FSR2_QUALITY_MODE_ULTRA_PERFORMANCE;
		break;
	case NVSDK_NGX_PerfQuality_Value_UltraQuality:
		//Not defined by AMD
		OutHeight = Height;
		OutWidth = Width;
		return;
	}

	ffxFsr2GetRenderResolutionFromQualityMode(&OutWidth, &OutHeight, Width, Height, fsrQualityMode);
}

NVSDK_NGX_Result NVSDK_CONV NVSDK_NGX_DLSS_GetOptimalSettingsCallback(NVSDK_NGX_Parameter* InParams)
{
	auto* params = (NvParameter*)InParams;
	params->EvaluateRenderScale();
	return NVSDK_NGX_Result_Success;
}