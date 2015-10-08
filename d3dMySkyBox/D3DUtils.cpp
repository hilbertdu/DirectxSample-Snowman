#include "D3DUtils.h"

// d3d namespace members

const DWORD d3d::TVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
const DWORD d3d::NTVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

void d3d::BeginRender(IDirect3DDevice9* _d3dDevice9)
{
    _d3dDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
    _d3dDevice9->BeginScene();
}

void d3d::EndRender(IDirect3DDevice9* _d3dDevice9)
{
    _d3dDevice9->EndScene();
    _d3dDevice9->Present(0, 0, 0, 0);
}

D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Ambient = *color * 0.4f;
    light.Diffuse = *color;
    light.Specular = *color * 0.6f;
    light.Direction = *direction;

    return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_POINT;
    light.Ambient = *color * 0.4f;
    light.Diffuse = *color;
    light.Specular = *color * 0.6f;
    light.Position = *position;
    light.Range = 1000.0f;
    light.Falloff = 1.0f;
    light.Attenuation0 = 1.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;

    return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_SPOT;
    light.Ambient = *color * 0.4f;
    light.Diffuse = *color;
    light.Specular = *color * 0.6f;
    light.Position = *position;
    light.Direction = *direction;
    light.Range = 1000.0f;
    light.Falloff = 1.0f;
    light.Attenuation0 = 1.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;
    light.Theta = 0.5f;
    light.Phi = 0.7f;

    return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
    D3DMATERIAL9 mtrl;
    mtrl.Ambient = a;
    mtrl.Diffuse = d;
    mtrl.Specular = s;
    mtrl.Emissive = e;
    mtrl.Power = p;
    return mtrl;
}

d3d::BoundingBox::BoundingBox()
{
    // infinite small 
    _min.x = INFINITY;
    _min.y = INFINITY;
    _min.z = INFINITY;

    _max.x = -INFINITY;
    _max.y = -INFINITY;
    _max.z = -INFINITY;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p)
{
    if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
        p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
    {
        return true;
    }
    else
    {
        return false;
    }
}

d3d::BoundingSphere::BoundingSphere()
{
    _radius = 0.0f;
}

float d3d::GetRandomFloat(float lowBound, float highBound)
{
    if (lowBound >= highBound) // bad input
        return lowBound;

    // get random float in [0, 1] interval
    float f = (rand() % 10000) * 0.0001f;

    // return float in [lowBound, highBound] interval. 
    return (f * (highBound - lowBound)) + lowBound;
}

void d3d::GetRandomVector(
    D3DXVECTOR3* out,
    D3DXVECTOR3* min,
    D3DXVECTOR3* max)
{
    out->x = GetRandomFloat(min->x, max->x);
    out->y = GetRandomFloat(min->y, max->y);
    out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f)
{
    return *((DWORD*)&f);
}

float d3d::Lerp(float a, float b, float t)
{
    return a - (a*t) + (b*t);
}

void d3d::ComputeNormal(D3DXVECTOR3* p0, D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* out)
{
    D3DXVECTOR3 u = *p1 - *p0;
    D3DXVECTOR3 v = *p2 - *p0;

    D3DXVec3Cross(out, &u, &v);
    D3DXVec3Normalize(out, out);
}