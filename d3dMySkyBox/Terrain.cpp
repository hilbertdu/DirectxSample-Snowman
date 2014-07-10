#include "Terrain.h"
#include <fstream>
#include <cmath>

CTerrain::CTerrain(IDirect3DDevice9* device,
				 std::string heightmapFileName,
				 int numVertsPerRow,	
				 int numVertsPerCol,	
				 int cellSpacing,		
				 float heightScale)
{
	_device         = device;
	_numVertsPerRow = numVertsPerRow;
	_numVertsPerCol = numVertsPerCol;
	_cellSpacing    = cellSpacing;

	_numCellsPerRow = _numVertsPerRow - 1;
	_numCellsPerCol = _numVertsPerCol - 1;

	_width = _numCellsPerRow * _cellSpacing;
	_depth = _numCellsPerCol * _cellSpacing;

	_numVertices  = _numVertsPerRow * _numVertsPerCol;
	_numTriangles = _numCellsPerRow * _numCellsPerCol * 2;

	_heightScale = heightScale;

	// 加载高度图
	if( !readRawFile(heightmapFileName) )
	{
		::MessageBox(0, _T("readRawFile - FAILED"), 0, 0);
		::PostQuitMessage(0);
	}

	// 高度缩放
	for(unsigned int i = 0; i < _heightmap.size(); i++)
		_heightmap[i] *= heightScale;

	// 计算点点坐标
	if( !computeVertices() )
	{
		::MessageBox(0, _T("computeVertices - FAILED"), 0, 0);
		::PostQuitMessage(0);
	}

	// 计算索引
	if( !computeIndices() )
	{
		::MessageBox(0, _T("computeIndices - FAILED"), 0, 0);
		::PostQuitMessage(0);
	}
	computeNormals();
}

CTerrain::~CTerrain()
{
	d3d::Release<IDirect3DVertexBuffer9*>(_vb);
	d3d::Release<IDirect3DIndexBuffer9*>(_ib);
	d3d::Release<IDirect3DTexture9*>(_tex);
}

int CTerrain::getHeightmapEntry(int row, int col)
{
	return _heightmap[row * _numVertsPerRow + col];
}

void CTerrain::setHeightmapEntry(int row, int col, int value)
{
	_heightmap[row * _numVertsPerRow + col] = value;
}

bool CTerrain::computeVertices()
{
	HRESULT hr = 0;

	/*hr = _device->CreateVertexBuffer(
		_numVertices * sizeof(d3d::TVertex),
		D3DUSAGE_WRITEONLY,
		d3d::TVertex::FVF,
		D3DPOOL_MANAGED,
		&_vb,
		0);*/

	// 这里使用带有法线的顶点格式
	hr = _device->CreateVertexBuffer(
		_numVertices * sizeof(d3d::NTVertex),
		D3DUSAGE_WRITEONLY,
		d3d::NTVertex::FVF,
		D3DPOOL_MANAGED,
		&_vb,
		0);

	if(FAILED(hr))
		return false;

	// coordinates to start generating vertices at
	int startX = -_width / 2;
	int startZ =  _depth / 2;

	// coordinates to end generating vertices at
	int endX =  _width / 2;
	int endZ = -_depth / 2;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	float uCoordIncrementSize = 1.0f / (float)_numCellsPerRow;
	float vCoordIncrementSize = 1.0f / (float)_numCellsPerCol;

	d3d::NTVertex* v = 0;
	_vb->Lock(0, 0, (void**)&v, 0);

	int i = 0;
	for(int z = startZ; z >= endZ; z -= _cellSpacing)
	{
		int j = 0;
		for(int x = startX; x <= endX; x += _cellSpacing)
		{
			// compute the correct index into the vertex buffer and heightmap
			// based on where we are in the nested loop.
			// 法线初始化为0
			int index = i * _numVertsPerRow + j;

			v[index] = d3d::NTVertex(
				(float)x,
				(float)_heightmap[index],
				(float)z,
				0.0f,
				0.0f,
				0.0f,
				(float)j * uCoordIncrementSize,
				(float)i * vCoordIncrementSize);

			j++; // next column
		}
		i++; // next row
	}

	_vb->Unlock();

	return true;
}

bool CTerrain::computeIndices()
{
	HRESULT hr = 0;

	hr = _device->CreateIndexBuffer(
		_numTriangles * 3 * sizeof(WORD), // 3 indices per triangle
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_ib,
		0);

	if(FAILED(hr))
		return false;

	WORD* indices = 0;
	_ib->Lock(0, 0, (void**)&indices, 0);

	// index to start of a group of 6 indices that describe the
	// two triangles that make up a quad
	int baseIndex = 0;

	// loop through and compute the triangles of each quad
	for(int i = 0; i < _numCellsPerCol; i++)
	{
		for(int j = 0; j < _numCellsPerRow; j++)
		{
			indices[baseIndex]     =   i   * _numVertsPerRow + j;
			indices[baseIndex + 1] =   i   * _numVertsPerRow + j + 1;
			indices[baseIndex + 2] = (i+1) * _numVertsPerRow + j;

			indices[baseIndex + 3] = (i+1) * _numVertsPerRow + j;
			indices[baseIndex + 4] =   i   * _numVertsPerRow + j + 1;
			indices[baseIndex + 5] = (i+1) * _numVertsPerRow + j + 1;

			// next quad
			baseIndex += 6;
		}
	}

	_ib->Unlock();

	return true;
}

