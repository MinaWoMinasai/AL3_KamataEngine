#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

// KamataEngineの名前空間を使用
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	Initialize(L"LE2A_13_ホリケ_ハヤト_AL3_確認課題_03_12_ex1");

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	GameScene* gameScene = new GameScene;

	gameScene->Initialize();

	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// ゲームシーンの更新
		gameScene->Update();

		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		
		// ゲームシーンの描画
		gameScene->Draw();

		// ImGuiの描画
		imguiManager->Draw();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	Finalize();

	// ゲームシーンの解放
	delete gameScene;

	return 0;
}