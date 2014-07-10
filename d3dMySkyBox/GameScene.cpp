#include "GameScene.h"

CGameScene::CGameScene(void)
{
	_d3dDevice = NULL;
	_skybox		= NULL;
	_terrain    = NULL;
	_particle   = NULL;
	_camera		= NULL;
	_lastModeCamera = NULL;

	_faceDir.x  = _faceDir.y  = 0;
	_faceDir.z  = 1.0;
	_FPVman     = NULL;
	_speed      = 15.0f;
	_angle		= 0.01f;
	_ahead		= 20.0f;
	
	_isMouse	= false;
	_lastMousePos.x = _lastMousePos.y = 0;
	_currentMousePos.x = _currentMousePos.y = 0;
	
	_isMoving = true;
	_mode     = true;	// 第三视角雪人
	wsprintf(_outputStr, _T("%s"), _T("mode: following snowman"));

	// 边界
	_maxx = _maxy = _maxz = 1;
	_minx = _miny = _minz = -1;

	_FPS = 0;

	// 阴影
	_hasShadow = true;
	_shadowVolume = new CShadowVolume();

	_hasShadow = false;
	_hasParticle = false;
}

CGameScene::~CGameScene(void)
{
	// 释放指针
	delete _skybox;
	delete _terrain;
	delete _particle;
	delete _camera;
	delete _lastModeCamera;
	delete _d3dDevice;
	delete _FPVman;

	for (std::vector<CMeshX*>::iterator it = _mesh.begin(); it != _mesh.end(); it ++) 
    if (*it) 
    {
        delete *it; 
        *it = NULL;
    }
	_mesh.clear();
	d3d::Release<ID3DXFont*>(_Font);
}

void CGameScene::SetWindow(UINT width, UINT height, bool windowed)
{
	_windowWidth = width;
	_windowHeight = height;
	_windowed = windowed;
}

bool CGameScene::InitGameScene(HWND hwnd)
{
	_d3dDevice = new CD3DDevice(hwnd);
	_d3dDevice->SetD3DWindow(_windowWidth, _windowHeight, _windowed);

	if(!_d3dDevice->InitD3D())
	{
		::MessageBox(hwnd, _T("InitD3D Error"), 0, 0);
		return false;
	}

	_d3dTransform = new CD3DTransform(_d3dDevice->GetDevicePoint());

	if(!InitFont())
	{
		::MessageBox(hwnd, _T("InitFont Error"), 0, 0);
		return false;
	}

	return true;
}

// 创建一个天空盒
bool CGameScene::CreateSkyBox()
{
	if(_d3dDevice)
	{
		_skybox = new CSkyBox(_d3dDevice->GetDevicePoint());
		if(!_skybox->InitSkyBox(300))
		{
			::MessageBox(NULL, _T("Initialize SkyBox failed!"), 0, 0);
		}

		// 设置天空纹理
		_skybox->SetTexture(_T(".\\resource\\skybox\\bleached\\bleached_right.jpg"),0);
		_skybox->SetTexture(_T(".\\resource\\skybox\\bleached\\bleached_left.jpg"),1);
		_skybox->SetTexture(_T(".\\resource\\skybox\\bleached\\bleached_top.jpg"),2);
		_skybox->SetTexture(_T(".\\resource\\skybox\\bleached\\bleached_front.jpg"),3);
		_skybox->SetTexture(_T(".\\resource\\skybox\\bleached\\bleached_back.jpg"),4);
		/*_skybox->SetTexture(_T(".\\resource\\skybox\\skybox1\\skybox1_posx.jpg"),0);
		_skybox->SetTexture(_T(".\\resource\\skybox\\skybox1\\skybox1_negx.jpg"),1);
		_skybox->SetTexture(_T(".\\resource\\skybox\\skybox1\\skybox1_posy.jpg"),2);
		_skybox->SetTexture(_T(".\\resource\\skybox\\skybox1\\skybox1_posz.jpg"),3);
		_skybox->SetTexture(_T(".\\resource\\skybox\\skybox1\\skybox1_negz.jpg"),4);*/

		return true;
	}
	else
	{
		return false;
	}
}

