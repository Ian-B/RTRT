#include "Shader.h"

using SGLib::Shader;

class ShaderComplex : public Shader
{
	LPDIRECT3DTEXTURE9 g_pTextureNorm; //chev
	LPDIRECT3DTEXTURE9 g_pTextureNormGrass; //grass
	LPDIRECT3DTEXTURE9 g_pTextureNormLexus; //lexus
	LPDIRECT3DTEXTURE9 g_pTextureNormBcar; //BlueCar
	LPDIRECT3DTEXTURE9 g_pTextureDisp; // displacement texture
	LPDIRECT3DTEXTURE9 g_pTexture; // texture

	// Shadow map
	LPDIRECT3DTEXTURE9 g_pTextureShadowMap;
	LPDIRECT3DSURFACE9 g_pSurfaceShadowMap;
	LPDIRECT3DSURFACE9 g_pSurfaceShadowDS;

	INT	BILLBOARDRANGEX;		// width of billboarding samples
	INT	BILLBOARDRANGEY;		// depth of billboarding samples	
	FLOAT BILLBOARDSPACING;	// spacing between billboards

	D3DXHANDLE g_handleDispAmount;

public:
	ShaderComplex(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName) : Shader(a_pD3DDevice, a_sFileName)
	{
		//if (m_pEffect)
		//	m_pEffect->SetTechnique("PhongNormalTerrain");

		D3DXCreateTextureFromFile(a_pD3DDevice, L"Final.jpg", &g_pTexture);

		D3DXCreateTextureFromFile(a_pD3DDevice, L"Chevelle_normal_map.bmp", &g_pTextureNorm); //chev
		D3DXCreateTextureFromFile(a_pD3DDevice, L"Final_normal_map.png", &g_pTextureNormGrass); //grass
		D3DXCreateTextureFromFile(a_pD3DDevice, L"lexus_texture_normal_map.bmp", &g_pTextureNormLexus); //lexus
		D3DXCreateTextureFromFile(a_pD3DDevice, L"Car_Blue_Body_normal_map.bmp", &g_pTextureNormBcar); //blue car

		D3DXCreateTextureFromFileEx(a_pD3DDevice, L"disp_terrain.bmp", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_R32F, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &g_pTextureDisp);

		// create texture to store depth
		D3DXCreateTexture(a_pD3DDevice, 512, 512, 1, D3DUSAGE_RENDERTARGET, 
			D3DFMT_R32F, D3DPOOL_DEFAULT, &g_pTextureShadowMap);

		// create depth surface used when rendering the shadow map
		a_pD3DDevice->CreateDepthStencilSurface(512, 512, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pSurfaceShadowDS, NULL);
		
	}
	~ShaderComplex()
	{
		SAFE_RELEASE(g_pTextureNorm);
		SAFE_RELEASE(g_pTextureNormGrass);
		SAFE_RELEASE(g_pTextureNormLexus);
		SAFE_RELEASE(g_pTextureNormBcar);
		SAFE_RELEASE(g_pTextureDisp);
		SAFE_RELEASE(g_pTexture);

		SAFE_RELEASE(g_pTextureShadowMap);
		//SAFE_RELEASE(g_pSurfaceShadowMap);
		SAFE_RELEASE(g_pSurfaceShadowDS);
	}

	void SetCamPos(D3DXVECTOR3 a_vecCamPos)
	{
		if (m_pEffect)
			m_pEffect->SetValue("g_vecCameraPos", a_vecCamPos, sizeof(D3DXVECTOR3));
	}

	void SetVarAmbient(FLOAT g_fAmbients)
	{
		if (m_pEffect)
			m_pEffect->SetFloat("g_fAmbients", g_fAmbients);
	}

	void SetSpotlightPos1(D3DXVECTOR3 a_vecSpotlightPos)
	{
		if (m_pEffect)
			m_pEffect->SetValue("g_vecSpotlightPos1", a_vecSpotlightPos, sizeof(D3DXVECTOR3));
	}

