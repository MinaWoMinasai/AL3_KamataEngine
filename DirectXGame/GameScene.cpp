#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	// カメラの解放
	delete debugCamera_;
	// 敵キャラの解放
	delete enemy_;

	// モデルの解放
	delete playerModel_;
	delete enemyModel_;
	delete skydomeModel_;
	delete playerBulletModel_;
	delete enemyBulletModel_;
	delete groundModel_;

	// 衝突マネージャの解放
	delete collisionManager_;

	// 天球の解放
	delete skydome_;
}

void GameScene::Initialize() {

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 1000.0f;
	viewProjection_.Initialize();
	viewProjection_.translation_.y = 25.0f;
	viewProjection_.translation_.z = -40.0f;
	viewProjection_.rotation_.x = 0.5f;

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");
	enemyTextureHandle_ = TextureManager::Load("enemy.png");

	// 3Dモデルの生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	playerBulletModel_ = Model::CreateFromOBJ("playerBullet", true);
	enemyBulletModel_ = Model::CreateFromOBJ("enemyBullet", true);
	groundModel_ = Model::CreateFromOBJ("ground", true);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, textureHandle_, playerBulletModel_);

	// 敵キャラの生成
	enemy_ = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	// 敵キャラの初期化
	Vector3 enemyPosition = {10.0f, 0.0f, 50.0f};
	enemy_->Initialize(enemyModel_, enemyTextureHandle_, enemyPosition, enemyBulletModel_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// シングルトンインスタンス
	input_ = Input::GetInstance();

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションをしていする
	AxisIndicator::GetInstance()->SetTargetCamera(&viewProjection_);
	
	// 衝突マネージャの生成
	collisionManager_ = new CollisionManager();

	// 天球の生成と初期化
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, &viewProjection_);

	// 地面の生成と初期化
	ground_ = new Ground();
	ground_->Initialize(groundModel_, &viewProjection_);
}

void GameScene::Update() {

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();

	// 衝突マネージャの更新
	collisionManager_->CheckAllCollisions(player_, enemy_);

	// 天球の更新
	skydome_->Update();

	// 地面の更新
	ground_->Update();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("rotation", &viewProjection_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translation", &viewProjection_.translation_.x);
	ImGui::End();

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

#ifdef _DEBUG
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

	// 天球の描画
	skydome_->Draw();

	// 地面の描画
	ground_->Draw();

	// 敵の描画
	enemy_->Draw(viewProjection_);

	// プレイヤーの描画
	player_->Draw(viewProjection_);

	// 描画終了
	Model::PostDraw();
}