bool CGameScene::CreateMesh()
{
	if(_d3dDevice)
	{
		// 第三视角雪人
		_FPVman = new CMeshX(_d3dDevice->GetDevicePoint(), _T(".\\resource\\objects\\snowman\\snowman.x"), _T(".\\resource\\objects\\snowman"));
		_FPVman->SetScaling(0.05f, 0.05f, 0.05f);

		// 飞机
		//CMeshX* meshShip = new CMeshX(_d3dUtility->GetDevicePoint(), _T(".\\resource\\objects\\bigship1.x"), _T(".\\resource\\objects"));
		//meshShip->SetPosition(20.0f, 100.0f, 20.0f);
		//meshShip->SetScaling(0.5f, 0.5f, 0.5f);
		//_mesh.push_back(meshShip);

		// 房屋
		CMeshX* temple = new CMeshX(_d3dDevice->GetDevicePoint(), _T(".\\resource\\objects\\temple\\temple.x"), _T(".\\resource\\objects\\temple"));
		temple->SetPosition(-215.0f, 50.0f, -150.0f);
		temple->SetScaling(0.05f, 0.05f, 0.05f);
		temple->SetRotationY(-acos(0.0f));
		_mesh.push_back(temple);

		// 树木
		CMeshX* tree = new CMeshX(_d3dDevice->GetDevicePoint(), _T(".\\resource\\objects\\tree2\\tree2.x"), _T(".\\resource\\objects\\tree2"));
		tree->SetPosition(20.0f, 50.0f, 20.0f);
		tree->SetScaling(0.05f, 0.05f, 0.05f);
		//tree->SetRotationY(-acos(0.0));
		_mesh.push_back(tree);

		return true;
	}
	else
	{
		return false;
	}
}

bool CGameScene::CreateTerrain()
{
	if(_d3dDevice)
	{
		_terrain = new CTerrain(_d3dDevice->GetDevicePoint(), ".\\resource\\terrain\\coastMountain64.raw", 64, 64, 10, 0.5f);
		_terrain->loadTexture(_T(".\\resource\\terrain\\ground07.jpg"));
		
		// 设置边界
		_maxx = _maxz = 280;
		_minx = _minz = -280;
		_maxy = 200;
		_miny = -100;

		return true;
	}
	else
	{
		return false;
	}
}

// 创建粒子
bool CGameScene::CreateParticle()
{
	if(!_d3dDevice)
	{
		return false;
	}
	d3d::BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-20.0f, 0.0f, -20.0f);
	boundingBox._max = D3DXVECTOR3( 20.0f, 40.0f,  20.0f);
	_particle = new psys::CSnow(&boundingBox, 800);
	_particle->init(_d3dDevice->GetDevicePoint(), _T(".\\resource\\partical\\snowflake.dds"));

	return true;
}

// 设置照相机
bool CGameScene::CreateCamera()
{
	if(_d3dDevice)
	{
		_camera = new CCamera();
		_lastModeCamera = new CCamera();

		// 设置投影矩阵
		_d3dTransform->SetProjectionTransform(D3DX_PI * 0.5f, 1.0f, 1000.0f, _windowWidth, _windowHeight);
		return true;
	}
	else
	{
		return false;
	}
}

void CGameScene::CreateLight()
{
	// 创建平行光
	D3DXVECTOR3 lightDir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR   lightCol = d3d::WHITE;
	_dirLight = d3d::InitDirectionalLight(&lightDir, &lightCol);

	// 创建环境光
	_ambientLight[0] = d3d::DARK;
	_ambientLight[1] = d3d::RED;
	_ambientLight[2] = d3d::GREEN;
	_ambientLight[3] = d3d::YELLOW;
	_ambientLight[4] = d3d::DESERT_SAND;
	_indexALight = 0;
}

void CGameScene::SetFPVPosition(D3DXVECTOR3* pos)
{
	_FVPpos = *pos;
}

void CGameScene::GetFPVPosition(D3DXVECTOR3* pos)
{
	*pos = _FVPpos;
}

void CGameScene::SetFPVFaceDir(D3DXVECTOR3* dir)
{
	_faceDir = *dir;
}

