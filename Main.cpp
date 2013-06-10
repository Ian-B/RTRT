//--------------------------------------------------------------------------------------
// File: Main.cpp
//
// Empty starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "SGLibResource.h"
#include "ShaderSimple.h"
#include "ShaderComplex.h"
#include <sstream>

using namespace SGLib;

ShaderSimple*	g_pnodeShaderSimple = NULL;
ShaderComplex*	g_pnodeShaderComplex = NULL;
SGRenderer*		g_pRenderer = NULL;
Camera*			g_pnodeCamera = NULL;
Projection*		g_pnodeProj = NULL;
State*			g_pnodeState = NULL;
Transform*		g_pnodeTransPlane = NULL;
Transform*		g_pnodeTransPerson = NULL;
Transform*		g_pnodeTransHouse = NULL;
Transform*		g_pnodeTransHouse2 = NULL;
Transform*		g_pnodeTransTerrain = NULL;
Transform*		g_pnodeTransChevelle = NULL;
Transform*		g_pnodeTransBlueCar = NULL;
Transform*		g_pnodeTransLexus = NULL;
Transform*		g_pnodeTransSpotlight1 = NULL;
Transform*		g_pnodeTransSpotlight2 = NULL;
Transform*		g_pnodeTransTree = NULL;
Geometry*		g_pnodePlane = NULL;
Geometry*		g_pnodeHouse = NULL;
Geometry*		g_pnodeHouse2 = NULL;
Geometry*		g_pnodeTerrain = NULL;
Geometry*		g_pnodeChevelle = NULL;
Geometry*		g_pnodeBlueCar = NULL;
Geometry*		g_pnodeLexus = NULL;
Geometry*		g_pnodeSpotlight1 = NULL;
Geometry*		g_pnodeSpotlight2 = NULL;
Geometry*		g_pnodeTree = NULL;
Articulated*	g_pnodeArt = NULL;
//Transform*		g_pnodeTransPerson = NULL;
Articulated*	g_pnodePerson = NULL;
Articulated*	g_pnodePelvis = NULL;
Articulated*	g_pnodeRUpperLeg = NULL;
Articulated*	g_pnodeLUpperLeg = NULL;
Articulated*	g_pnodeRLowerLeg = NULL;
Articulated*	g_pnodeLLowerLeg = NULL;
Articulated*	g_pnodeLowerBack = NULL;
Articulated*	g_pnodeUpperBack = NULL;
Articulated*	g_pnodeNeck = NULL;
Articulated*	g_pnodeHead = NULL;
Articulated*	g_pnodeShoulders = NULL;
Articulated*	g_pnodeRUpperArm = NULL;
Articulated*	g_pnodeLUpperArm = NULL;
Articulated*	g_pnodeRLowerArm = NULL;
Articulated*	g_pnodeLLowerArm = NULL;
D3DXVECTOR3		g_vecCamPos(32.5f, 68.0f, 10.0f);
D3DXVECTOR3		g_vecCamUp(0.0f, 0.0f, 1.0f);
D3DXVECTOR3		g_vecCamLook(32.5f, 30.0f, 10.0f);
LPD3DXFONT		g_pD3DXFont = NULL;
FLOAT			g_fAnimLength = 0.0f;
FLOAT			g_fPlayerSpeed = 0.5f;
FLOAT			g_fDayNight = -0.8f;			//light level
BOOL			g_bFlag = FALSE;			//day or night
BOOL			g_bWalking = FALSE;
BOOL			g_bFreeCam = FALSE;
BOOL			g_bCamAimUp = FALSE;		// aiming the camera up in Free Cam
BOOL			g_bCamAimDown = FALSE;		// aiming the camera down in Free Cam
BOOL			g_bPlayerForward = FALSE;	// player is moving forward
BOOL			g_bPlayerBackward = FALSE;	// player is moving backward
BOOL			g_bPlayerLeft = FALSE;		// player is strafing left
BOOL			g_bPlayerRight = FALSE;		// player is strafing right
BOOL			g_bPlayerTurnLeft = FALSE;	// player is turning left
BOOL			g_bPlayerTurnRight = FALSE; // player is turning right
BOOL			g_bHelp = FALSE;

