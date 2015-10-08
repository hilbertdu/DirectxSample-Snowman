#include "D3DTransform.h"


CD3DTransform::CD3DTransform(IDirect3DDevice9* device)
{
    _device = device;

    // 初始化为单位矩阵
    D3DXMatrixIdentity(&_transMatrix);
}

CD3DTransform::~CD3DTransform(void)
{
}

void CD3DTransform::SetWorldTransform()
{
    _device->SetTransform(D3DTS_WORLD, &_transMatrix);
}

void CD3DTransform::SetWorldTransform(D3DXMATRIX* matrix)
{
    _device->SetTransform(D3DTS_WORLD, matrix);
}

void CD3DTransform::MutliTransformMatrix(D3DXMATRIX* matrix)
{
    _transMatrix *= *matrix;
}

void CD3DTransform::ResetTransformMatrix()
{
    D3DXMatrixIdentity(&_transMatrix);
}

void CD3DTransform::SetTranslation(D3DXVECTOR3 pos)
{
    SetTranslation(pos.x, pos.y, pos.z);
}

void CD3DTransform::SetScaling(D3DXVECTOR3 pos)
{
    SetScaling(pos.x, pos.y, pos.z);
}

void CD3DTransform::SetRotation(D3DXVECTOR3 axis, float angle)
{
    D3DXMATRIX world;
    D3DXMatrixRotationAxis(&world, &axis, angle);
    _transMatrix *= world;
}

void CD3DTransform::SetTranslation(float x, float y, float z)
{
    D3DXMATRIX world;
    D3DXMatrixTranslation(&world, x, y, z);
    _transMatrix *= world;
}

void CD3DTransform::SetScaling(float sx, float sy, float sz)
{
    D3DXMATRIX world;
    D3DXMatrixScaling(&world, sx, sy, sz);
    _transMatrix *= world;
}

void CD3DTransform::SetRotation(float ax, float ay, float az, float angle)
{
    D3DXVECTOR3 axis(ax, ay, az);
    SetRotation(axis, angle);
}

void CD3DTransform::SetRotationY(float angle)
{
    D3DXMATRIX world;
    D3DXMatrixRotationY(&world, angle);
    _transMatrix *= world;
}

void CD3DTransform::SetViewTransform(float x, float y, float z)
{
    D3DXVECTOR3 pos(x, y, z);
    D3DXVECTOR3 target(0.0f, 0.0f, 10.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    _device->SetTransform(D3DTS_VIEW, &V);
}

void CD3DTransform::UpdateViewTransform(D3DXMATRIX* V)
{
    _device->SetTransform(D3DTS_VIEW, V);
}

void CD3DTransform::SetProjectionTransform(float fovy, float zn, float zf, float width, float height)
{
    // Set the projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj,
                                fovy,
                                width / height,
                                zn,
                                zf);
    _device->SetTransform(D3DTS_PROJECTION, &proj);
}

void CD3DTransform::GetD3DXMatrix(D3DXMATRIX* matrix)
{
    *matrix = _transMatrix;
}