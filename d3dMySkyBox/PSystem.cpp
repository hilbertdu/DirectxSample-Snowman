#include <cstdlib>
#include "pSystem.h"

using namespace psys;

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CPSystem::CPSystem()
{
    _device = 0;
    _vb = 0;
    _tex = 0;
}

CPSystem::~CPSystem()
{
    d3d::Release<IDirect3DVertexBuffer9*>(_vb);
    d3d::Release<IDirect3DTexture9*>(_tex);
}

bool CPSystem::init(IDirect3DDevice9* device, TCHAR* texFileName)
{
    _device = device;

    HRESULT hr = 0;
    hr = device->CreateVertexBuffer(
        _vbSize * sizeof(Particle),
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
        Particle::FVF,
        D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
        &_vb,
        0);

    if (FAILED(hr))
    {
        ::MessageBox(0, _T("CreateVertexBuffer() - FAILED"), _T("PSystem"), 0);
        return false;
    }

    hr = D3DXCreateTextureFromFile(
        device,
        texFileName,
        &_tex);

    if (FAILED(hr))
    {
        ::MessageBox(0, _T("D3DXCreateTextureFromFile() - FAILED"), _T("PSystem"), 0);
        return false;
    }

    return true;
}

void CPSystem::reset()
{
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        resetParticle(&(*i));
    }
}

void CPSystem::addParticle()
{
    Attribute attribute;

    resetParticle(&attribute);

    _particles.push_back(attribute);
}

void CPSystem::preRender()
{
    _device->SetRenderState(D3DRS_LIGHTING, false);
    _device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    _device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
    _device->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(_size));
    _device->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f));

    // control the size of the particle relative to distance
    _device->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f));
    _device->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
    _device->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f));

    // use alpha from texture
    _device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    _device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    _device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    _device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CPSystem::postRender()
{
    _device->SetRenderState(D3DRS_LIGHTING, true);
    _device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
    _device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
    _device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CPSystem::Render()
{
    // 采用分片渲染的方式，顶点缓存分为多个片，每次拷贝需要渲染的数据到一个片中，
    // 渲染该片的同时，拷贝下一个片段的数据，这种方式充分挖掘了并行性，使得GPU和CPU并行地工作

    if (!_particles.empty())
    {
        // 设置相关的渲染模式
        preRender();

        _device->SetTexture(0, _tex);
        _device->SetFVF(Particle::FVF);
        _device->SetStreamSource(0, _vb, 0, sizeof(Particle));

        // render batches one by one
        // start at beginning if we're at the end of the vb
        if (_vbOffset >= _vbSize)
            _vbOffset = 0;

        Particle* v = 0;

        _vb->Lock(
            _vbOffset    * sizeof(Particle),
            _vbBatchSize * sizeof(Particle),
            (void**)&v,
            _vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

        DWORD numParticlesInBatch = 0;

        // Until all particles have been rendered.
        std::list<Attribute>::iterator i;
        for (i = _particles.begin(); i != _particles.end(); i++)
        {
            // 拷贝所有活跃顶点的数据到缓存
            if (i->_isAlive)
            {
                // Copy a batch of the living particles to the
                // next vertex buffer segment
                v->_position = i->_position;
                v->_color = (D3DCOLOR)i->_color;
                v++; // next element;

                numParticlesInBatch++; //increase batch counter

                // if this batch full?（如果一个片段满了就绘制）
                if (numParticlesInBatch == _vbBatchSize)
                {
                    // Draw the last batch of particles that was
                    // copied to the vertex buffer. 
                    _vb->Unlock();

                    _device->DrawPrimitive(
                        D3DPT_POINTLIST,
                        _vbOffset,
                        _vbBatchSize);

                    // While that batch is drawing, start filling the
                    // next batch with particles.

                    // move the offset to the start of the next batch(开始拷贝下一个片段的数据)
                    _vbOffset += _vbBatchSize;

                    // don't offset into memory thats outside the vb's range.
                    // If we're at the end, start at the beginning.
                    if (_vbOffset >= _vbSize)
                        _vbOffset = 0;

                    _vb->Lock(
                        _vbOffset    * sizeof(Particle),
                        _vbBatchSize * sizeof(Particle),
                        (void**)&v,
                        _vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

                    numParticlesInBatch = 0; // reset for new batch
                }
            }
        }

        _vb->Unlock();

        // its possible that the LAST batch being filled never 
        // got rendered because the condition 
        // (numParticlesInBatch == _vbBatchSize) would not have
        // been satisfied.  We draw the last partially filled batch now.
        // 绘制最后一个未满的片段（如果有）
        if (numParticlesInBatch)
        {
            _device->DrawPrimitive(
                D3DPT_POINTLIST,
                _vbOffset,
                numParticlesInBatch);
        }

        // next block
        _vbOffset += _vbBatchSize;

        // reset render states
        postRender();
    }
}

bool CPSystem::isEmpty()
{
    return _particles.empty();
}

bool CPSystem::isDead()
{
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        // is there at least one living particle?  If yes,
        // the system is not dead.
        if (i->_isAlive)
            return false;
    }
    // no living particles found, the system must be dead.
    return true;
}

void CPSystem::removeDeadParticles()
{
    std::list<Attribute>::iterator i;

    i = _particles.begin();

    while (i != _particles.end())
    {
        if (i->_isAlive == false)
        {
            // erase returns the next iterator, so no need to
            // incrememnt to the next one ourselves.
            i = _particles.erase(i);
        }
        else
        {
            i++; // next in list
        }
    }
}

//*****************************************************************************
// Snow System
//***************

CSnow::CSnow(d3d::BoundingBox* boundingBox, int numParticles)
{
    _boundingBox = *boundingBox;
    _size = 0.25f;
    _vbSize = 2048;			// vertex buffer的大小
    _vbOffset = 0;
    _vbBatchSize = 512;			// 每一个片段的大小为512

    for (int i = 0; i < numParticles; i++)
        addParticle();
}

void CSnow::resetParticle(Attribute* attribute)
{
    attribute->_isAlive = true;

    // 在boundingbox中随机生成初始坐标
    d3d::GetRandomVector(
        &attribute->_position,
        &_boundingBox._min,
        &_boundingBox._max);

    // 总是从顶部出生
    attribute->_position.y = _boundingBox._max.y;

    // 随机模拟一个速度偏移
    attribute->_velocity.x = d3d::GetRandomFloat(0.0f, 1.0f) * -3.0f;
    attribute->_velocity.y = d3d::GetRandomFloat(0.0f, 1.0f) * -10.0f;
    attribute->_velocity.z = 0.0f;

    // white snow flake
    attribute->_color = d3d::WHITE;
}

void CSnow::update(float timeDelta)
{
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        i->_position += i->_velocity * timeDelta;

        // 顶点是否在指定范围中
        if (_boundingBox.isPointInside(i->_position) == false)
        {
            // 重置这些死亡的粒子
            resetParticle(&(*i));
        }
    }
}
