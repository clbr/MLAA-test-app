class blendcb : public IShaderConstantSetCallBack {
	public:
	virtual void OnSetConstants(IMaterialRendererServices* services, s32) {
		int a = 0;
		services->setPixelShaderConstant("edgesMapL", (const float*)&a, 1);
		a = 1;
		services->setPixelShaderConstant("edgesMap", (const float*)&a, 1);
		a = 2;
		services->setPixelShaderConstant("areaMap", (const float*)&a, 1);
	}
};

class neighcb : public IShaderConstantSetCallBack {
	public:
	virtual void OnSetConstants(IMaterialRendererServices* services, s32) {
		int a = 0;
		services->setPixelShaderConstant("blendMap", (const float*)&a, 1);
		a = 1;
		services->setPixelShaderConstant("colorMap", (const float*)&a, 1);
	}
};
