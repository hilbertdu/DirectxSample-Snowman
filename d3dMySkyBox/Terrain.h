#pragma once

/*
 * d3d地形类，负责地形的初始化，渲染，本例中计算了地形的法线
 */

#include "d3dUtils.h"
#include <string>
#include <vector>

class CTerrain
{
public:
	CTerrain(
		IDirect3DDevice9* device,
		std::string heightmapFileName,	// 高度图文件
		int numVertsPerRow,				// 行顶点个数
		int numVertsPerCol,				// 列顶点个数
		int cellSpacing,				// 单位方格的大小
		float heightScale				// 高度比例系数
		);   

	~CTerrain();

	int  getHeightmapEntry(int row, int col);
	void setHeightmapEntry(int row, int col, int value);

	float getHeight(float x, float z);

	bool  loadTexture(TCHAR* fileName);
	//bool  genTexture(D3DXVECTOR3* directionToLight);
	bool  Render();

private:
	IDirect3DDevice9*       _device;
	IDirect3DTexture9*      _tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;

	int _numVertsPerRow;
	int _numVertsPerCol;
	int _cellSpacing;

	int _numCellsPerRow;		// 行单元格个数
	int _numCellsPerCol;		// 列单元格个数
	int _width;				
	int _depth;
	int _numVertices;
	int _numTriangles;

	float _heightScale;

	std::vector<int> _heightmap;

	// helper methods
	bool readRawFile(std::string fileName);
	bool computeVertices();
	bool computeIndices();
	bool computeNormals();
	//bool  lightTerrain(D3DXVECTOR3* directionToLight);
	//float computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);
};