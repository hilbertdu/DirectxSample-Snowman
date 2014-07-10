#pragma once
/*
 * 游戏场景类，负责整个场景的创建和绘制，角色的漫游
 */

#include "stdafx.h"
#include "D3DUtils.h"
#include "D3DTransform.h"
#include "Camera.h"
#include "SkyBox.h"
#include "PSystem.h"
#include "Terrain.h"
#include "MeshX.h"

#include "ShadowVolume.h"


class CGameScene
{
public:
	CGameScene(void);
	~CGameScene(void);

	bool InitGameScene(HWND hwnd);
	bool InitFont();
	bool CreateSkyBox();
	bool CreateCamera();
	bool CreateTerrain();
	bool CreateParticle();
	bool CreateMesh();
	void CreateLight();

	void SetWindow(UINT width, UINT height, bool windowed);

	void Display(float timeDelta);

private:
	// functions
	void FrameUpdate();
	void FrameRender();
	bool RenderScene();
	void RenderParticle();

	void GetMouseInput();

	void SetFPVPosition(D3DXVECTOR3* pos);
	void GetFPVPosition(D3DXVECTOR3* pos);
	void SetFPVFaceDir(D3DXVECTOR3* dir);

	void UpdateCameraView();
	void AdjustCameraPos(float h);

	void UpdatePartical(float deltaTime);

	bool CheckBound(D3DXVECTOR3* pos, D3DXVECTOR3* dir);	// 边界检测

	void CountFPS(float deltaTime);

	void ShowMode();	// 显示模式
	void ShowHelp();	// 显示帮助

	// member values
	// 窗口尺寸
	UINT _windowWidth;
	UINT _windowHeight;
	bool _windowed;
	
	// 特效选项
	bool _hasShadow;
	bool _hasParticle;

	// 场景元素
	CD3DDevice*				_d3dDevice;
	CD3DTransform*			_d3dTransform;
	CSkyBox*				_skybox;
	CTerrain*		        _terrain;
	psys::CPSystem*			_particle;
	CCamera*				_camera;
	CCamera*				_lastModeCamera;

	// 网格对象
	std::vector<CMeshX*>	_mesh;

	// 光照元素(本例中仅有一个平行光和ambient light)
	D3DLIGHT9   _dirLight;
	D3DXCOLOR   _ambientLight[5];
	UINT		_indexALight;

	// 第三视角（本例中是雪人）
	CMeshX*		_FPVman;
	D3DXMATRIX  _FPVMatrix; // 变换矩阵
	D3DXVECTOR3 _FVPpos;
	D3DXVECTOR3 _faceDir; //面部方向
	float		_speed;	  //行走速度
	float	    _angle;	  //旋转角度
	float       _ahead;	  //雪人离摄像机的距离
	
	bool  _isMouse;		  //是否有鼠标点击
	POINT _lastMousePos;
	POINT _currentMousePos;

	bool _isMoving;

	// 摄像机模式(第一视角漫游or第三视角)
	bool _mode;

	// 边界
	float _maxy;
	float _miny;
	float _maxx;
	float _minx;
	float _maxz;
	float _minz;

	// 字体
	ID3DXFont*		_Font;
	D3DXFONT_DESC	_lf;
	TCHAR			_outputStr[256];

	// FPS
	DWORD _FPS;

	// CShadowVolume类
	CShadowVolume*  _shadowVolume;
};

