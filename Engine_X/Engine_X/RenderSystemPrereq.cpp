// RenderSystemPrereq.cpp

#include "stdafx.h"
#include <cassert>
#include "RenderSystemPrereq.h"

#define SAFE_RELEASE(x) if(x) { x->Release(); x=nullptr; }

// -------------------------------------------------------
unsigned int Shader::msIDGen = 0;
unsigned int Texture::msIDGen = 0;
unsigned int IndexBuffer::msIDGen = 0;
unsigned int VertexBuffer::msIDGen = 0;
unsigned int BlendState::msIDGen = 0;
unsigned int DepthState::msIDGen = 0;
unsigned int RasterizerState::msIDGen = 0;

// -------------------------------------------------------

#pragma region Constant
Constant::Constant() 
{
	mSize = 0;
	mData = nullptr;
	mBuffer = nullptr;
}

void Constant::Release() 
{
	mSize = 0;

	if(mData) 
	{
		delete[] mData;
		mData = nullptr;
	}

	SAFE_RELEASE(mBuffer);
}
#pragma endregion

#pragma region Sampler
Sampler::Sampler() 
{
	mState = nullptr;
}

Sampler::Sampler(const Sampler& rhs) 
{
	mState = rhs.mState;
}
void Sampler::operator=(const Sampler& rhs) 
{
	mState = rhs.mState;
}
void Sampler::Release() 
{
	SAFE_RELEASE(mState);
}
#pragma endregion

#pragma region Shader
Shader::Shader() 
{
	mID = msIDGen++;
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mInputLayout = nullptr;
	mSignature = nullptr;

	mNumVSConstants = 0;
	mVSConstants = nullptr;
	mVSDirty = nullptr;

	mNumPSConstants = 0;
	mPSConstants = nullptr;
	mPSDirty = nullptr;

	mNumSamplers = 0;
	mSamplers = nullptr;
}

void Shader::AllocateVSConstants(int count, int size[]) 
{
	mNumVSConstants = count;
	mVSConstants = new Constant[count];
	mVSDirty = new bool[count];
	for(int i = 0; i < count; ++i) 
	{
		mVSConstants[i].mSize = size[i];
		mVSConstants[i].mData = new unsigned char[mVSConstants[i].mSize];
		mVSDirty[i] = false;
	}
}

void Shader::SetVSConstant(int index, int size, void* data) 
{
	assert(index < mNumVSConstants);
	if(mVSConstants[index].mData == nullptr) 
	{
		::memcpy(mVSConstants[index].mData, data, size);
		mVSDirty[index] = true;
	}
	if(::memcmp(mVSConstants[index].mData, data, size) != 0) 
	{
		::memcpy(mVSConstants[index].mData, data, size);
		mVSDirty[index] = true;
	}
}

void Shader::AllocatePSConstants(int count, int size[]) 
{
	mNumPSConstants = count;
	mPSConstants = new Constant[count];
	mPSDirty = new bool[count];
	for(int i = 0; i < count; ++i) 
	{
		mPSConstants[i].mSize = size[i];
		mPSConstants[i].mData = new unsigned char[mPSConstants[i].mSize];
		mPSDirty[i] = false;
	}
}

void Shader::SetPSConstant(int index, int size, void* data) 
{
	assert(index < mNumPSConstants);
	if(mPSConstants[index].mData == nullptr) 
	{
		::memcpy(mPSConstants[index].mData, data, size);
		mPSDirty[index] = true;
	}
	if(::memcmp(mPSConstants[index].mData, data, size) != 0) 
	{
		::memcpy(mPSConstants[index].mData, data, size);
		mPSDirty[index] = true;
	}
}

void Shader::AllocateSamplers(int count) 
{
	mNumSamplers = count;
	mSamplers = new Sampler[mNumSamplers];
}

void Shader::SetSampler(int index, Sampler& sampler) 
{
	assert(index < mNumSamplers);
	mSamplers[index] = sampler;
}

void Shader::Release() 
{
	SAFE_RELEASE(mVertexShader);
	SAFE_RELEASE(mPixelShader);
	SAFE_RELEASE(mInputLayout);
	SAFE_RELEASE(mSignature);

	mNumVSConstants = 0;

	if(mVSConstants) 
	{
		delete[] mVSConstants;
		mVSConstants = nullptr;
	}

	if(mVSDirty) 
	{
		delete[] mVSDirty;
		mVSDirty = nullptr;
	}

	mNumPSConstants = 0;
	if(mPSConstants) 
	{
		delete[] mPSConstants;
		mPSConstants = nullptr;
	}
	if(mPSDirty) 
	{
		delete[] mPSDirty;
		mPSDirty = nullptr;
	}

	mNumSamplers = 0;
	if(mSamplers) 
	{
		delete[] mSamplers;
		mSamplers = nullptr;
	}
}
#pragma endregion

#pragma region Texture
Texture::Texture() 
{
	mID = ++msIDGen;
	mFormat = DXGI_FORMAT_UNKNOWN;
	mResource = nullptr;
	mShaderResourceView = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilView = nullptr;
}

void Texture::Release() 
{
	SAFE_RELEASE(mResource);
	SAFE_RELEASE(mShaderResourceView);
}
#pragma endregion

#pragma region IndexBuffer
IndexBuffer::IndexBuffer() 
{
	mID = ++msIDGen;
	mBuffer = nullptr;
}

void IndexBuffer::Release() 
{
	SAFE_RELEASE(mBuffer);
}
#pragma endregion

#pragma region VertexBuffer
VertexBuffer::VertexBuffer() 
{
	mID = ++msIDGen;  
	mSize = 0;  
	mCount = 0; 
	mRadius = 0.0f;
	mBuffer = nullptr;
}

void VertexBuffer::Release() 
{
	SAFE_RELEASE(mBuffer);
}
#pragma endregion

#pragma region DepthState
DepthState::DepthState() 
{
	mID = ++msIDGen;
	mState = nullptr;
}

void DepthState::Release() 
{
	SAFE_RELEASE(mState);
}
#pragma endregion

#pragma region BlendState
BlendState::BlendState() 
{
	mID = ++msIDGen;
	mState = nullptr;
}

void BlendState::Release() 
{
	SAFE_RELEASE(mState);
}
#pragma endregion

#pragma region RasterizerState
RasterizerState::RasterizerState() 
{
	mID = ++msIDGen;
	mState = nullptr;
}

void RasterizerState::Release() 
{
	SAFE_RELEASE(mState);
}
#pragma endregion

#pragma region Renderable
Renderable::Renderable() 
{
	mCount = 0;
}

void Renderable::Release() 
{
	mIndexBuffer.Release();
	mVertexBuffer.Release();
}
#pragma endregion

// ------------------------------------------------------------------------
RenderState::RenderState() 
{
	Reset();
}

void RenderState::Reset() 
{
	mShader = nullptr;
	for(int i = 0; i < 8; ++i) { mTexture[i] = nullptr; };
	mIndexBuffer = nullptr;
	for(int i = 0; i < 4; ++i) { mVertexBuffer[i] = nullptr; }
	mDepthState = nullptr;
	mBlendState = nullptr;
	mRasterizerState = nullptr;
}