void CGameScene::FrameUpdate()
{
	// 如果是雪人的第三视角，更新雪人的transform
	if(_FPVman)
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 z(0, 0, 1.0f);

		_d3dTransform->ResetTransformMatrix();
		// 一定要先做缩放，再做平移！
		_d3dTransform->SetScaling(_FPVman->GetScaling());
		// 先绕Y轴旋转180度(面向世界矩阵的z轴)
		_d3dTransform->SetRotationY(acos(-1.0f));

		if(_isMoving && _mode)
		{
			// set position
			D3DXVECTOR3 look;
			
			_camera->getPosition(&pos);
			_camera->getLook(&look);
	
			pos += _ahead*look;
			if(_terrain)
				pos.y = _terrain->getHeight( pos.x, pos.z );
			SetFPVPosition(&pos);
			// 变更雪人的面部朝向(始终朝向摄像机look方向)
			_faceDir.x = look.x;
			_faceDir.z = look.z;
			_faceDir.y = 0;
			float angle = acos(D3DXVec3Dot(&z, &_faceDir) / D3DXVec3Length(&_faceDir));

			if(_faceDir.x >= 0)
			{
				_d3dTransform->SetRotationY(angle);
			}
			else
			{
				_d3dTransform->SetRotationY(-angle);
			}
			//_isMoving = false;
		}
		else
		{
			// 设置面部朝向
			float angle = acos(D3DXVec3Dot(&z, &_faceDir) / D3DXVec3Length(&_faceDir));
			if(_faceDir.x >= 0)
			{
				_d3dTransform->SetRotationY(angle);
			}
			else
			{
				_d3dTransform->SetRotationY(-angle);
			}
		}
		GetFPVPosition(&pos);
		_d3dTransform->SetTranslation(pos);	

		// 保存雪人的变换矩阵
		_d3dTransform->GetD3DXMatrix(&_FPVMatrix);
	}
	
	if(_hasShadow)
	{
		static bool firstBuildVolume = true;
		if(_isMoving || firstBuildVolume)
		{
			firstBuildVolume = false;
			// 生成阴影体
			D3DXVECTOR3 vLightInWorldDir(_dirLight.Direction);
			D3DXVECTOR3 vLightInObjectDir;
			D3DXMATRIXA16 matInverse;
			D3DXMatrixInverse( &matInverse, NULL, &_FPVMatrix );
			D3DXVec3TransformNormal( &vLightInObjectDir, &vLightInWorldDir, &matInverse );

			_shadowVolume->Reset();
			_shadowVolume->BuildShadowVolume(_FPVman->GetID3DXMesh(), vLightInObjectDir);
		}
	}
	_isMoving = false;
}

void CGameScene::FrameRender()
{
	IDirect3DDevice9* device = _d3dDevice->GetDevicePoint();
	// Ambient light
	device->SetRenderState(D3DRS_AMBIENT, _ambientLight[_indexALight]);
	device->SetLight(0, &_dirLight);
	device->LightEnable(0, true);

	if(_hasShadow)
	{
		// 第一步生成阴影体在FrameUpdate中完成
		// 第二步，关闭光照，绘制场景，此时z-buffer保存了深度信息
		d3d::BeginRender(_d3dDevice->GetDevicePoint());

		// 关闭光照(开启ambient light)
		device->SetRenderState(D3DRS_LIGHTING, TRUE); 
		device->LightEnable(0, FALSE);

		// 打开z-buffer写和test
		device->SetRenderState( D3DRS_ZENABLE, TRUE );
		device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		// 关闭stencil buffer
		device->SetRenderState( D3DRS_STENCILENABLE, FALSE );

		// 渲染场景到z-buffer中
		RenderScene();

		// 第三步，设置第一轮z-pass 渲染
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); // 关闭z-buffer写

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// 开启stencil test
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE); 
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); 

		// stencil和depth测试都通过时stencil值加1，渲染正面（剔除掉逆时针三角形）
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
		device->SetTransform( D3DTS_WORLD, &_FPVMatrix );
		_shadowVolume->Render(device);

		// 设置第二轮z pass 渲染, stencil和depth测试都通过时stencil值减1，渲染反面
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR); 
		device->SetTransform( D3DTS_WORLD, &_FPVMatrix );
		_shadowVolume->Render(device);

		// 还原渲染设置
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		// 再次渲染场景，只渲染stencil值为0的像素
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

		// 打开光源并渲染场景
		device->LightEnable(0, TRUE);
		RenderScene();

		// 还原渲染设置
		device->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
		device->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
		device->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
		device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		// 渲染剩余的元素
		RenderParticle();
		ShowMode();
		ShowHelp();
		d3d::EndRender(device);
	}
	else
	{
		d3d::BeginRender(_d3dDevice->GetDevicePoint());
		RenderScene();
		RenderParticle();
		ShowMode();
		ShowHelp();
		d3d::EndRender(device);
	}
}