// 计算法线,均分每个顶点的法线
bool CTerrain::computeNormals()
{
	D3DXVECTOR3 vNormal;

	WORD* vertexCount = new WORD[_numVertices];
	D3DVECTOR* vertexNormalSum = new D3DVECTOR[_numVertices];

	memset(vertexCount, 0, _numVertices*sizeof(WORD));
	memset(vertexNormalSum, 0, _numVertices*sizeof(D3DVECTOR));

	d3d::NTVertex* v = 0;
	WORD* indices = 0;
	_vb->Lock(0, 0, (void**)&v, 0);
	_ib->Lock(0, 0, (void**)&indices, 0);

	 for(DWORD i = 0; i < _numTriangles * 3; i += 3)
    {
        DWORD dwVertex1 = indices[i];
        DWORD dwVertex2 = indices[i + 1];
        DWORD dwVertex3 = indices[i + 2];

		// 计算三角形法线
		d3d::ComputeNormal(&D3DXVECTOR3(v[dwVertex1]._x, 
                                        v[dwVertex1]._y, 
                                        v[dwVertex1]._z), 
                           &D3DXVECTOR3(v[dwVertex2]._x, 
                                        v[dwVertex2]._y, 
                                        v[dwVertex2]._z), 
                           &D3DXVECTOR3(v[dwVertex3]._x, 
                                        v[dwVertex3]._y, 
                                        v[dwVertex3]._z),
										&vNormal);
        
        vertexCount[dwVertex1]++;
        vertexCount[dwVertex2]++;
        vertexCount[dwVertex3]++;

        vertexNormalSum[dwVertex1].x += vNormal.x;
        vertexNormalSum[dwVertex1].y += vNormal.y;
        vertexNormalSum[dwVertex1].z += vNormal.z;
        
        vertexNormalSum[dwVertex2].x += vNormal.x;
        vertexNormalSum[dwVertex2].y += vNormal.y;
        vertexNormalSum[dwVertex2].z += vNormal.z;

        vertexNormalSum[dwVertex3].x += vNormal.x;
        vertexNormalSum[dwVertex3].y += vNormal.y;
        vertexNormalSum[dwVertex3].z += vNormal.z;
    }
	_ib->Unlock();
	
	// 更新顶点坐标法线
	for(int i = 0; i < _numVertices; i++)
    {
        vNormal.x = vertexNormalSum[i].x / vertexCount[i];
        vNormal.y = vertexNormalSum[i].y / vertexCount[i];
        vNormal.z = vertexNormalSum[i].z / vertexCount[i];

        D3DXVec3Normalize(&vNormal, &vNormal);
        
        v[i]._nx = vNormal.x;
        v[i]._ny = vNormal.y;
        v[i]._nz = vNormal.z;
    }
	_vb->Unlock();
	
	delete vertexCount;
	delete vertexNormalSum;
	vertexCount = NULL;
	vertexNormalSum = NULL;

	return true;
}

bool CTerrain::loadTexture(TCHAR* fileName)
{
	HRESULT hr = 0;

	hr = D3DXCreateTextureFromFile(
		_device,
		fileName,
		&_tex);

	if(FAILED(hr))
		return false;

	return true;
}

