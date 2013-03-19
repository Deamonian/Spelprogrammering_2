#pragma once

static const int DEFAULT_WIDTH = 1280;
static const int DEFAULT_HEIGHT = 720;

#include "Camera.h"

class MessageSystem;
class RenderSystem;
class ShaderManager;
class TextureManager;
class SceneManager;

class Game
{
public:
	Game(void);
	~Game(void);

	bool Initialize(MessageSystem* msgsys, HWND hWnd, ConfigFile cfg);
	void Deinitialize();

	void Update();

	void OnKeyboard(IEvent* event);

protected:
	MessageSystem* mMessageSystem;
	RenderSystem* mRenderSystem;

	ShaderManager* mShaderManager;
	TextureManager* mTextureManager;
	SceneManager* mSceneManager;

	Timer mTimer;
	Camera mCamera;
	class CameraControl* mCameraControl;
};