bool CGameScene::RenderScene()
{
	// 绘制场景元素
	if(_skybox)
	{
		D3DXVECTOR3 pos;
		_camera->getPosition(&pos);
		_d3dTransform->ResetTransformMatrix();
		_d3dTransform->SetTranslation(pos);
		_d3dTransform->SetWorldTransform();
		d3d::Render<CSkyBox*>(_skybox);
	}
	if(_terrain)
	{
		_d3dTransform->ResetTransformMatrix();
		_d3dTransform->SetTranslation(0, 0, 0);
		_d3dTransform->SetWorldTransform();
		d3d::Render<CTerrain*>(_terrain);
	}
	if(_FPVman)
	{	
		_d3dTransform->SetWorldTransform(&_FPVMatrix);
		d3d::Render<CMeshX*>(_FPVman);	
	}
	if(_mesh.size() > 0)
	{
		for(unsigned int i=0; i<_mesh.size(); i++)
		{
			_d3dTransform->ResetTransformMatrix();
			_d3dTransform->SetScaling(_mesh[i]->GetScaling());
			_d3dTransform->SetRotationY(_mesh[i]->GetRotationY());
			_d3dTransform->SetTranslation(_mesh[i]->GetPosition());
			_d3dTransform->SetWorldTransform();
			d3d::Render<CMeshX*>(_mesh[i]);
		}
	}
	return true;
}

// 渲染粒子
void CGameScene::RenderParticle()
{
	if(_particle && _hasParticle)
	{
		D3DXVECTOR3 pos;
		_camera->getPosition(&pos);
		_d3dTransform->ResetTransformMatrix();
		_d3dTransform->SetTranslation(pos.x, pos.y, pos.z);
		_d3dTransform->SetWorldTransform();
		d3d::Render<psys::CPSystem*>(_particle);
	}
}

// 获取鼠标输入，左右旋转摄像机
void CGameScene::GetMouseInput()
{
	POINT mousePos;
	GetCursorPos( &mousePos );
	ScreenToClient( NULL, &mousePos );

	_currentMousePos.x = mousePos.x;
	_currentMousePos.y = mousePos.y;

	if(_isMouse)
	{
		int xDiff = _currentMousePos.x - _lastMousePos.x;
		int yDiff = _currentMousePos.y - _lastMousePos.y;

		if(xDiff != 0)
		{
			D3DXVECTOR3 pos, look;
			D3DXVECTOR3 y(0, 1.0, 0);
			// 摄像机绕y轴旋转(注意不是up轴)
			//_camera->yaw(-xDiff*_angle);
			_camera->rotation(&y, xDiff*_angle);

			if(_mode)
			{
				// 更新摄像机的位置
				_camera->getLook(&look);
				GetFPVPosition(&pos);
				pos += (-_ahead*look);
				_camera->setPosition(&pos);
			}
		}

		if(yDiff != 0)
		{
			D3DXVECTOR3 pos, look;
			// 摄像机绕right轴旋转
			_camera->pitch(yDiff*_angle);

			if(_mode)
			{
				// 更新摄像机的位置
				_camera->getLook(&look);
				GetFPVPosition(&pos);
				pos += (-_ahead*look);
				_camera->setPosition(&pos);
			}
		}
	}

	_lastMousePos.x = _currentMousePos.x;
	_lastMousePos.y = _currentMousePos.y;
}

