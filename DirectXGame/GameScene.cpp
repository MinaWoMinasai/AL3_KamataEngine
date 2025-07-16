#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	delete debugCamera_;
	delete enemy_;
}

void GameScene::Initialize() {

	// ビュープロジェクションの初期化
	//viewProjection_.farZ = 10.0f;
	viewProjection_.Initialize();
	
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");
	enemyTextureHandle_ = TextureManager::Load("enemy.png");

	// 3Dモデルの生成
	playerModel_ = Model::Create();
	enemyModel_ = Model::Create();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, textureHandle_);

	// 敵キャラの生成
	enemy_ = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	// 敵キャラの初期化
	Vector3 enemyPosition = {0.0f, 5.0f, 20.0f};
	enemy_->Initialize(enemyModel_, enemyTextureHandle_, enemyPosition);
	
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// シングルトンインスタンス
	input_ = Input::GetInstance();

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションをしていする
	AxisIndicator::GetInstance()->SetTargetCamera(&viewProjection_);
}

void GameScene::Update() {

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();

	if (isDebugCameraActive_) {
		debugCamera_->Update();

		viewProjection_.matView = debugCamera_->GetCamera().matView;
		viewProjection_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {

		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();
	}

#ifdef  _DEBUG
	if (input_->TriggerKey(DIK_LSHIFT)) {
		isDebugCameraActive_ = true;
	}
#endif


}

void GameScene::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	// 敵の描画
	enemy_->Draw(viewProjection_);

	// プレイヤーの描画
	player_->Draw(viewProjection_);
	
	// 描画終了
	Model::PostDraw();

}