// define VKs for alphabetical keystrokes
#define VK_W	0x057
#define VK_A	0x041
#define VK_S	0x053
#define VK_D	0x044
#define VK_Q	0x051
#define VK_E	0x045

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	//turn on AA
	pDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
	return true;
}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	D3DXCreateFont(	pd3dDevice, 14, 0, FW_BOLD, 0, FALSE, 
		            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		            DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), 
		            &g_pD3DXFont );

	if (g_pnodeCamera)
		g_pnodeCamera->OnCreateDevice(pd3dDevice);
	//pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if (g_pD3DXFont)
		g_pD3DXFont->OnResetDevice();
		
	if (g_pnodeCamera)
		g_pnodeCamera->OnResetDevice(DXUTGetD3DDevice());
		
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double dTime, float fElapsedTime, void* pUserContext )
{
	g_pnodeShaderComplex->SetCamPos(g_pnodeCamera->GetPos());

	// set spotlight1 pos
	D3DXVECTOR3 spotlightPos1(g_pnodeTransSpotlight1->GetMatrix()._41,
							  g_pnodeTransSpotlight1->GetMatrix()._42,
							  g_pnodeTransSpotlight1->GetMatrix()._43);
	g_pnodeShaderComplex->SetSpotlightPos1(spotlightPos1);

	// set spotlight1 direction
	D3DXVECTOR3 spotlightDir1(g_pnodeTransSpotlight1->GetMatrix()._31,
						      g_pnodeTransSpotlight1->GetMatrix()._32,
							  g_pnodeTransSpotlight1->GetMatrix()._33);
	g_pnodeShaderComplex->SetSpotlightDir1(spotlightDir1);

	// set spotlight2 pos
	D3DXVECTOR3 spotlightPos2(g_pnodeTransSpotlight2->GetMatrix()._41,
							  g_pnodeTransSpotlight2->GetMatrix()._42,
							  g_pnodeTransSpotlight2->GetMatrix()._43);
	g_pnodeShaderComplex->SetSpotlightPos2(spotlightPos2);

	// set spotlight2 direction
	D3DXVECTOR3 spotlightDir2(g_pnodeTransSpotlight2->GetMatrix()._31,
							  g_pnodeTransSpotlight2->GetMatrix()._32,
							  g_pnodeTransSpotlight2->GetMatrix()._33);
	g_pnodeShaderComplex->SetSpotlightDir2(spotlightDir2);

	g_pRenderer->Update(g_pnodeCamera, fElapsedTime);

	// day and night cycle
	if (g_fDayNight < -9.0)
	{
		g_bFlag = TRUE;
	}
	if (g_fDayNight > 0.3)
	{
		g_bFlag = FALSE;
	}
	if (g_bFlag)
	{
		g_fDayNight += 0.01f;
	}
	else
	{
		g_fDayNight -= 0.01f;
	}
	//change sun ambient
	g_pnodeShaderComplex->SetVarAmbient(g_fDayNight);

	D3DXMATRIX _mPlayer = g_pnodeTransPerson->GetMatrix();

	// Player movement - working
	D3DXMATRIX _mPlayerTransform(_mPlayer);				// player's transform matrix
	D3DXMATRIX _mPlayerTrans;							// player's translation matrix
	D3DXMATRIX _mPlayerRotZ;							// player's rotation matrix
	D3DXVECTOR3 _vecPlayerUp(0.0, 0.0, 1.0);			// player's up vector

	// Player rotation - working
	D3DXVECTOR3 _vecPlayerRot(0.0f, 0.0f, 0.0f);		// player's rotation vector
	if (g_bPlayerTurnLeft)
		_vecPlayerRot.z = -D3DX_PI/60;
	if (g_bPlayerTurnRight)
		_vecPlayerRot.z = D3DX_PI/60;

	D3DXMatrixTranslation(&_mPlayerTrans, -_mPlayer._41, -_mPlayer._42, -_mPlayer._43);
	D3DXMatrixRotationZ(&_mPlayerRotZ, _vecPlayerRot.z);
	D3DXMatrixMultiply(&_mPlayerRotZ, &_mPlayerTrans, &_mPlayerRotZ);
	D3DXMatrixMultiply(&_mPlayerTransform, &_mPlayerTransform, &_mPlayerRotZ);
	D3DXMatrixTranslation(&_mPlayerTrans, _mPlayer._41, _mPlayer._42, _mPlayer._43);
	D3DXMatrixMultiply(&_mPlayerTransform, &_mPlayerTransform, &_mPlayerTrans);

	// Player translation - working
	D3DXVECTOR3 _vecPlayerFacing(	_mPlayer._31,		// player's facing direction vector
									_mPlayer._32,
									_mPlayer._33);

	D3DXVECTOR3 _vecPlayerMove(0.0f, 0.0f, 0.0f);		// player's movement vector

	if (g_bPlayerForward)
	{
		D3DXVECTOR3 _vecTemp;
		D3DXVec3Cross(&_vecTemp, &-_vecPlayerFacing, &_vecPlayerUp);
		_vecPlayerMove += _vecTemp;
	}
	if (g_bPlayerBackward)
	{
		D3DXVECTOR3 _vecTemp;
		D3DXVec3Cross(&_vecTemp, &_vecPlayerFacing, &_vecPlayerUp);
		_vecPlayerMove += _vecTemp;
	}
	if (g_bPlayerLeft)
		_vecPlayerMove += _vecPlayerFacing;
	if (g_bPlayerRight)
		_vecPlayerMove += -_vecPlayerFacing;

	if ((_vecPlayerMove.x == 0) && (_vecPlayerMove.y == 0) && (_vecPlayerMove.z == 0))
	{
		if (g_bWalking)
		{
			// Stops the walking animation
			g_bWalking = FALSE;
			g_pnodePerson->StopAnimationAll();
		}
	}
	else
	{
		if (!g_bWalking)
		{
			// Starts the walking animation
			if (g_pnodePerson->GetCurrAnimation() != L"Walk")
				g_pnodePerson->SetAnimationAll(L"Walk", TRUE);
			else
				g_pnodePerson->ContinueAnimationAll();
			g_bWalking = TRUE;
		}
	}

	D3DXMatrixTranslation(&_mPlayerTrans,	_vecPlayerMove.x * g_fPlayerSpeed,
											_vecPlayerMove.y * g_fPlayerSpeed,
											_vecPlayerMove.z * g_fPlayerSpeed);

	D3DXMatrixMultiply(&_mPlayerTransform, &_mPlayerTransform, &_mPlayerTrans);

	g_pnodeTransPerson->SetMatrix(_mPlayerTransform);

	if (!g_bFreeCam)
	{
		// Chase Cam - almost working
		D3DXVECTOR3 _vecPlayerPos(	_mPlayer._41,
									_mPlayer._42,
									_mPlayer._43 +10);

		D3DXVECTOR3 _vecNewCamPos;
		D3DXVECTOR3 _vecPlayerForward;
		D3DXVec3Cross(&_vecPlayerForward, &-_vecPlayerFacing, &_vecPlayerUp);
		_vecNewCamPos = _vecPlayerPos + (-40*_vecPlayerForward);

		g_pnodeCamera->SetPos(_vecNewCamPos);
		g_pnodeCamera->SetLook(_vecPlayerPos);
	} else {
		// Free Cam aiming up/down - failtastic
		if (g_bCamAimUp)
		{
			D3DXVECTOR3 _vecCamAimUp = g_pnodeCamera->GetLook();
			_vecCamAimUp.z += 1;
			g_pnodeCamera->SetLook(_vecCamAimUp);
		}
		if (g_bCamAimDown)
		{
			D3DXVECTOR3 _vecCamAimDown = g_pnodeCamera->GetLook();
			_vecCamAimDown.z -= 1;
			g_pnodeCamera->SetLook(_vecCamAimDown);
		}
	}
}