// 检测摄像机位置是否超出边界
bool CGameScene::CheckBound(D3DXVECTOR3* pos, D3DXVECTOR3* dir)
{
	float distance, maxdis;
	
	for(int i=0; i<3; i++)
	{
		switch(i)
		{
		case 0:
			if(dir->x > 0)
			{
				distance = pos->x;
				maxdis = _maxx;
			}
			else
			{
				distance = -pos->x;
				maxdis = -_minx;
			}
			break;
		case 1:
			if(dir->z > 0)
			{
				distance = pos->z;
				maxdis = _maxz;
			}
			else
			{
				distance = -pos->z;
				maxdis = -_minz;
			}
			break;
		case 2:
			if(dir->y > 0)
			{
				distance = pos->y;
				maxdis = _maxy;
			}
			else
			{
				distance = -pos->y;
				maxdis = -_miny;
			}
			break;
		}

		// 判断
		if(!(distance <= maxdis))
		{
			return false;
		}
	}
	return true;
}

void CGameScene::Display(float timeDelta)
{
	CountFPS(timeDelta);

	if(_d3dDevice)
	{
		D3DXVECTOR3 cameraPos, FPVPos, look, right, up, dir;

		if(_FPVman)
		{
			FPVPos = _FPVman->GetPosition();
		}
		_camera->getPosition(&cameraPos);
		_camera->getLook(&look);
		_camera->getRight(&right);
		_camera->getUp(&up);

		// 是否开启特效
		if(::GetAsyncKeyState('O') & 0x0001f)
		{
			_hasShadow = !_hasShadow;
		}
		if(::GetAsyncKeyState('P') & 0x0001f)
		{
			_hasParticle = !_hasParticle;
		}
		// 是否切换环境光
		if(::GetAsyncKeyState('L') & 0x0001f)
		{
			_indexALight = (_indexALight+1) % 5;
		}

		// 改变视角模式
		if(::GetAsyncKeyState(VK_CONTROL) & 0x0001f)
		{
			_mode = !_mode;
			if(!_mode)
			{
				// 第一视角
				_isMoving = false;
				*_lastModeCamera = CCamera(*_camera);
			}
			else
			{
				// 第三视角
				_isMoving = true;
				*_camera = CCamera(*_lastModeCamera);
			}
		}

		if( ::GetAsyncKeyState(VK_SPACE) & 0x8000f && (!_mode))
		{
			dir = up;
			if(CheckBound(&cameraPos, &dir))
			{
				_camera->fly(_speed * timeDelta);
			}
		}

		// 获取鼠标左键状态
		if(::GetAsyncKeyState(VK_LBUTTON))
		{
			_isMouse = true;
		}
		else
		{
			_isMouse = false;
		}

		GetMouseInput();
		
		// 获取键盘输入状态
		if( (::GetAsyncKeyState('W') & 0x8000f) || (::GetAsyncKeyState(VK_UP) & 0x8000f))
		{
			dir = look;
			if(CheckBound(&cameraPos, &dir))
			{
				if(!_FPVman)
				{
					_camera->walk(_speed * timeDelta);
				}
				else if(CheckBound(&FPVPos, &dir))
				{
					_camera->walk(_speed * timeDelta);
				}
			}
			_isMoving = true;
		}

		if( (::GetAsyncKeyState('S') & 0x8000f) || (::GetAsyncKeyState(VK_DOWN) & 0x8000f))
		{
			dir = -look;
			if(CheckBound(&cameraPos, &dir))
			{
				if(!_FPVman)
				{
					_camera->walk(-_speed * timeDelta);
				}
				else if(CheckBound(&FPVPos, &dir))
				{
					_camera->walk(-_speed * timeDelta);
				}
			}
			_isMoving = true;
		}

		if( (::GetAsyncKeyState('A') & 0x8000f) || (::GetAsyncKeyState(VK_LEFT) & 0x8000f))
		{
			dir = -right;
			if(CheckBound(&cameraPos, &dir))
			{
				if(!_FPVman)
				{
					_camera->strafe(-_speed * timeDelta);
				}
				else if(CheckBound(&FPVPos, &dir))
				{
					_camera->strafe(-_speed * timeDelta);
				}
			}
			_isMoving = true;
		}

		if( (::GetAsyncKeyState('D') & 0x8000f) || (::GetAsyncKeyState(VK_RIGHT) & 0x8000f))
		{
			dir = right;
			if(CheckBound(&cameraPos, &dir))
			{
				if(!_FPVman)
				{
					_camera->strafe(_speed * timeDelta);
				}
				else if(CheckBound(&FPVPos, &dir))
				{
					_camera->strafe(_speed * timeDelta);
				}
			}
			_isMoving = true;
		}

		AdjustCameraPos(10.0f);
		if(_hasParticle)
		{
			UpdatePartical(timeDelta);
		}
		UpdateCameraView();

		// 绘制场景
		FrameUpdate();
		FrameRender();
	}
}

