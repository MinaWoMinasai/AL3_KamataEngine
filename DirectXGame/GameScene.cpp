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
	delete modelBlock_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
	delete mapChipField_;
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

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("Player.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("Player", true);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// ブロックの3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// カメラの初期化
	camera_.farZ = 100;
	camera_.Initialize();

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_, playerPosition);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GeneratteBlocks();

	// デバッグカメラの初期化
	debugCamera_ = new DebugCamera(1280, 720);

}

// 更新
void GameScene::Update() {

	player_->Update();

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

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
#endif

}

// 描画
void GameScene::Draw() {

	// プレイヤーの描画
	player_->Draw();

	// 天球の描画
	skydome_->Draw();

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
			modelBlock_->Draw(*worldTransformBlock, debugCamera_->GetCamera());
		}
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

}
