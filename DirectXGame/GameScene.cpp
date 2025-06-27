#include "GameScene.h"
#include "ImGui.h"
#include "KamataEngine.h"
#include <Windows.h>
using namespace KamataEngine;
using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete modelSkydome_;
	delete player_;
	for (Enemy*& enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete modelBlock_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
	delete mapChipField_;
	delete cameraContoroller_;
	delete deathParticles_;
	delete fade_;
}

void GameScene::GeneratteBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	
	// 要素数を変更する
	// 列数を設定(縦設定のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheakAllCollisions() {

	// 自キャラと敵キャラのあたり判定
	
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾すべてのあたり判定
	for (Enemy* enemy : enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の当たり判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(enemy);
			// 敵キャラの衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}
}

void GameScene::ChengePhase() {

	switch (phase_) {
	case GameScene::Phase::kFadeIn:

		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPley;
		}

		break;

	case GameScene::Phase::kPley:

		// プレイヤーがデスしたか
		if (player_->IsDead()) {
		
			// 死亡演出に切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの位置にデスパーティクルを生成
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);

		}

		break;
	case GameScene::Phase::kDeath:
		break;

	case Phase::kFadeOut:
		
		if (fade_->IsFinished()) {
			finished_ = true;
			fade_->Stop();
		}
		fade_->Update();

		break;
	}
}

void GameScene::Initialize() {

	// ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("Player", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelEnemy_ = Model::CreateFromOBJ("Player", true);
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true); 
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// カメラの初期化
	camera_.farZ = 1000.0f;
	camera_.Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GeneratteBlocks();

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 12);

	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition);

	player_->SetMapChipField(mapChipField_);

	// 敵の生成と初期化
	for (int32_t i = 0; i < kEnemyCount; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(25, 13 + i * 2);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// デスパーティクルを生成
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelDeathParticle_, &camera_, playerPosition);

	// デバッグカメラの初期化
	debugCamera_ = new DebugCamera(1280, 720);

	// カメラコントローラの初期化
	cameraContoroller_ = new CameraContoroller();
	cameraContoroller_->Initialize(&camera_);
	cameraContoroller_->SetTarget(player_);
	cameraContoroller_->Reset();
	cameraContoroller_->SetMovebleArea({22, 100, 12, 100});
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, kDuration);
}

// 更新
void GameScene::Update() {

	ChengePhase();

	switch (phase_) {

	case GameScene::Phase::kPley:
	case GameScene::Phase::kFadeIn:
		
		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// 敵の更新
		for (Enemy*& enemy : enemies_) {
			enemy->Update();
		}

		// カメラコントローラの更新
		cameraContoroller_->Update();

		// カメラの更新
		if (isDebugCameraActive_) {
			debugCamera_->Update();

			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {

			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			camera_.TransferMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransformBlock->scale_);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransformBlock->translation_);
				Matrix4x4 rotateXMatrix = MakeRotateXMatrix(worldTransformBlock->rotation_.x);
				Matrix4x4 rotateYMatrix = MakeRotateYMatrix(worldTransformBlock->rotation_.y);
				Matrix4x4 rotateZMatrix = MakeRotateZMatrix(worldTransformBlock->rotation_.z);
				Matrix4x4 affineMatrix = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

				worldTransformBlock->matWorld_ = affineMatrix;

				// 定数バッファに転送
				worldTransformBlock->TransferMatrix();
			}
		}

		// 当たり判定の更新
		CheakAllCollisions();

		break;
	case GameScene::Phase::kDeath:
	case GameScene::Phase::kFadeOut:
		
		// ゲームシーン終了
		if (deathParticles_ && deathParticles_->IsFinished()) {
			if (phase_ == Phase::kDeath) {
				fade_->Start(Fade::Status::FadeOut, kDuration);
				phase_ = Phase::kFadeOut;
			}
		}

		// 天球の描画
		skydome_->Update();

		// 敵の更新
		for (Enemy*& enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		deathParticles_->Update();

		// カメラの更新
		if (isDebugCameraActive_) {
			debugCamera_->Update();

			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {

			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
			camera_.TransferMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransformBlock->scale_);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransformBlock->translation_);
				Matrix4x4 rotateXMatrix = MakeRotateXMatrix(worldTransformBlock->rotation_.x);
				Matrix4x4 rotateYMatrix = MakeRotateYMatrix(worldTransformBlock->rotation_.y);
				Matrix4x4 rotateZMatrix = MakeRotateZMatrix(worldTransformBlock->rotation_.z);
				Matrix4x4 affineMatrix = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

				worldTransformBlock->matWorld_ = affineMatrix;

				// 定数バッファに転送
				worldTransformBlock->TransferMatrix();
			}
		}

		break;
	
	}


#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
#endif

}

// 描画
void GameScene::Draw() {

	// 敵の描画
	for (Enemy*& enemy : enemies_) {
		enemy->Draw();
	}
	
	// 天球の描画
	skydome_->Draw();

	// プレイヤーの描画
	if (player_->IsDead()) {
		// デスパーティクルを描画
		deathParticles_->Draw();
	} else {
		// プレイヤーの描画
		player_->Draw();
	}

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	//	ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
	
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}
