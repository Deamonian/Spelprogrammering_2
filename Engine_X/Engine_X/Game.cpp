#include "stdafx.h"
#include "Game.h"
#include "MessageSystem.h"
#include "RenderSystem.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Skydome.h"
#include "CameraControl.h"


Game::Game(void)
{
	mMessageSystem = nullptr;
	mRenderSystem = nullptr;
	mShaderManager = nullptr;
	mTextureManager = nullptr;
	mSceneManager = nullptr;
}

Game::~Game(void)
{
}

bool Game::Initialize(MessageSystem* msgsys, HWND hWnd, ConfigFile cfg)
{
	// Initialize all systems here, except for the message system
	
	float width = cfg.HasKey("width") ? cfg.AsFloat("width") : DEFAULT_WIDTH;
	float height = cfg.HasKey("height") ? cfg.AsFloat("height") : DEFAULT_HEIGHT;

	mMessageSystem = msgsys;

	mRenderSystem = new RenderSystem;

	if(!mRenderSystem->Init((int)width, (int)height, hWnd)) 
	{
		System::Notify("Could not initialize RenderSystem!\n"
			"Read log.txt for more information.","Error!");
		return false;
	}

	mMessageSystem->Register(EVT_KEYBOARD, this, &Game::OnKeyboard);

	mShaderManager = new ShaderManager(mRenderSystem);
	mTextureManager = new TextureManager(mRenderSystem);
	mSceneManager = new SceneManager(mShaderManager, mTextureManager);
	mSceneManager->Init();

	// set up camera
	mCamera.SetPerspective(width / height, DirectX::XM_PIDIV4, cfg.AsFloat("c_znear"), cfg.AsFloat("c_zfar"));
	mCameraControl = new CameraControl(&mCamera);
	mCameraControl->Init(mMessageSystem);
	mCameraControl->Settings(cfg.AsFloat("c_speed"), cfg.AsFloat("m_yaw"), cfg.AsFloat("m_pitch"), cfg.AsFloat("m_sensitivity"));

	// skydome
	Skydome* skydome = new Skydome;
	skydome->Init(mRenderSystem);
	mSceneManager->SetSkydome(skydome);
	
	/*
		RenderSystem,
			Shader handling,
			
		Scene handling

		Entity handling?

		Camera stuff
	*/

	return true;
}

void Game::Deinitialize()
{
	// Deinitialize everything, EXCEPT FOR THE MESSAGE SYSTEM!

	mMessageSystem->Unregister(EVT_KEYBOARD, this);

	if(mCameraControl) 
	{
		mCameraControl->Shut(mMessageSystem);
		delete mCameraControl;
		mCameraControl = nullptr;
	};

	if(mTextureManager) 
	{
		delete mTextureManager;
		mTextureManager = nullptr;
	};

	if(mShaderManager) 
	{
		delete mShaderManager;
		mShaderManager=nullptr;
	};

	if(mSceneManager) 
	{
		mSceneManager->Shut();
		delete mSceneManager;
		mSceneManager=nullptr;
	};
	
	if(mRenderSystem)
	{
		mRenderSystem->Shut();
		delete mRenderSystem;
		mRenderSystem = nullptr;
	}
}

void Game::Update()
{
	// Update logic goes here!
	mTimer.Update();
	mCameraControl->Update(mTimer.GetDelta());

	mRenderSystem->Clear();

	// Render logic sandwiched here!
	mSceneManager->Render(mRenderSystem, &mCamera);

	mRenderSystem->Present();
}


void Game::OnKeyboard(IEvent* event)
{
	KeyboardEvent* evt = static_cast<KeyboardEvent*>(event);
	if(evt->mKeycode == KC_Escape)	
	{
		PostQuitMessage(0);
	}
}