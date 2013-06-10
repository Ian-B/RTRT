#include "Shader.h"

using SGLib::Shader;

class ShaderSimple : public Shader
{
public:
	ShaderSimple(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName) : Shader(a_pD3DDevice, a_sFileName)
	{
		if (m_pEffect)
			m_pEffect->SetTechnique("SpotLight");
	}
	~ShaderSimple(){}

	void RenderGeometry(SGLib::Geometry* a_pGeoNode)
	{
		if (!a_pGeoNode && m_pEffect)
			return;

		HRESULT hr;
		UINT unPasses;
		D3DXMATRIX oMatWorldViewProj, oMatWorld, oMatView, oMatProj, oMatWorldIT;

		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorldViewProj, &oMatProj);

		D3DXMatrixInverse(&oMatWorldIT, NULL, &oMatWorld);
		D3DXMatrixTranspose(&oMatWorldIT, &oMatWorldIT);

		V(m_pEffect->SetMatrix("g_matWorldViewProjection", &oMatWorldViewProj))
		V(m_pEffect->SetMatrix("g_matWorld", &oMatWorld))
		V(m_pEffect->SetMatrix("g_matWorldInverseTranspose", &oMatWorldIT))

		//------------------
		//	shader config
		//------------------

		//Light
		D3DXVECTOR3 g_vecLightPosition(0.0f, 1000.0f, 0.0f); //fix this
		D3DXVECTOR3 g_vecLightDirection(0.0f, 1.0f, 0.0f); //fix this

		//Angles
		FLOAT g_fTheta = 0.5235988f; //inner circle 30
		FLOAT g_fPhi = 0.87266463f; //outer circle 60

		//-----------------
		//	set parameters

		V(m_pEffect->SetValue("g_vecLightDirection", g_vecLightDirection, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetValue("g_vecLightPosition", g_vecLightPosition, sizeof(D3DXVECTOR3)))
		V(m_pEffect->SetFloat("g_fTheta", g_fTheta))
		V(m_pEffect->SetFloat("g_fPhi", g_fPhi))

		//technique passes
		V(m_pEffect->Begin(&unPasses, NULL))

		for (UINT i = 0; i < unPasses; ++i)
		{
			V(m_pEffect->BeginPass(i))

			a_pGeoNode->Render();

			V(m_pEffect->EndPass())
		}

		V(m_pEffect->End())
	}
};
