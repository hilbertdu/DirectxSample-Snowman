#pragma once

/*
 * D3D Transform变换类，负责坐标变换
 */

#include "D3DUtils.h"
#include "D3DDevice.h"

class CD3DTransform
{
public:
    CD3DTransform(IDirect3DDevice9* device);
    ~CD3DTransform(void);

    void SetViewTransform(float x, float y, float z);
    void SetProjectionTransform(float fovy, float zn, float zf, float width, float height);

    void UpdateViewTransform(D3DXMATRIX* V);

    void SetWorldTransform();
    void SetWorldTransform(D3DXMATRIX* matrix);
    void ResetTransformMatrix();
    void MutliTransformMatrix(D3DXMATRIX* matrix);

    // 平移变换
    void SetTranslation(float x, float y, float z);
    void SetTranslation(D3DXVECTOR3 pos);

    // 旋转变换
    void SetRotation(float ax, float ay, float az, float angle);
    void SetRotation(D3DXVECTOR3 axis, float angle);
    void SetRotationY(float angle);
    void SetRotationX(float angle);
    void SetRotationZ(float angle);

    // 缩放变换
    void SetScaling(float sx, float sy, float sz);
    void SetScaling(D3DXVECTOR3 scale);

    void GetD3DXMatrix(D3DXMATRIX* matrix);

private:
    IDirect3DDevice9*   _device;        // d3d设备指针
    D3DXMATRIX          _transMatrix;   // 变换矩阵
};