//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double dTime, float fElapsedTime, void* pUserContext )
{
	g_pRenderer->Render(g_pnodeCamera);

	std::wstringstream _stringStream;
	RECT font_rect;

	D3DXVECTOR3 currentCamPos = g_pnodeCamera->GetPos();

	//A pre-formatted string showing the current frames per second
	_stringStream	<< "FPS: " << DXUTGetFPS()
					<< "\nCamera Pos: (" << currentCamPos.x << ", " << currentCamPos.y << ", " << currentCamPos.z << ")"
					<< "\nPress F1 to toggle help"
					<< "\nPress F2 to toggle camera";

	if (g_bFreeCam)
	{
		_stringStream << "\nFree Cam";
		if (g_bHelp)
			_stringStream	<< "\nnum8 - Cam Forward"
							<< "\nnum5 - Cam Backward"
							<< "\nnum7 - Cam Strafe Left"
							<< "\nnum9 - Cam Strafe Right"
							<< "\nnum4 - Cam Turn Left"
							<< "\nnum6 - Cam Turn Right";
	} else {
		_stringStream << "\nChase Cam";
		if (g_bHelp)
			_stringStream	<< "\nW - Player Forward"
							<< "\nS - Player Backward"
							<< "\nQ - Player Strafe Left"
							<< "\nE - Player Strafe Right"
							<< "\nA - Player Turn Left"
							<< "\nD - Player Turn Right";
	}

	SetRect(&font_rect, 0, 0, 640, 480);

	g_pD3DXFont->DrawTextW(	NULL,						//pSprite
							_stringStream.str().c_str(),//pString
							-1,							//Count
							&font_rect,					//pRect
							DT_LEFT|DT_NOCLIP,			//Format,
							0xFFFFFFFF);				//Color
}