#if 0
bool CTerrain::genTexture(D3DXVECTOR3* directionToLight)
{
	// Method fills the top surface of a texture procedurally.  Then
	// lights the top surface.  Finally, it fills the other mipmap
	// surfaces based on the top surface data using D3DXFilterTexture.

	HRESULT hr = 0;

	// texel for each quad cell
	int texWidth  = _numCellsPerRow;
	int texHeight = _numCellsPerCol;

	// create an empty texture
	hr = D3DXCreateTexture(
		_device,
		texWidth, texHeight,
		0, // create a complete mipmap chain
		0, // usage
		D3DFMT_X8R8G8B8,// 32 bit XRGB format
		D3DPOOL_MANAGED, &_tex);

	if(FAILED(hr))
		return false;

	D3DSURFACE_DESC textureDesc; 
	_tex->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT lockedRect;
	_tex->LockRect(0/*lock top surface*/, &lockedRect, 
		0 /* lock entire tex*/, 0/*flags*/);         

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(int i = 0; i < texHeight; i++)
	{
		for(int j = 0; j < texWidth; j++)
		{
			D3DXCOLOR c;

			// get height of upper left vertex of quad.
			float height = (float)getHeightmapEntry(i, j) / _heightScale;

			if( (height) < 42.5f ) 		 c = d3d::BEACH_SAND;
			else if( (height) < 85.0f )	 c = d3d::LIGHT_YELLOW_GREEN;
			else if( (height) < 127.5f ) c = d3d::PUREGREEN;
			else if( (height) < 170.0f ) c = d3d::DARK_YELLOW_GREEN;
			else if( (height) < 212.5f ) c = d3d::DARKBROWN;
			else	                     c = d3d::WHITE;

			// fill locked data, note we divide the pitch by four because the
			// pitch is given in bytes and there are 4 bytes per DWORD.
			imageData[i * lockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	if(!lightTerrain(directionToLight))
	{
		::MessageBox(0, _T("lightTerrain() - FAILED"), 0, 0);
		return false;
	}
	
	hr = D3DXFilterTexture(
		_tex,
		0, // default palette
		0, // use top level as source level
		D3DX_DEFAULT); // default filter

	if(FAILED(hr))
	{
		::MessageBox(0, _T("D3DXFilterTexture() - FAILED"), 0, 0);
		return false;
	}

	return true;
}


bool CTerrain::lightTerrain(D3DXVECTOR3* directionToLight)
{
	HRESULT hr = 0;

	D3DSURFACE_DESC textureDesc; 
	_tex->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code that fills the
	// texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
		
	D3DLOCKED_RECT lockedRect;
	_tex->LockRect(
		0,          // lock top surface level in mipmap chain
		&lockedRect,// pointer to receive locked data
		0,          // lock entire texture image
		0);         // no lock flags specified

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(unsigned int i = 0; i < textureDesc.Height; i++)
	{
		for(unsigned int j = 0; j < textureDesc.Width; j++)
		{
			// index into texture, note we use the pitch and divide by 
			// four since the pitch is given in bytes and there are 
			// 4 bytes per DWORD.
			int index = i * lockedRect.Pitch / 4 + j;

			// get current color of quad
			D3DXCOLOR c( imageData[index] );

			// shade current quad
			c *= computeShade(i, j, directionToLight);;

			// save shaded color
			imageData[index] = (D3DCOLOR)c;
		}
	}

	_tex->UnlockRect(0);

	return true;
}

float CTerrain::computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight)
{
	// get heights of three vertices on the quad
	float heightA = getHeightmapEntry(cellRow,   cellCol);
	float heightB = getHeightmapEntry(cellRow,   cellCol+1);
	float heightC = getHeightmapEntry(cellRow+1, cellCol);

	// build two vectors on the quad
	D3DXVECTOR3 u(_cellSpacing, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f,         heightC - heightA, -_cellSpacing);

	// find the normal by taking the cross product of two
	// vectors on the quad.
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	float cosine = D3DXVec3Dot(&n, directionToLight);

	if(cosine < 0.0f)
		cosine = 0.0f;

	return cosine;
}
#endif 

bool CTerrain::readRawFile(std::string fileName)
{
	// Restriction: RAW file dimensions must be >= to the
	// dimensions of the CTerrain.  That is a 128x128 RAW file
	// can only be used with a CTerrain constructed with at most
	// 128x128 vertices.

	// A height for each vertex
	std::vector<BYTE> in( _numVertices );

	std::ifstream inFile(fileName.c_str(), std::ios_base::binary);

	if( inFile == 0 )
		return false;

	// 读入高度信息，每个高度由一个字节表示
	inFile.read(
		(char*)&in[0], // buffer
		in.size());// number of bytes to read into buffer

	inFile.close();

	// copy BYTE vector to int vector
	_heightmap.resize( _numVertices );

	for(unsigned int i = 0; i < in.size(); i++)
		_heightmap[i] = in[i];

	return true;
}

// 获取指定坐标高度，用向量的运算和插值完成
float CTerrain::getHeight(float x, float z)
{
	// Translate on xz-plane by the transformation that takes
	// the CTerrain START point to the origin.

	// 如下图所示：
	//            +z
	//            |
	// startpoint |
    //       (*)--*---*
    //        |   |   |
    //  ------*---*---*------ +x
	//        |   |   |
	//        *---*---*
	//            |
	//            |
	//
	x = ((float)_width / 2.0f) + x;
	z = ((float)_depth / 2.0f) - z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= (float)_cellSpacing;
	z /= (float)_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	float col = ::floorf(x);
	float row = ::floorf(z);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	float A = getHeightmapEntry(row,   col);
	float B = getHeightmapEntry(row,   col+1);
	float C = getHeightmapEntry(row+1, col);
	float D = getHeightmapEntry(row+1, col+1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		height = A + d3d::Lerp(0.0f, uy, dx) + d3d::Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + d3d::Lerp(0.0f, uy, 1.0f - dx) + d3d::Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

bool CTerrain::Render()
{
	HRESULT hr = 0;

	if( _device )
	{	
		// Set texture filters.
		_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		_device->SetStreamSource(0, _vb, 0, sizeof(d3d::NTVertex));
		_device->SetFVF(d3d::NTVertex::FVF);
		_device->SetIndices(_ib);
		
		_device->SetTexture(0, _tex);

		hr =_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			_numVertices,
			0,
			_numTriangles);

		if(FAILED(hr))
			return false;
	}

	return true;
}