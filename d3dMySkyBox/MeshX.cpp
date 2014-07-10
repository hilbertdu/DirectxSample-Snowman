#include "StdAfx.h"
#include "MeshX.h"


CMeshX::CMeshX(void)
{
}


CMeshX::~CMeshX(void)
{
	for(int i = 0; i < _Textures.size(); i++)
		d3d::Release<IDirect3DTexture9*>( _Textures[i] );

	d3d::Release<ID3DXMesh*>(_Mesh);
}

CMeshX::CMeshX(IDirect3DDevice9* device, TCHAR* xfilename, TCHAR* parentpath)
{
	_device = device;

	wsprintf(_xfileDir, _T("%s"), parentpath);

	// 默认位置
	SetPosition(0, 0, 0);
	SetScaling(1.0f, 1.0f, 1.0f);
	SetRotationY(0.0f);
	
	if(FAILED(D3DXLoadMeshFromX(
				xfilename,
				D3DXMESH_MANAGED,
				_device,
				&_adjBuffer,
				&_mtrlBuffer,
				0,
				&_numMtrls,
				&_Mesh)))
	{
		::MessageBox(0, _T("D3DXLoadMeshFromX() - FAILED"), 0, 0);
		return ;
	}

	// 提取材质，加载纹理
	if( _mtrlBuffer != 0 && _numMtrls != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)_mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < _numMtrls; i++)
		{
			// 设置Ambient color
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// 保存材质
			_Mtrls.push_back( mtrls[i].MatD3D );

			// 加载纹理
			if( mtrls[i].pTextureFilename != 0 )
			{
				// 纹理路径转换成unicode,使用完整路径
				TCHAR path[255] = {0};
				TCHAR TextureFullPath[255] = {0};
				MultiByteToWideChar(CP_ACP, 0, mtrls[i].pTextureFilename, -1, path, 100*sizeof(WCHAR));
				wsprintf(TextureFullPath, _T("%s\\%s"), _xfileDir, path);

				IDirect3DTexture9* tex = 0;
				if( D3D_OK != D3DXCreateTextureFromFile(
					_device,
					//mtrls[i].pTextureFilename,
					TextureFullPath,
					&tex))
				{
					::MessageBox(0, TextureFullPath, 0, 0);
				}

				_Textures.push_back( tex );
			}
			else
			{
				_Textures.push_back( 0 );
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(_mtrlBuffer);

	// 优化网格
	if(FAILED(_Mesh->OptimizeInplace(		
				D3DXMESHOPT_ATTRSORT |
				D3DXMESHOPT_COMPACT  |
				D3DXMESHOPT_VERTEXCACHE,
				(DWORD*)_adjBuffer->GetBufferPointer(),
				0, 0, 0)))
	{
		::MessageBox(0, _T("OptimizeInplace() - FAILED"), 0, 0);
		d3d::Release<ID3DXBuffer*>(_adjBuffer);
		return ;
	}
	d3d::Release<ID3DXBuffer*>(_adjBuffer);

	AddNormals();

	return ;
}

// 如果没有法线信息，手动计算法线
bool CMeshX::AddNormals()
{
	if (_Mesh->GetFVF() & D3DFVF_NORMAL)
	{
		return TRUE;
	} 
	else
	{
		ID3DXMesh * pTempMesh = NULL;
		pTempMesh->CloneMeshFVF(D3DXMESH_MANAGED, _Mesh->GetFVF() | D3DFVF_NORMAL, _device, &_Mesh);
		D3DXComputeNormals(pTempMesh,0);
		_Mesh->Release();
		_Mesh = pTempMesh;
		return TRUE;
	}
}

void CMeshX::SetPosition(float x, float y, float z)
{
	_pos.x = x;
	_pos.y = y;
	_pos.z = z;
}

void CMeshX::SetScaling(float sx, float sy, float sz)
{
	_scale.x = sx;
	_scale.y = sy;
	_scale.z = sz;
}

void CMeshX::SetRotationY(float angle)
{
	_angley = angle;
}

D3DXVECTOR3 CMeshX::GetPosition()
{
	return _pos;
}

D3DXVECTOR3 CMeshX::GetScaling()
{
	return _scale;
}

float CMeshX::GetRotationY()
{
	return _angley;
}

bool CMeshX::Render()
{
	if(_Mtrls.size() == 0)
	{
		return false;
	}

	// Set texture filters
	_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	for(int i = 0; i < _Mtrls.size(); i++)
	{
		_device->SetMaterial( &_Mtrls[i] );
		_device->SetTexture(0, _Textures[i]);
		_Mesh->DrawSubset(i);
	}	
	return true;
}

ID3DXMesh* CMeshX::GetID3DXMesh()
{
	return _Mesh;
}