// 调整摄像机的高度使之不低于地面高度 + h
void CGameScene::AdjustCameraPos(float h)
{
	if(_terrain)
	{
		D3DXVECTOR3 pos, look;
		D3DXVECTOR3 hpos;
		_camera->getPosition(&pos);
		_camera->getLook(&look);

		float height = _terrain->getHeight( pos.x, pos.z );
		if(pos.y <= height + h)
		{
			pos.y = height + h;
			_camera->setPosition(&pos);
		}
	}
}

void CGameScene::UpdateCameraView()
{
	if(_d3dDevice)
	{
		D3DXMATRIX V;
		_camera->getViewMatrix(&V);
		_d3dTransform->UpdateViewTransform(&V);
	}
}

void CGameScene::UpdatePartical(float deltaTime)
{
	if(_particle)
	{
		_particle->update(deltaTime);
	}
}

bool CGameScene::InitFont()
{
	ZeroMemory(&_lf, sizeof(D3DXFONT_DESC));

	_lf.Height         = 21;         // in logical units
	_lf.Width          = 6;          // in logical units     
	_lf.Weight         = FW_LIGHT;   // boldness, range 0(light) - 1000(bold)
	_lf.Italic         = false;        
	_lf.CharSet        = DEFAULT_CHARSET;
	_lf.OutputPrecision   = 0;                        
	_lf.Quality        = 255;           
	_lf.PitchAndFamily = 0;           
	//wsprintf(_lf.FaceName, _T("Times New Roman")); 
	wsprintf(_lf.FaceName, _T("Arial")); // 设置字体

	if(FAILED(D3DXCreateFontIndirect(_d3dDevice->GetDevicePoint(), &_lf, &_Font)))
	{
		return false;
	}
	return true;
}

void CGameScene::ShowMode()
{
	if(_mode)
	{
		wsprintf(_outputStr, _T("mode: following snowman\nFPS: %ld"), _FPS);
	}
	else
	{
		wsprintf(_outputStr, _T("mode: first person flying\nFPS: %ld"), _FPS);
	}

	RECT rect;
	_d3dDevice->GetD3DRect(&rect);
	
	_Font->DrawText(
		NULL,
		_outputStr, 
		-1,				  // size of string or -1 indicates null terminating string
		&rect,            // rectangle text is to be formatted to in windows coords
		DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
		0xff000000);      // black text
}

void CGameScene::ShowHelp()
{
	static TCHAR helpStr[256] = 
		_T("ASWD+鼠标左键控制行走方向\n[space]-flying模式中向上飞行\n[Ctrl]-切换漫游模式\n[O]-开启阴影效果\n[P]-开启粒子效果\n[L]-切换环境光");

	RECT rect;
	_d3dDevice->GetD3DRect(&rect);
	
	_Font->DrawText(
		NULL,
		helpStr, 
		-1,				  // size of string or -1 indicates null terminating string
		&rect,            // rectangle text is to be formatted to in windows coords
		DT_BOTTOM | DT_LEFT, // 显示在左下角
		0xff000000);      // black text
}

void CGameScene::CountFPS(float deltaTime)
{
	static DWORD FrameCnt = 0;
	static float TimeElapsed = 0.0;

	// 计算FPS
	FrameCnt++;
	TimeElapsed += deltaTime;
	if(TimeElapsed >= 1.0f)
	{
		_FPS = (DWORD)(FrameCnt / TimeElapsed);
		TimeElapsed = 0.0f;
		FrameCnt    = 0;
	}
}