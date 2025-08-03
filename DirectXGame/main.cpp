#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// シーン(型)
enum class Scene {

	kUnknown = 0,

	kTitle,
	kGame,
};

// ゲームシーンのインスタンスを生成
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

// 現在シーン(型)
Scene scene = Scene::kUnknown;

// シーン切り替え
void ChangeScene();

// 現在シーンの更新
void UpdateScene();

// シーンの描画
void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	Initialize(L"LE2A_13_ホリケ_ハヤト_トツゲキック");

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;

	// タイトルのインスタンスを生成
	titleScene = new TitleScene();

	// タイトルの初期化
	titleScene->Initialize();

#ifdef _DEBUG
	scene = Scene::kGame;
	gameScene = new GameScene();
	gameScene->Initialize();
#endif

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}
		
		// 現在シーンの更新
		UpdateScene();
		// シーン切り替え
		ChangeScene();
		

		// 描画開始
		dxCommon->PreDraw();

		// 現在シーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	Finalize();

	// ゲームシーンの解放
	delete gameScene;
	// タイトルシーンの解放
	delete titleScene;
	// nullpttrの代入
	gameScene = nullptr;

	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}