	void SetSpotlightDir1(D3DXVECTOR3 a_vecSpotlightDir)
	{
		if (m_pEffect)
			m_pEffect->SetValue("g_vecSpotlightDir1", a_vecSpotlightDir, sizeof(D3DXVECTOR3));
	}

	void SetSpotlightPos2(D3DXVECTOR3 a_vecSpotlightPos)
	{
		if (m_pEffect)
			m_pEffect->SetValue("g_vecSpotlightPos2", a_vecSpotlightPos, sizeof(D3DXVECTOR3));
	}

	void SetSpotlightDir2(D3DXVECTOR3 a_vecSpotlightDir)
	{
		if (m_pEffect)
			m_pEffect->SetValue("g_vecSpotlightDir2", a_vecSpotlightDir, sizeof(D3DXVECTOR3));
	}

	void RenderGeometry(SGLib::Geometry* a_pGeoNode)
	{
		if (!a_pGeoNode && m_pEffect)
			return;

		HRESULT hr;
		UINT unPasses;
		D3DXMATRIX oMatWorldViewProj, oMatWorld, oMatView, oMatProj, oMatWorldIT, oMatLightWVP;

		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorldViewProj, &oMatProj);

		D3DXMatrixInverse(&oMatWorldIT, NULL, &oMatWorld);
		D3DXMatrixTranspose(&oMatWorldIT, &oMatWorldIT);
	

		//D3DXVECTOR3 g_vecLightDirection(0.5f, 0.3f, 0.6f);

		//light stuff
		D3DXMATRIX lightView, lightProj, g_matLightVP;

