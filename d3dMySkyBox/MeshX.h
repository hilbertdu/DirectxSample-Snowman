#pragma once

/*
 * d3d网格对象类，负责读取x文件创建网格对象，绘制网格
 */

#include "D3DUtils.h"
#include <vector>

class CMeshX
{
public:
    CMeshX(void);
    ~CMeshX(void);

    CMeshX(IDirect3DDevice9* device, TCHAR* xfilename, TCHAR* parentpath);

    void SetPosition(float x, float y, float z);
    void SetScaling(float sx, float sy, float sz);
    void SetRotationY(float angle);

    D3DXVECTOR3 GetPosition();
    D3DXVECTOR3 GetScaling();
    float GetRotationY();

    bool AddNormals();
    bool Render();

    ID3DXMesh* GetID3DXMesh();

private:
    IDirect3DDevice9* _device;

    ID3DXMesh*                      _Mesh;
    std::vector<D3DMATERIAL9>       _Mtrls;
    std::vector<IDirect3DTexture9*> _Textures;

    D3DXVECTOR3  _pos;
    D3DXVECTOR3  _scale;
    float        _angley;

    ID3DXBuffer* _adjBuffer;
    ID3DXBuffer* _mtrlBuffer;
    DWORD        _numMtrls;

    TCHAR        _xfileDir[255];
};

