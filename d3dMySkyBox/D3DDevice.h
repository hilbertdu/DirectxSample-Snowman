#pragma once
/* 
 * D3D设备类，负责保存D3D设备指针，参数，后备缓冲大小以及D3D的初始化
 */

#include "stdafx.h"
#include "D3DUtils.h"

class CD3DDevice
{
public:
	CD3DDevice(HWND hWnd);
	~CD3DDevice(void);

	bool InitD3D(/*HWND hwnd, UINT backWidth, UINT backHeight, bool windowed*/);
	void SetDeviceType(D3DDEVTYPE deviceType);
	void SetD3DWindow(UINT backWidth, UINT backHeight, bool windowed);
	void GetD3DRect(RECT* rect);

	IDirect3DDevice9* GetDevicePoint();

private:
	IDirect3D9*				_d3d9;
	IDirect3DDevice9*		_d3dDevice9;
	D3DPRESENT_PARAMETERS	_d3dpp;
	D3DDEVTYPE				_d3dDeviceType;

	HWND _hWnd;
	UINT _backWidth;
	UINT _backHeight;
	bool _windowed;
};

