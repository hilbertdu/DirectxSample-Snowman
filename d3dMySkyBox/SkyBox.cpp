#include "SkyBox.h"

CSkyBox::CSkyBox(IDirect3DDevice9* pDevice)
{
    _d3dDevice = pDevice;
    _vb = NULL;
    _ib = NULL;
    memset(_texture, NULL, sizeof(_texture));
}

CSkyBox::~CSkyBox(void)
{
    d3d::Release<IDirect3DVertexBuffer9*>(_vb);
    d3d::Release<IDirect3DIndexBuffer9*>(_ib);
}

bool CSkyBox::InitSkyBox(int scale)
{
    // 创建顶点坐标
    if (FAILED(_d3dDevice->CreateVertexBuffer(
        20 * sizeof(d3d::TVertex),
        D3DUSAGE_WRITEONLY,
        d3d::TVertex::FVF,
        D3DPOOL_MANAGED,
        &_vb,
        0)))
    {
        return false;
    }

    d3d::TVertex* v = 0;
    _vb->Lock(0, 0, (void**)&v, 0);

    // positive x
    v[0] = d3d::TVertex(1.0f*scale, -1.0f*scale, 1.0f*scale, 0.0f, 1.0f);
    v[1] = d3d::TVertex(1.0f*scale, 1.0f*scale, 1.0f*scale, 0.0f, 0.0f);
    v[2] = d3d::TVertex(1.0f*scale, 1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
    v[3] = d3d::TVertex(1.0f*scale, -1.0f*scale, -1.0f*scale, 1.0f, 1.0f);

    // negative x
    v[4] = d3d::TVertex(-1.0f*scale, -1.0f*scale, -1.0f*scale, 0.0f, 1.0f);
    v[5] = d3d::TVertex(-1.0f*scale, 1.0f*scale, -1.0f*scale, 0.0f, 0.0f);
    v[6] = d3d::TVertex(-1.0f*scale, 1.0f*scale, 1.0f*scale, 1.0f, 0.0f);
    v[7] = d3d::TVertex(-1.0f*scale, -1.0f*scale, 1.0f*scale, 1.0f, 1.0f);

    // positive y
    v[8] = d3d::TVertex(-1.0f*scale, 1.0f*scale, 1.0f*scale, 0.0f, 1.0f);
    v[9] = d3d::TVertex(-1.0f*scale, 1.0f*scale, -1.0f*scale, 0.0f, 0.0f);
    v[10] = d3d::TVertex(1.0f*scale, 1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
    v[11] = d3d::TVertex(1.0f*scale, 1.0f*scale, 1.0f*scale, 1.0f, 1.0f);

    // positive z
    v[12] = d3d::TVertex(-1.0f*scale, -1.0f*scale, 1.0f*scale, 0.0f, 1.0f);
    v[13] = d3d::TVertex(-1.0f*scale, 1.0f*scale, 1.0f*scale, 0.0f, 0.0f);
    v[14] = d3d::TVertex(1.0f*scale, 1.0f*scale, 1.0f*scale, 1.0f, 0.0f);
    v[15] = d3d::TVertex(1.0f*scale, -1.0f*scale, 1.0f*scale, 1.0f, 1.0f);

    // negative z
    v[16] = d3d::TVertex(1.0f*scale, -1.0f*scale, -1.0f*scale, 0.0f, 1.0f);
    v[17] = d3d::TVertex(1.0f*scale, 1.0f*scale, -1.0f*scale, 0.0f, 0.0f);
    v[18] = d3d::TVertex(-1.0f*scale, 1.0f*scale, -1.0f*scale, 1.0f, 0.0f);
    v[19] = d3d::TVertex(-1.0f*scale, -1.0f*scale, -1.0f*scale, 1.0f, 1.0f);

    _vb->Unlock();

    // 创建索引
    if (FAILED(_d3dDevice->CreateIndexBuffer(
        30 * sizeof(WORD),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &_ib,
        0)))
    {
        return false;
    }

    WORD* indices = 0;
    _ib->Lock(0, 0, (void**)&indices, 0);

    // positive x
    indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
    indices[3] = 0;	indices[4] = 2;	indices[5] = 3;

    // negative x
    indices[6] = 4;  indices[7] = 5;  indices[8] = 6;
    indices[9] = 4;  indices[10] = 6;  indices[11] = 7;

    // positive y
    indices[12] = 8;  indices[13] = 9;  indices[14] = 10;
    indices[15] = 8;  indices[16] = 10; indices[17] = 11;

    // positive z
    indices[18] = 12; indices[19] = 13; indices[20] = 14;
    indices[21] = 12; indices[22] = 14; indices[23] = 15;

    // negative z
    indices[24] = 16; indices[25] = 17; indices[26] = 18;
    indices[27] = 16; indices[28] = 18; indices[29] = 19;

    _ib->Unlock();

    return true;
}

bool CSkyBox::SetTexture(const TCHAR *TextureFile, int flag)
{
    if (FAILED(D3DXCreateTextureFromFile(
        _d3dDevice,
        TextureFile,
        &_texture[flag])))
    {
        ::MessageBox(0, _T(""), 0, 0);
        return false;
    }
    return true;
}

void CSkyBox::Render()
{
    // 设置过滤器
    _d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    _d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    _d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // 设置寻址模式
    _d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    _d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // 不关闭光照
    /*DWORD lightState;
    _d3dDevice->GetRenderState(D3DRS_LIGHTING, &lightState);
    _d3dDevice->SetRenderState(D3DRS_LIGHTING, false);*/

    _d3dDevice->SetStreamSource(0, _vb, 0, sizeof(d3d::TVertex));
    _d3dDevice->SetFVF(d3d::TVertex::FVF);
    _d3dDevice->SetIndices(_ib); // 设置索引缓存

    // 顺序是：右->左->上->前->后
    for (int i = 0; i < 5; ++i)
    {
        _d3dDevice->SetTexture(0, _texture[i]);
        _d3dDevice->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            0,   // 将要绘制的索引缓冲区的起始地址
            i * 4, // 索引数组中最小的索引值
            4,   // 要绘制的索引数组中的顶点数量
            i * 6, // 从索引数组中的第几个元素开始绘制图元
            2);  // 绘制的图元数量
    }

    //_d3dDevice->SetRenderState(D3DRS_LIGHTING, lightState);
}