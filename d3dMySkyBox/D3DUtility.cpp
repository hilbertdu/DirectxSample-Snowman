#include "D3DUtility.h"


CD3DUtility::CD3DUtility(void)
{
	_backWidth = 0;
	_backHeight = 0;
	_windowed = true;
	_d3dDeviceType = D3DDEVTYPE_HAL;
}

CD3DUtility::~CD3DUtility(void)
{
}

void CD3DUtility::SetHW(UINT width, UINT height, bool windowed)
{
	_backWidth = width;
	_backHeight = height;
	_windowed = windowed;
}

void CD3DUtility::GetRect(RECT* rect)
{
	rect->left = 0;
	rect->top = 0;
	rect->right = _backWidth;
	rect->bottom = _backHeight;
}

void CD3DUtility::SetDeviceType(D3DDEVTYPE deviceType)
{
	_d3dDeviceType = deviceType;
}

IDirect3DDevice9* CD3DUtility::GetDevicePoint()
{
	return _d3dDevice9;
}

/* 初始化D3D设备,成功返回1，失败返回0 */
bool CD3DUtility::InitD3D(HWND hwnd)
{
	HRESULT hr = 0;
	// Step 1: Create the IDirect3D9 object.
    if(!(_d3d9 = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		::MessageBox(0, _T("Direct3DCreate9() - FAILED"), 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.
	D3DCAPS9 caps;
	_d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, _d3dDeviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Retrieves the current display mode of the adapter.
	D3DDISPLAYMODE d3ddm;
    if(FAILED(_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
		::MessageBox(0, _T("GetAdapterDisplayMode() - FAILED"), 0, 0);
        return false;
	}

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	_d3dpp.BackBufferWidth            = _backWidth;
	_d3dpp.BackBufferHeight           = _backHeight;
	_d3dpp.BackBufferFormat           = d3ddm.Format; //D3DFMT_A8R8G8B8;
	_d3dpp.BackBufferCount            = 1;
	_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	_d3dpp.MultiSampleQuality         = 0;
	_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	_d3dpp.hDeviceWindow              = hwnd;
	_d3dpp.Windowed                   = _windowed;
	_d3dpp.EnableAutoDepthStencil     = true; 
	_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	_d3dpp.Flags                      = 0;
	_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	hr = _d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		_d3dDeviceType,     // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
	    &_d3dpp,            // present parameters
	    &_d3dDevice9);       // return created device

	if( FAILED(hr) )
	{
		_d3d9->Release(); // done with d3d9 object
		::MessageBox(0, _T("CreateDevice() - FAILED"), 0, 0);
		return false;
	}

	_d3d9->Release(); // done with d3d9 object
	
	return true;
}

void CD3DUtility::SetWorldTransform()
{
	_d3dDevice9->SetTransform(D3DTS_WORLD, &_transMatrix);
}

void CD3DUtility::SetWorldTransform(D3DXMATRIX* matrix)
{
	_d3dDevice9->SetTransform(D3DTS_WORLD, matrix);
}

void CD3DUtility::MutliTransformMatrix(D3DXMATRIX* matrix)
{
	_transMatrix *= *matrix;
}

void CD3DUtility::ResetTransformMatrix()
{
	D3DXMatrixIdentity(&_transMatrix);
}

void CD3DUtility::SetTranslation(D3DXVECTOR3 pos)
{
	SetTranslation(pos.x, pos.y, pos.z);
}

void CD3DUtility::SetScaling(D3DXVECTOR3 pos)
{
	SetScaling(pos.x, pos.y, pos.z);
}

void CD3DUtility::SetRotation(D3DXVECTOR3 axis, float angle)
{
	D3DXMATRIX world;
	D3DXMatrixRotationAxis(&world, &axis, angle);
	_transMatrix *= world;
}

void CD3DUtility::SetTranslation(float x, float y, float z)
{
	D3DXMATRIX world;
	D3DXMatrixTranslation(&world, x, y, z);
	_transMatrix *= world;
}

void CD3DUtility::SetScaling(float sx, float sy, float sz)
{
	D3DXMATRIX world;
	D3DXMatrixScaling(&world, sx, sy, sz);
	_transMatrix *= world;
}

void CD3DUtility::SetRotation(float ax, float ay, float az, float angle)
{
	D3DXVECTOR3 axis(ax, ay, az);
	SetRotation(axis, angle);
}

void CD3DUtility::SetRotationY(float angle)
{
	D3DXMATRIX world;
	D3DXMatrixRotationY(&world, angle);
	_transMatrix *= world;
}

void CD3DUtility::SetViewTransform(float x, float y, float z)
{
	D3DXVECTOR3 pos(x, y, z);
	D3DXVECTOR3 target(0.0f, 0.0f, 10.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	_d3dDevice9->SetTransform(D3DTS_VIEW, &V);
}

void CD3DUtility::SetProjectionTransform(float fovy, float zn, float zf)
{
	// Set the projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, 
					fovy, 
					(float)_backWidth / (float)_backHeight, 
					zn, 
					zf);
	_d3dDevice9->SetTransform(D3DTS_PROJECTION, &proj);
}

void CD3DUtility::SetDirectionLight(D3DXVECTOR3 dir, D3DXCOLOR color)
{
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &color);

	_d3dDevice9->SetLight(0, &light);
	_d3dDevice9->LightEnable(0, true);
	_d3dDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	_d3dDevice9->SetRenderState(D3DRS_SPECULARENABLE, true);
}

void CD3DUtility::UpdateView(CCamera* camera)
{
	if(_d3dDevice9)
	{
		// Update the view matrix representing the cameras 
        // new position/orientation.
		D3DXMATRIX V;
		camera->getViewMatrix(&V);
		_d3dDevice9->SetTransform(D3DTS_VIEW, &V);
	}
}

void CD3DUtility::BeginRender()
{
	//_d3dDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	_d3dDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	_d3dDevice9->BeginScene();
}
	
void CD3DUtility::EndRender()
{
	_d3dDevice9->EndScene();
	_d3dDevice9->Present(0, 0, 0, 0);
}


// d3d namespace members

const DWORD d3d::TextureVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
const DWORD d3d::Vertex::FVF        = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

d3d::BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = d3d::INFINITY;
	_min.y = d3d::INFINITY;
	_min.z = d3d::INFINITY;

	_max.x = -d3d::INFINITY;
	_max.y = -d3d::INFINITY;
	_max.z = -d3d::INFINITY;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

d3d::BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

float d3d::GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void d3d::GetRandomVector(
	  D3DXVECTOR3* out,
	  D3DXVECTOR3* min,
	  D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f)
{
	return *((DWORD*)&f);
}

float d3d::Lerp(float a, float b, float t)
{
	return a - (a*t) + (b*t);
}