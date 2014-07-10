#include "D3DDevice.h"


CD3DDevice::CD3DDevice(HWND hWnd)
{
	_hWnd = hWnd;
	_backWidth = 640;
	_backHeight = 480;
	_windowed = true;

	_d3dDeviceType = D3DDEVTYPE_HAL;
}

CD3DDevice::~CD3DDevice(void)
{
	d3d::Release<IDirect3DDevice9*>(_d3dDevice9);
	d3d::Release<IDirect3D9*>(_d3d9);
}

void CD3DDevice::SetDeviceType(D3DDEVTYPE deviceType)
{
	_d3dDeviceType = deviceType;
}

void CD3DDevice::SetD3DWindow(UINT backWidth, UINT backHeight, bool windowed)
{
	_backWidth = backWidth;
	_backHeight = backHeight;
	_windowed = windowed;
}

IDirect3DDevice9* CD3DDevice::GetDevicePoint()
{
	return _d3dDevice9;
}

void CD3DDevice::GetD3DRect(RECT* rect)
{
	rect->left = 0;
	rect->top = 0;
	rect->right = _backWidth;
	rect->bottom = _backHeight;
}

/* 初始化D3D设备,成功返回1，失败返回0 */
bool CD3DDevice::InitD3D()
{
	HRESULT hr = 0;
	// 创建IDirect3D9对象
    if(!(_d3d9 = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		::MessageBox(0, _T("Direct3DCreate9() - FAILED"), 0, 0);
		return false;
	}

	// 检测硬件能力
	D3DCAPS9 caps;
	_d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, _d3dDeviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 获取适配器的显示模式
	D3DDISPLAYMODE d3ddm;
    if(FAILED(_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
		::MessageBox(0, _T("GetAdapterDisplayMode() - FAILED"), 0, 0);
        return false;
	}

	// 填充D3DPRESENT_PARAMETERS结构.
	_d3dpp.BackBufferWidth            = _backWidth;
	_d3dpp.BackBufferHeight           = _backHeight;
	_d3dpp.BackBufferFormat           = d3ddm.Format; //D3DFMT_A8R8G8B8;
	_d3dpp.BackBufferCount            = 1;
	_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	_d3dpp.MultiSampleQuality         = 0;
	_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	_d3dpp.hDeviceWindow              = _hWnd;
	_d3dpp.Windowed                   = _windowed;
	_d3dpp.EnableAutoDepthStencil     = true; 
	_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	_d3dpp.Flags                      = 0;
	_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 创建设备
	hr = _d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		_d3dDeviceType,     // device type
		_hWnd,              // window associated with device
		vp,                 // vertex processing
	    &_d3dpp,            // present parameters
	    &_d3dDevice9);      // return created device

	if( FAILED(hr) )
	{
		_d3d9->Release();
		::MessageBox(0, _T("CreateDevice() - FAILED"), 0, 0);
		return false;
	}

	_d3d9->Release();

	return true;
}