		D3DXVECTOR3 lightLook(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 lightUp(0.0f, 0.0f, 1.0f);

		// calculate new light position
		D3DXVECTOR3 g_vecLightPos;
		g_vecLightPos = D3DXVECTOR3(-136.0f, 5.0f, 500.0f);

		// calculate light's view matrix
		D3DXMatrixLookAtLH(&lightView, &g_vecLightPos, &lightLook, &lightUp);
		
		// calculate light's projection matrix
		D3DXMatrixPerspectiveFovLH(&lightProj, D3DXToRadian(110.0f), 1.0f, 1.0f, 800.0f);

		// calculate light's view-projection matrix
		D3DXMatrixMultiply(&g_matLightVP, &lightView, &lightProj);

		D3DXVECTOR3	g_vecLightDir(1.0f, 1.0f, -1.0f);
		//D3DXVECTOR3	g_vecLightDir(0.5f, 0.3f, 0.6f);
		g_vecLightDir = g_vecLightPos - lightLook;
		D3DXVec3Normalize(&g_vecLightDir, &g_vecLightDir);

		// calculate light world-view-projection matrix and set it
		D3DXMatrixMultiply(&oMatLightWVP, &oMatWorld, &g_matLightVP);
		V(m_pEffect->SetMatrix("g_matLightWVP", &oMatLightWVP))

		//----------------------------------
		
		V(m_pEffect->SetMatrix("g_matWorldViewProjection", &oMatWorldViewProj))
		V(m_pEffect->SetMatrix("g_matWorld", &oMatWorld))
		V(m_pEffect->SetMatrix("g_matWorldInverseTranspose", &oMatWorldIT))
		V(m_pEffect->SetMatrix("g_matView", &oMatView))

		//------------------
		//	shader config
		//------------------

		//tree layout config
		BILLBOARDRANGEX = 5;
		BILLBOARDRANGEY = 1;
		BILLBOARDSPACING = 7.0f;

		// material components
		D3DXVECTOR4 g_vecMaterialDiffuse(1.0f, 0.5f, 0.0f, 1.0f);
		D3DXVECTOR4 g_vecMaterialAmbient(0.9f, 0.2f, 0.0f, 1.0f);
		D3DXVECTOR4 g_vecMaterialSpecular(0.2f, 0.2f, 0.2f, 1.0f);

		// light components
		D3DXVECTOR4 g_vecLightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
		D3DXVECTOR4 g_vecLightAmbient(0.3f, 0.4f, 0.3f, 1.0f);
		D3DXVECTOR4 g_vecLightSpecular(0.2f, 0.2f, 0.2f, 1.0f);

		//light specular power
		FLOAT g_fSpecPower = 8.0f;

		//displacement 
		//FLOAT g_fDispValue = 1.0f;

		//spotlights
		D3DXVECTOR3 g_vecAttenuation012(0.0f, 0.0f, 1.0f);

		//spotlight1
		FLOAT g_fSpotlightPower1 = 3.0f;
		D3DXVECTOR3 g_vecSpotlightPos1(10.0f, 70.0f, 20.0f);
		D3DXVECTOR3 g_vecSpotlightDirection1(0.0f, 0.0f, -1.0f);
		D3DXVECTOR3 g_vecSpotlightColour1(1.0f, 1.0f, 1.0f);

		//spotlight2
		FLOAT g_fSpotlightPower2 = 3.0f;
		D3DXVECTOR3 g_vecSpotlightPos2(10.0f, 70.0f, 20.0f);
		D3DXVECTOR3 g_vecSpotlightDirection2(0.0f, 0.0f, -1.0f);
		D3DXVECTOR3 g_vecSpotlightColour2(1.0f, 1.0f, 1.0f);

		//-----------------
		//	set parameters
		V(m_pEffect->SetVector("g_vecMaterialDiffuse", &g_vecMaterialDiffuse))
		V(m_pEffect->SetVector("g_vecMaterialAmbient", &g_vecMaterialAmbient))
		V(m_pEffect->SetVector("g_vecMaterialSpecular", &g_vecMaterialSpecular))
		V(m_pEffect->SetVector("g_vecLightDiffuse", &g_vecLightDiffuse))
		V(m_pEffect->SetVector("g_vecLightAmbient", &g_vecLightAmbient))
		V(m_pEffect->SetVector("g_vecLightSpecular", &g_vecLightSpecular))

		V(m_pEffect->SetValue("g_vecLightDirection", &g_vecLightDir, sizeof(D3DXVECTOR3)))
		
		V(m_pEffect->SetFloat("g_fSpecPower", g_fSpecPower))
		V(m_pEffect->SetValue("g_vecAttenuation012", &g_vecAttenuation012, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetFloat("g_fSpotlightPower1", g_fSpotlightPower1))
		//V(m_pEffect->SetValue("g_vecSpotlightPos1", &g_vecSpotlightPos1, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetValue("g_vecSpotlightDirection1", &g_vecSpotlightDirection1, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetValue("g_vecSpotlightColour1", &g_vecSpotlightColour1, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetFloat("g_fSpotlightPower2", g_fSpotlightPower2))
		//V(m_pEffect->SetValue("g_vecSpotlightPos2", &g_vecSpotlightPos2, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetValue("g_vecSpotlightDirection2", &g_vecSpotlightDirection2, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetValue("g_vecSpotlightColour2", &g_vecSpotlightColour2, sizeof(D3DXVECTOR3)))
		
		// shadow map
		LPDIRECT3DSURFACE9 pSurfaceOld = NULL;
		LPDIRECT3DSURFACE9 pSurfaceTexture = NULL;
		LPDIRECT3DSURFACE9 pSurfaceOldDS = NULL;

		// setup rendering to shadow map
		V(m_pD3DDevice->GetRenderTarget(0, &pSurfaceOld))
		V(g_pTextureShadowMap->GetSurfaceLevel(0, &pSurfaceTexture))
		V(m_pD3DDevice->SetRenderTarget(0, pSurfaceTexture))
		V(m_pD3DDevice->GetDepthStencilSurface(&pSurfaceOldDS))
		V(m_pD3DDevice->SetDepthStencilSurface(g_pSurfaceShadowDS))

		// clear z and back buffers
		V(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0, 0))

		V(m_pEffect->SetTechnique("GenerateShadowMapTech"))

		// stores number of passes required for selected technique
		V(m_pEffect->Begin(&unPasses, NULL))

		for (UINT i = 0; i < unPasses; ++i)
		{
			V(m_pEffect->BeginPass(i))

			a_pGeoNode->Render();

			V(m_pEffect->EndPass())
		}

		V(m_pEffect->End())

		// set rendering states back to normal rendering
		V(m_pD3DDevice->SetRenderTarget(0, pSurfaceOld))
		V(m_pD3DDevice->SetDepthStencilSurface(pSurfaceOldDS))
		
		//V(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(10,10,10), 1.0, 0))

		//V(m_pEffect->SetTechnique("PhongNormalTerrain")) 
		//V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))

		//change technique based on node
		if (a_pGeoNode->GetDescription() == L"Terrain")
		{
			V(m_pEffect->SetTexture("g_texture", g_pTexture))
			V(m_pEffect->SetTexture("g_normalMap", g_pTextureNormGrass))
			V(m_pEffect->SetTexture("g_textureDisp", g_pTextureDisp))	
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("PhongNormalTerrain");
		}
		else if (a_pGeoNode->GetDescription() == L"Chevelle")
		{
			V(m_pEffect->SetTexture("g_textureDisp", NULL))
			V(m_pEffect->SetTexture("g_normalMap", g_pTextureNorm))
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("PhongNormalTerrain");
		}
		else if (a_pGeoNode->GetDescription() == L"lexus")
		{
			V(m_pEffect->SetTexture("g_textureDisp", NULL))
			V(m_pEffect->SetTexture("g_normalMap", g_pTextureNormLexus))
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("PhongNormalTerrain");
		}
		else if (a_pGeoNode->GetDescription() == L"BlueCar")
		{
			V(m_pEffect->SetTexture("g_textureDisp", NULL))
			V(m_pEffect->SetTexture("g_normalMap", g_pTextureNormBcar))
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("PhongNormalTerrain");		
		}
		else if (a_pGeoNode->GetDescription() == L"Tree")
		{			
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("BillboardingTech");
		}
		else //default technique
		{
			V(m_pEffect->SetTexture("g_textureDisp", NULL))
			V(m_pEffect->SetTexture("g_normalMap", NULL))
			V(m_pEffect->SetTexture("g_texShadowMap", g_pTextureShadowMap))
			m_pEffect->SetTechnique("PhongNormalTerrain");
		}

		//technique passes
		if (a_pGeoNode->GetDescription() == L"Tree")
		{
			// draw mesh subset
			for (int i = 0; i < BILLBOARDRANGEX; ++i)	// width
			{
				for (int j = 0; j < BILLBOARDRANGEY; ++j)	// depth
				{
					D3DXMATRIX matTrans;
					D3DXMatrixTranslation(&matTrans, i * BILLBOARDSPACING, j * BILLBOARDSPACING, 0.0f);
					V(m_pEffect->SetMatrix("g_matBillboardTrans", &matTrans))

					// render trees
					V(m_pEffect->Begin(&unPasses, NULL))

					for (UINT i = 0; i < unPasses; ++i)
					{
						V(m_pEffect->BeginPass(i))

						a_pGeoNode->Render();

						V(m_pEffect->EndPass())
					}

					V(m_pEffect->End())
				}
			}
		} else {
		
		V(m_pEffect->Begin(&unPasses, NULL))

			for (UINT i = 0; i < unPasses; ++i)
			{
				V(m_pEffect->BeginPass(i))

				a_pGeoNode->Render();

				V(m_pEffect->EndPass())
			}

			V(m_pEffect->End())
		}
		SAFE_RELEASE(pSurfaceTexture);
		SAFE_RELEASE(pSurfaceOld);
		SAFE_RELEASE(pSurfaceOldDS);		
	}
};
