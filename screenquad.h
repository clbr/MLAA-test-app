class ScreenQuad {
    public:
      ScreenQuad(IVideoDriver *xy)
      {
	vd = xy;

	const SColor white(255, 255, 255, 255);
	vertices[0].Pos = vector3df(-1, 1, 0);
	vertices[0].TCoords = vector2df(0, 0);
	vertices[0].Color = white;
	vertices[1].Pos = vector3df(1, 1, 0);
	vertices[1].TCoords = vector2df(1, 0);
	vertices[1].Color = white;
	vertices[2].Pos = vector3df(1, -1, 0);
	vertices[2].TCoords = vector2df(1, 1);
	vertices[2].Color = white;
	vertices[3].Pos = vector3df(-1, -1, 0);
	vertices[3].TCoords = vector2df(0, 1);
	vertices[3].Color = white;
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	mat.BackfaceCulling = false;
	mat.Lighting = false;
	mat.ZBuffer = video::ECFN_ALWAYS;
	mat.ZWriteEnable = false;
	for(u32 c = 0; c < MATERIAL_MAX_TEXTURES; c++)
	{
	   mat.TextureLayer[c].TextureWrapU = video::ETC_CLAMP;
	   mat.TextureLayer[c].TextureWrapV = video::ETC_CLAMP;
	}
      }

      SMaterial& GetMaterial() { return mat; }

      //Set the texture to render with the quad
      void SetTexture(ITexture* __restrict tex, u32 layer = 0)
      {
	mat.TextureLayer[layer].Texture = tex;
      }

      ITexture* GetTexture(u32 layer = 0) { return mat.TextureLayer[layer].Texture; }

      void SetMaterialType(E_MATERIAL_TYPE mt) { mat.MaterialType = mt; }

      void Render(bool setRTToFrameBuff = true)
      {
	if(setRTToFrameBuff)
	   vd->setRenderTarget(video::ERT_FRAME_BUFFER);
	vd->setMaterial(mat);
	vd->setTransform(ETS_WORLD, core::IdentityMatrix);
	vd->setTransform(ETS_VIEW, core::IdentityMatrix);
	vd->setTransform(ETS_PROJECTION, core::IdentityMatrix);
	vd->drawIndexedTriangleList(vertices, 4, indices, 2);
      }

      void Render(ITexture* rt)
      {
	vd->setRenderTarget(rt);
	vd->setMaterial(mat);
	vd->setTransform(ETS_WORLD, core::IdentityMatrix);
	vd->setTransform(ETS_VIEW, core::IdentityMatrix);
	vd->setTransform(ETS_PROJECTION, core::IdentityMatrix);
	vd->drawIndexedTriangleList(vertices, 4, indices, 2);
      }

   protected:
	S3DVertex vertices[4];
	u16 indices[6];
	SMaterial mat;

	IVideoDriver* vd;
};
