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

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の衝突判定
	posA = player_->GetWorldPosition();
	// 自キャラと敵弾すべてのあたり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();
		// AとBの距離を求める
		float distance = Length(posA - posB);
		// 球と球のあたり判定
		if (distance < player_->kRadius + bullet->kRadius) {
			// 自キャラのデスフラグを立てる
			player_->OnCollision();
			// 衝突したら敵弾のデスフラグを立てる
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 敵キャラと自弾の衝突判定
	posA = enemy_->GetWorldPosition();
	// 敵キャラと自弾すべてのあたり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();
		// AとBの距離を求める
		float distance = Length(posA - posB);
		// 球と球のあたり判定
		if (distance < enemy_->kRadius + bullet->kRadius) {
			// 敵キャラのデスフラグを立てる
			enemy_->OnCollision();
			// 衝突したら自弾のデスフラグを立てる
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵弾のあたり判定
	
	for (PlayerBullet* playerBullet : playerBullets) {
		// 自弾の座標
		posA = playerBullet->GetWorldPosition();

		for (EnemyBullet* enemyBullet : enemyBullets) {
			// 敵弾の座標
			posB = enemyBullet->GetWorldPosition();
			// AとBの距離を求める
			float distance = Length(posA - posB);
			// 球と球のあたり判定
			if (distance < playerBullet->kRadius + enemyBullet->kRadius) {
				// 自弾のデスフラグを立てる
				playerBullet->OnCollision();
				// 敵弾のデスフラグを立てる
				enemyBullet->OnCollision();
			}
		}
	}

#pragma endregion
}

void GameScene::Initialize() {

	// ビュープロジェクションの初期化
	// viewProjection_.farZ = 10.0f;
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
	Vector3 enemyPosition = {10.0f, 0.0f, 50.0f};
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

	// あたり判定
	CheckAllCollisions();

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

	// 敵の描画
	enemy_->Draw(viewProjection_);

	// プレイヤーの描画
	player_->Draw(viewProjection_);

	// 描画終了
	Model::PostDraw();
}