//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch(uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;

		case WM_KEYDOWN:
		{
			switch(wParam)
			{
				case VK_SPACE:
					g_bCamAimUp = TRUE;
					break;

				case VK_CONTROL:
					g_bCamAimDown = TRUE;
					break;

				case VK_W:
					g_bPlayerForward = TRUE;
					break;

				case VK_S:
					g_bPlayerBackward = TRUE;
					break;

				case VK_Q:
					g_bPlayerLeft = TRUE;
					break;

				case VK_E:
					g_bPlayerRight = TRUE;
					break;

				case VK_A:
					g_bPlayerTurnLeft = TRUE;
					break;

				case VK_D:
					g_bPlayerTurnRight = TRUE;
					break;
			}
			break;
		}

		case WM_KEYUP:
		{
			switch(wParam)
			{
				case VK_ESCAPE:
					DestroyWindow(hWnd);
					return 0;
					break;

				case VK_F1:
					g_bHelp = !g_bHelp;
					break;

				case VK_F2:
					if (!g_bFreeCam)
					{
						g_bFreeCam = TRUE;
						g_pnodeCamera->SetSimpleMovement(true);
					} else {
						g_bFreeCam = FALSE;
						g_pnodeCamera->SetSimpleMovement(false);
					}
					break;

				case VK_SPACE:
					g_bCamAimUp = FALSE;
					break;

				case VK_CONTROL:
					g_bCamAimDown = FALSE;
					break;

				case VK_W:
					g_bPlayerForward = FALSE;
					break;

				case VK_S:
					g_bPlayerBackward = FALSE;
					break;

				case VK_Q:
					g_bPlayerLeft = FALSE;
					break;

				case VK_E:
					g_bPlayerRight = FALSE;
					break;

				case VK_A:
					g_bPlayerTurnLeft = FALSE;
					break;

				case VK_D:
					g_bPlayerTurnRight = FALSE;
					break;
			}
			break;
		}
	}
	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	if (g_pD3DXFont)
		g_pD3DXFont->OnLostDevice();

	if (g_pnodeCamera)
		g_pnodeCamera->OnLostDevice();
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_RELEASE(g_pD3DXFont);

	if (g_pnodeCamera)
		g_pnodeCamera->OnDestroyDevice();
}

