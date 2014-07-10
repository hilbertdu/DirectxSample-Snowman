#pragma once

#include "stdafx.h"
#include "Camera.h"
#include "SkyBox.h"

#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib,"d3dx9.lib")

class CD3DUtility
{
public:
	CD3DUtility(void);
	~CD3DUtility(void);

	bool InitD3D(HWND hwnd);
	void SetHW(UINT width, UINT height, bool windowed);
	void GetRect(RECT* rect);
	void SetDeviceType(D3DDEVTYPE deviceType);
	IDirect3DDevice9* GetDevicePoint();

	void SetViewTransform(float x, float y, float z);
	void SetProjectionTransform(float fovy, float zn, float zf);
	
	void SetWorldTransform();
	void SetWorldTransform(D3DXMATRIX* matrix);
	void ResetTransformMatrix();
	void MutliTransformMatrix(D3DXMATRIX* matrix);

	// 平移变换
	void SetTranslation(float x, float y, float z);
	void SetTranslation(D3DXVECTOR3 pos);

	// 旋转变换
	void SetRotation(float ax, float ay, float az, float angle);
	void SetRotation(D3DXVECTOR3 axis, float angle);
	void SetRotationY(float angle);

	// 缩放变换
	void SetScaling(float sx, float sy, float sz);
	void SetScaling(D3DXVECTOR3 scale);

	void UpdateView(CCamera* camera);
//	void DrawBasicScene();

	void SetDirectionLight(D3DXVECTOR3 dir, D3DXCOLOR color);
	bool DefaultDisplay(float timeDelta);
	void BeginRender();
	void EndRender();

	template<class T> void Render(T element)
	{
		if(element)
		{
			element->Render();
		}
	}

private:
	IDirect3D9*				_d3d9;
	IDirect3DDevice9*		_d3dDevice9;
	D3DPRESENT_PARAMETERS	_d3dpp;
	D3DDEVTYPE				_d3dDeviceType;

	UINT _backWidth;
	UINT _backHeight;
	bool _windowed;

	D3DXMATRIX   _transMatrix; // 变换矩阵
};




namespace d3d
{
	template<class T> void Release(T t)
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}
		
	template<class T> void Delete(T t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}

	// vertex structure
	struct Vertex
	{
		Vertex(){}
		Vertex(float x, float y, float z, 
			float nx, float ny, float nz,
			float u, float v)
		{
			_x  = x;  _y  = y;  _z  = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u  = u;  _v  = v;
		}
		float _x, _y, _z;
		float _nx, _ny, _nz;
		float _u, _v;

		static const DWORD FVF;
	};

	struct TextureVertex
	{
		TextureVertex(){}
		TextureVertex(
			float x, float y, float z,
			float u, float v)
		{
			_x  = x;  _y  = y;  _z  = z;
			_u  = u;  _v  = v;
		}
		float _x, _y, _z;
		float _u, _v; // texture coordinates

		static const DWORD FVF;
	};

	// Colors
	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

	const D3DXCOLOR BEACH_SAND( D3DCOLOR_XRGB(255, 249, 157) );
	const D3DXCOLOR DESERT_SAND( D3DCOLOR_XRGB(250, 205, 135) );

	const D3DXCOLOR LIGHTGREEN( D3DCOLOR_XRGB( 60, 184, 120) );
	const D3DXCOLOR  PUREGREEN( D3DCOLOR_XRGB(  0, 166,  81) );
	const D3DXCOLOR  DARKGREEN( D3DCOLOR_XRGB(  0, 114,  54) );

	const D3DXCOLOR LIGHT_YELLOW_GREEN( D3DCOLOR_XRGB(124, 197, 118) );
	const D3DXCOLOR  PURE_YELLOW_GREEN( D3DCOLOR_XRGB( 57, 181,  74) );
	const D3DXCOLOR  DARK_YELLOW_GREEN( D3DCOLOR_XRGB( 25, 123,  48) );

	const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
	const D3DXCOLOR DARKBROWN( D3DCOLOR_XRGB(115, 100,  87));

	// Lights
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

	// Materials
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

	const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	// Bounding Objects
	struct BoundingBox
	{
		BoundingBox();

		bool isPointInside(D3DXVECTOR3& p);

		D3DXVECTOR3 _min;
		D3DXVECTOR3 _max;
	};

	struct BoundingSphere
	{
		BoundingSphere();

		D3DXVECTOR3 _center;
		float       _radius;
	};

	// Constants
	const float INFINITY = FLT_MAX;
	const float EPSILON  = 0.001f;

	// Randomness
	// Desc: Return random float in [lowBound, highBound] interval.
	float GetRandomFloat(float lowBound, float highBound);
	
	// Desc: Returns a random vector in the bounds specified by min and max.
	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max);

	// Conversion
	DWORD FtoDw(float f);

	// Interpolation
	float Lerp(float a, float b, float t);
}