#pragma once
#pragma once
struct VkParameterImpl : NVSDK_NGX_Parameter
{
	unsigned int Width{}, Height{}, OutWidth{}, OutHeight{};
	NVSDK_NGX_PerfQuality_Value PerfQualityValue = NVSDK_NGX_PerfQuality_Value_Balanced;
	bool RTXValue{};
	float Sharpness = 1.0f;
	bool ResetRender{};
	float MVScaleX = 1.0, MVScaleY = 1.0;
	float JitterOffsetX{}, JitterOffsetY{};

	bool DepthInverted, AutoExposure, Hdr, EnableSharpening, JitterMotion, LowRes;

	//external Vulkan Resources
	NVSDK_NGX_Resource_VK* InputBiasCurrentColorMask;
	NVSDK_NGX_Resource_VK* Color;
	NVSDK_NGX_Resource_VK* Depth;
	NVSDK_NGX_Resource_VK* MotionVectors;
	NVSDK_NGX_Resource_VK* Output;
	NVSDK_NGX_Resource_VK* TransparencyMask;
	NVSDK_NGX_Resource_VK* ExposureTexture;

	virtual void Set(const char* InName, unsigned long long InValue) override;
	virtual void Set(const char* InName, float InValue) override;
	virtual void Set(const char* InName, double InValue) override;
	virtual void Set(const char* InName, unsigned int InValue) override;
	virtual void Set(const char* InName, int InValue) override;
	virtual void Set(const char* InName, ID3D11Resource* InValue) override;
	virtual void Set(const char* InName, ID3D12Resource* InValue) override;
	virtual void Set(const char* InName, void* InValue) override;
	virtual NVSDK_NGX_Result Get(const char* InName, unsigned long long* OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, float* OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, double* OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, unsigned int* OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, int* OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, ID3D11Resource** OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, ID3D12Resource** OutValue) const override;
	virtual NVSDK_NGX_Result Get(const char* InName, void** OutValue) const override;
	virtual void Reset() override;

	void EvaluateRenderScale();
};