void InitalizeGraph()
{
	LPDIRECT3DDEVICE9 pD3DDevice = DXUTGetD3DDevice();
	g_pRenderer = new SGRenderer();

	//Here are the shader nodes to be created later to be inserted.

	g_pnodeShaderSimple = new ShaderSimple(pD3DDevice, L"SimpleEffect.fx");
	g_pnodeShaderComplex = new ShaderComplex(pD3DDevice, L"LightEffect.fx");

	g_pnodeCamera = new Camera(pD3DDevice, g_vecCamPos, g_vecCamUp, g_vecCamLook);
	g_pnodeCamera->SetSimpleMovement(g_bFreeCam);

	D3DXMATRIX oMatrixProj;
	D3DXMatrixPerspectiveFovLH(&oMatrixProj, D3DX_PI * 0.25f, 1.5f, 1.0f, 1000.0f);
	g_pnodeProj = new Projection(pD3DDevice, oMatrixProj);

	D3DXMATRIX matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;
	D3DXMatrixScaling(&matScale, 0.001f, 0.001f, 0.001f);
	D3DXMatrixRotationY(&matRotY, 0.5f);
	D3DXMatrixRotationZ(&matRotZ, -D3DX_PI/4);
	D3DXMatrixMultiply(&matWorld, &matScale, &matRotY);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRotZ);
	g_pnodeTransPlane = new Transform(pD3DDevice, matWorld);

	g_pnodePlane = new Geometry(pD3DDevice, L"plane.x");

	D3DXMatrixScaling(&matScale, 5.0f, 5.0f, 5.0f);
	D3DXMatrixRotationZ(&matRotZ, -D3DX_PI * 0.85f);
	D3DXMatrixTranslation(&matTrans, -45.0f, 0.0f, -10.0f);
	D3DXMatrixMultiply(&matWorld, &matScale, &matRotZ);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransHouse = new Transform(pD3DDevice, matWorld);

	D3DXMatrixTranslation(&matTrans, 2.0f, 50.0f, 0.0f);
	D3DXMatrixScaling(&matScale, 0.2f, 0.2f, 0.2f);
	g_pnodeTransHouse2 = new Transform(pD3DDevice, matTrans);
	
	g_pnodeHouse = new Geometry(pD3DDevice, L"house.x");

	// add terrain object
	g_pnodeTransTerrain = new Transform(pD3DDevice, matWorld);
	//g_pnodeTerrain = new Geometry(pD3DDevice, L"terrain.x");
	g_pnodeTerrain = new Geometry(pD3DDevice, L"terrain_plane.x");

	// add chevelle object
	D3DXMatrixScaling(&matScale, 5.0f, 5.0f, 5.0f);
	D3DXMatrixTranslation(&matTrans, 50.0f, 0.0f, 40.0f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransChevelle = new Transform(pD3DDevice, matWorld);
	g_pnodeChevelle = new Geometry(pD3DDevice, L"chevelle.x");

	// add blue car object
	D3DXMatrixScaling(&matScale, 0.4f, 0.4f, 0.4f);
	D3DXMatrixTranslation(&matTrans, -80.0f, 20.0f, -3.5f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransBlueCar = new Transform(pD3DDevice, matWorld);
	g_pnodeBlueCar = new Geometry(pD3DDevice, L"Car_LowPoly_Blue.x");

	// add lexus object
	D3DXMatrixScaling(&matScale, 0.6f, 0.6f, 0.6f);
	D3DXMatrixTranslation(&matTrans, -35.0f, 20.0f, -5.5f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransLexus = new Transform(pD3DDevice, matWorld);
	g_pnodeLexus = new Geometry(pD3DDevice, L"Lexus.x");

	// add spotlight1
	D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 20.0f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransSpotlight1 = new Transform(pD3DDevice, matWorld);
	g_pnodeSpotlight1 = new Geometry(pD3DDevice, L"torch.x");

	// add spotlight2
	D3DXMatrixTranslation(&matTrans, 0.0f, -100.0f, 0.0f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransSpotlight2 = new Transform(pD3DDevice, matWorld);
	g_pnodeSpotlight2 = new Geometry(pD3DDevice, L"torch.x");

	// add tree1
	//D3DXMatrixRotationX(&matWorld, D3DX_PI);
	D3DXMatrixTranslation(&matTrans, 100.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);
	g_pnodeTransTree = new Transform(pD3DDevice, matWorld);
	g_pnodeTree = new Geometry(pD3DDevice, L"tree.x");

	g_pnodeCamera->SetChild(g_pnodeProj);

	// Simple Shader is now a child of the projection matrix, and the person will be shaded using the simple shader.

	g_pnodeProj->SetChild(g_pnodeShaderComplex);

	D3DXMatrixRotationY(&matRotY, -D3DX_PI/2);
	D3DXMatrixRotationZ(&matRotZ, 0);
	D3DXMatrixTranslation(&matTrans, 32.5f, 28.0f, -1.0f);

	D3DXMatrixMultiply(&matWorld, &matRotY, &matRotZ);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	g_pnodeTransPerson = new Transform(pD3DDevice, matWorld);

	//												Leng, Disp,		Rot Default Min Max,					Twist Default Min Max,			Filename
	g_pnodePerson =		new Articulated(pD3DDevice, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,						0.0f, 0.0f, 0.0f,				NULL);
	g_pnodePelvis =		new Articulated(pD3DDevice, 0.0f, 0.0f,		D3DX_PI, D3DX_PI, D3DX_PI,				0.0f, 0.0f, 0.0f,				NULL);
	
	g_pnodeRUpperLeg =	new Articulated(pD3DDevice, 3.5f, -1.0f,	D3DX_PI/25, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"barney_rightupperleg.x"); 
	g_pnodeLUpperLeg =	new Articulated(pD3DDevice, 3.5f, 1.0f,		D3DX_PI/25, -D3DX_PI/4, D3DX_PI/3,		0.0f, 0.0f, 0.0f,				L"barney_leftupperleg.x");
	g_pnodeRLowerLeg =	new Articulated(pD3DDevice, 0.0f, -0.3f,	-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"barney_rightlowerleg.x");
	g_pnodeLLowerLeg =	new Articulated(pD3DDevice, 0.0f, 0.3f,		-D3DX_PI/13, -D3DX_PI/2, 0.0f,			0.0f, 0.0f, 0.0f,				L"barney_leftlowerleg.x");
	
	g_pnodeLowerBack =	new Articulated(pD3DDevice, 3.0f, 0.0f,		D3DX_PI/25, -D3DX_PI/10, D3DX_PI/10,	0.0f, -D3DX_PI/8, D3DX_PI/8,	L"barney_lowerbody.x");
	g_pnodeUpperBack =	new Articulated(pD3DDevice, 2.5f, 0.0f,		-D3DX_PI/24, -D3DX_PI/2, 0.0f,			0.0f, -D3DX_PI/8, D3DX_PI/8,	L"barney_upperbody.x");
	
	g_pnodeNeck =		new Articulated(pD3DDevice, 0.5f, 0.0f,		0.0f, 0.0f, 0.0f,						0.0f, -D3DX_PI/3, D3DX_PI/3,	NULL);
	g_pnodeHead =		new Articulated(pD3DDevice, 0.0f, 0.0f,		0.0f, -D3DX_PI/3, D3DX_PI/3,			0.0f, 0.0f, 0.0f,				L"barney_head.x");
	g_pnodeShoulders =	new Articulated(pD3DDevice, 0.5f, 0.0f,		D3DX_PI, D3DX_PI, D3DX_PI,				0.0f, 0.0f, 0.0f,				NULL);
	
	g_pnodeRUpperArm =	new Articulated(pD3DDevice, 3.0f, -2.5f,	D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"barney_rightupperarm.x");
	g_pnodeLUpperArm =	new Articulated(pD3DDevice, 3.0f, 2.5f,		D3DX_PI/20, -D3DX_PI/2, D3DX_PI/1.1f,	0.0f, -D3DX_PI/12, D3DX_PI/12,	L"barney_leftupperarm.x");
	g_pnodeRLowerArm =	new Articulated(pD3DDevice, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"barney_rightlowerarm.x");
	g_pnodeLLowerArm =	new Articulated(pD3DDevice, 3.0f, 0.0f,		D3DX_PI/10, 0.0f, D3DX_PI/1.3f,			0.0f, 0.0f, 0.0f,				L"barney_leftlowerarm.x");

	g_pnodePerson->SetDescription(L"Base");
	g_pnodeNeck->SetDescription(L"Neck");
	g_pnodeHead->SetDescription(L"Head");

	g_pnodeTerrain->SetDescription(L"Terrain");
	g_pnodeChevelle->SetDescription(L"Chevelle"); // chevelle car
	g_pnodeBlueCar->SetDescription(L"BlueCar"); // blue car
	g_pnodeBlueCar->SetDescription(L"lexus"); // pickup lexus

	g_pnodeSpotlight1->SetDescription(L"spotlight1"); // spotlight1
	g_pnodeSpotlight1->SetDescription(L"spotlight2"); // spotlight2

	g_pnodeTree->SetDescription(L"Tree"); // tree1

	g_pnodeShaderComplex->SetChild(g_pnodeTransPerson);
	
	g_pnodeTransPerson->SetSibling(g_pnodeTransPlane);
	g_pnodeTransPlane->SetChild(g_pnodePlane);

	g_pnodeTransPlane->SetSibling(g_pnodeTransHouse);
	g_pnodeTransHouse->SetChild(g_pnodeHouse);
	g_pnodeHouse->SetChild(g_pnodeTransHouse2);

	// terrain is sibling of house
	g_pnodeTransHouse->SetSibling(g_pnodeTransTerrain);
	g_pnodeTransTerrain->SetChild(g_pnodeTerrain);

	// chevelle is sibling of terrain
	g_pnodeTransTerrain->SetSibling(g_pnodeTransChevelle);
	g_pnodeTransChevelle->SetChild(g_pnodeChevelle);

	// blue car is sibling of chevelle
	g_pnodeTransChevelle->SetSibling(g_pnodeTransBlueCar);
	g_pnodeTransBlueCar->SetChild(g_pnodeBlueCar);

	// lexus is sibling of blue car
	g_pnodeTransBlueCar->SetSibling(g_pnodeTransLexus);
	g_pnodeTransLexus->SetChild(g_pnodeLexus);

	// spotlight1 is sibling of lexus
	g_pnodeTransLexus->SetSibling(g_pnodeTransSpotlight1);
	g_pnodeTransSpotlight1->SetChild(g_pnodeSpotlight1);

	// spotlight2 is sibling of spotlight1
	g_pnodeTransSpotlight1->SetSibling(g_pnodeTransSpotlight2);
	g_pnodeTransSpotlight2->SetChild(g_pnodeSpotlight2);

	// tree1 is sibling of spotlight2
	g_pnodeTransSpotlight2->SetSibling(g_pnodeTransTree);
	g_pnodeTransTree->SetChild(g_pnodeTree);


	// Now the house has a shader added to it to perform lighting.
	//g_pnodeTransHouse->InsertChild(g_pnodeShaderSimple);
	//g_pnodeTransChevelle->InsertChild(g_pnodeShaderSimple);
	
	g_pnodeTransPerson->SetChild(g_pnodePerson);
	g_pnodePerson->SetChild(g_pnodeLowerBack);
	g_pnodeLowerBack->SetSibling(g_pnodePelvis);
	g_pnodePelvis->SetChild(g_pnodeLUpperLeg);
	g_pnodeLUpperLeg->SetSibling(g_pnodeRUpperLeg);
	g_pnodeRUpperLeg->SetChild(g_pnodeRLowerLeg);
	g_pnodeLUpperLeg->SetChild(g_pnodeLLowerLeg);
	g_pnodeLowerBack->SetChild(g_pnodeUpperBack);
	g_pnodeUpperBack->SetChild(g_pnodeNeck);
	g_pnodeNeck->SetChild(g_pnodeHead);
	g_pnodeNeck->SetSibling(g_pnodeShoulders);
	g_pnodeShoulders->SetChild(g_pnodeRUpperArm);
	g_pnodeRUpperArm->SetSibling(g_pnodeLUpperArm);
	g_pnodeRUpperArm->SetChild(g_pnodeRLowerArm);
	g_pnodeLUpperArm->SetChild(g_pnodeLLowerArm);

	std::vector<TimeStep> vecRotRUA, vecTwistRUA, vecRotLUA, vecTwistLUA;
	
	vecRotLUA.push_back(TimeStep(0.0f, D3DX_PI/20));
	vecRotLUA.push_back(TimeStep(0.5f, D3DX_PI/6));
	vecRotLUA.push_back(TimeStep(1.0f, D3DX_PI/20));
	vecRotLUA.push_back(TimeStep(1.5f, -D3DX_PI/25));
	vecRotLUA.push_back(TimeStep(2.0f, D3DX_PI/20));
	
	vecRotRUA.push_back(TimeStep(0.0f, D3DX_PI/20));
	vecRotRUA.push_back(TimeStep(0.5f, -D3DX_PI/25));
	vecRotRUA.push_back(TimeStep(1.0f, D3DX_PI/20));
	vecRotRUA.push_back(TimeStep(1.5f, D3DX_PI/6));
	vecRotRUA.push_back(TimeStep(2.0f, D3DX_PI/20));

	vecTwistRUA.push_back(TimeStep(0.0f, -D3DX_PI/15));
	vecTwistLUA.push_back(TimeStep(0.0f, D3DX_PI/15));

	std::vector<TimeStep> vecRotRUL, vecRotLUL, vecRotRLL, vecRotLLL, vecRotUB, vecTwist;
	vecTwist.push_back(TimeStep(0.0f, 0.0f));

	vecRotRUL.push_back(TimeStep(0.0f, D3DX_PI/15));
	vecRotRUL.push_back(TimeStep(0.5f, D3DX_PI/6));
	vecRotRUL.push_back(TimeStep(1.0f, D3DX_PI/15));
	vecRotRUL.push_back(TimeStep(1.5f, -D3DX_PI/10));
	vecRotRUL.push_back(TimeStep(2.0f, D3DX_PI/15));

	vecRotLUL.push_back(TimeStep(0.0f, D3DX_PI/15));
	vecRotLUL.push_back(TimeStep(0.5f, -D3DX_PI/10));
	vecRotLUL.push_back(TimeStep(1.0f, D3DX_PI/15));
	vecRotLUL.push_back(TimeStep(1.5f, D3DX_PI/6));
	vecRotLUL.push_back(TimeStep(2.0f, D3DX_PI/15));

	vecRotRLL.push_back(TimeStep(0.0f, -D3DX_PI/13));
	vecRotRLL.push_back(TimeStep(0.5f, -D3DX_PI/50));
	vecRotRLL.push_back(TimeStep(1.0f, -D3DX_PI/13));
	vecRotRLL.push_back(TimeStep(1.5f, -D3DX_PI/6));
	vecRotRLL.push_back(TimeStep(2.0f, -D3DX_PI/13));

	vecRotLLL.push_back(TimeStep(0.0f, -D3DX_PI/13));
	vecRotLLL.push_back(TimeStep(0.5f, -D3DX_PI/6));
	vecRotLLL.push_back(TimeStep(1.0f, -D3DX_PI/13));
	vecRotLLL.push_back(TimeStep(1.5f, -D3DX_PI/50));
	vecRotLLL.push_back(TimeStep(2.0f, -D3DX_PI/13));

	vecRotUB.push_back(TimeStep(0.0f, -D3DX_PI/24));
	vecRotUB.push_back(TimeStep(0.5f, -D3DX_PI/20));
	vecRotUB.push_back(TimeStep(1.0f, -D3DX_PI/24));
	vecRotUB.push_back(TimeStep(1.5f, -D3DX_PI/20));
	vecRotUB.push_back(TimeStep(2.0f, -D3DX_PI/24));

	AnimContainer RUAWalkAnim(vecRotRUA, vecTwistRUA);
	AnimContainer LUAWalkAnim(vecRotLUA, vecTwistLUA);
	AnimContainer RULWalkAnim(vecRotRUL, vecTwist);
	AnimContainer LULWalkAnim(vecRotLUL, vecTwist);
	AnimContainer RLLWalkAnim(vecRotRLL, vecTwist);
	AnimContainer LLLWalkAnim(vecRotLLL, vecTwist);
	AnimContainer UBWalkAnim(vecRotUB, vecTwist);

	g_pnodeRUpperArm->AddAnimation(L"Walk", RUAWalkAnim);
	g_pnodeLUpperArm->AddAnimation(L"Walk", LUAWalkAnim);
	g_pnodeRUpperLeg->AddAnimation(L"Walk", RULWalkAnim);
	g_pnodeLUpperLeg->AddAnimation(L"Walk", LULWalkAnim);
	g_pnodeRLowerLeg->AddAnimation(L"Walk", RLLWalkAnim);
	g_pnodeLLowerLeg->AddAnimation(L"Walk", LLLWalkAnim);
	g_pnodeUpperBack->AddAnimation(L"Walk", UBWalkAnim);
}

void CleanUp()
{
	SAFE_DELETE(g_pRenderer);
	SAFE_DELETE(g_pnodeCamera);
	SAFE_DELETE(g_pnodeProj);
	SAFE_DELETE(g_pnodeState);
	SAFE_DELETE(g_pnodeTransPlane);
	SAFE_DELETE(g_pnodeTransPerson);
	SAFE_DELETE(g_pnodeTransHouse);
	SAFE_DELETE(g_pnodeTransHouse2);
	SAFE_DELETE(g_pnodeTransTerrain);
	SAFE_DELETE(g_pnodeTransChevelle);
	SAFE_DELETE(g_pnodeTransBlueCar);
	SAFE_DELETE(g_pnodeTransLexus);
	SAFE_DELETE(g_pnodeTransSpotlight1);
	SAFE_DELETE(g_pnodeTransSpotlight2);
	SAFE_DELETE(g_pnodeTransTree);
	SAFE_DELETE(g_pnodeArt);
	SAFE_DELETE(g_pnodePlane);
	SAFE_DELETE(g_pnodeHouse);
	SAFE_DELETE(g_pnodeHouse2);
	SAFE_DELETE(g_pnodeTerrain);
	SAFE_DELETE(g_pnodeChevelle);
	SAFE_DELETE(g_pnodeBlueCar);
	SAFE_DELETE(g_pnodeLexus);
	SAFE_DELETE(g_pnodeSpotlight1);
	SAFE_DELETE(g_pnodeSpotlight2);
	SAFE_DELETE(g_pnodeTree);
	SAFE_DELETE(g_pnodeShaderSimple);
	SAFE_DELETE(g_pnodeShaderComplex);
	SAFE_DELETE(g_pnodeTransPerson);
	SAFE_DELETE(g_pnodePerson);
	SAFE_DELETE(g_pnodePelvis);
	SAFE_DELETE(g_pnodeRUpperLeg);
	SAFE_DELETE(g_pnodeLUpperLeg);
	SAFE_DELETE(g_pnodeRLowerLeg);
	SAFE_DELETE(g_pnodeLLowerLeg);
	SAFE_DELETE(g_pnodeLowerBack);
	SAFE_DELETE(g_pnodeUpperBack);
	SAFE_DELETE(g_pnodeNeck);
	SAFE_DELETE(g_pnodeHead);
	SAFE_DELETE(g_pnodeShoulders);
	SAFE_DELETE(g_pnodeRUpperArm);
	SAFE_DELETE(g_pnodeLUpperArm);
	SAFE_DELETE(g_pnodeRLowerArm);
	SAFE_DELETE(g_pnodeLLowerArm);
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( TEXT("INB382 Assignment 2") );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

	InitalizeGraph();

    // Start the render loop
    DXUTMainLoop();

	CleanUp();

    return DXUTGetExitCode();
}