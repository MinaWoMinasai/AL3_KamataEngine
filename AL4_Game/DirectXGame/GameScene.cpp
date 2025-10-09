#include "GameScene.h"

using namespace KamataEngine;
using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	delete debugCamera_;
	delete enemy_;
	delete playerModel_;
	delete enemyModel_;

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
	modelBlock_ = std::unique_ptr<Model>(Model::Create());

	mapChip_ = std::make_unique<MapChip>();
	mapChip_->LoadMapChipCsv("Resources/map.csv");
	GeneratteBlocks();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, textureHandle_);

	// 敵キャラの生成
	enemy_ = new Enemy();
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
	player_->Update(viewProjection_);

	// 敵の更新
	enemy_->Update();

	// 当たり判定
	CheckCollisionPlayerAndBlocks();

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

	// ブロックの更新
	for (auto& line : blocks_) {
		for (auto& block : line) {
			if (!block.worldTransform)
				continue;
				WorldTransformUpdate(*block.worldTransform);
		}
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

	//	ブロックの描画
	for (auto& line : blocks_) {
		for (auto& block : line) {
			if (!block.worldTransform)
				continue;
			modelBlock_->Draw(*block.worldTransform, viewProjection_);
		}
	}

	// 敵の描画
	enemy_->Draw(viewProjection_);

	// プレイヤーの描画
	player_->Draw(viewProjection_);

	// 描画終了
	Model::PostDraw();
}

void GameScene::GeneratteBlocks() {

	uint32_t numBlockVirtical = mapChip_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChip_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦設定のブロック数)
	blocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		// 1列の要素数を設定(横方向のブロック数)
		blocks_[i].resize(numBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			if (mapChip_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				// 実体に直接アクセスして代入できる
				Block& block = blocks_[i][j];

				block.worldTransform = std::make_unique<KamataEngine::WorldTransform>();
				block.worldTransform->Initialize();

				// 座標設定
				block.worldTransform->translation_ = mapChip_->GetMapChipPositionByIndex(j, i);

				// 中心座標
				const auto& pos = block.worldTransform->translation_;

				// AABB設定
				block.aabb.min = {pos.x - mapChip_->kBlockWidth / 2.0f, pos.y - mapChip_->kBlockHeight / 2.0f, pos.z - mapChip_->kBlockHeight / 2.0f};
				block.aabb.max = {pos.x + mapChip_->kBlockWidth / 2.0f, pos.y + mapChip_->kBlockHeight / 2.0f, pos.z + mapChip_->kBlockHeight / 2.0f};
			} else {
				// 空白セルはnullptrにしておく
				blocks_[i][j].worldTransform = nullptr;
			}
		}
	}
}

void GameScene::CheckCollisionPlayerAndBlocks() {

	AABB playerAABB = player_->GetAABB();
	Vector3 playerPos = player_->GetWorldPosition();

	for (auto& line : blocks_) {
		for (auto& block : line) {
			if (!block.worldTransform)
				continue;

			const AABB& blockAABB = block.aabb;

			const float kEpsilon = 0.1f;

			if (IsCollision(playerAABB, blockAABB)) {

				Vector3 overlap = {
				    std::min(playerAABB.max.x, blockAABB.max.x) - std::max(playerAABB.min.x, blockAABB.min.x),
				    std::min(playerAABB.max.y, blockAABB.max.y) - std::max(playerAABB.min.y, blockAABB.min.y),
				    std::min(playerAABB.max.z, blockAABB.max.z) - std::max(playerAABB.min.z, blockAABB.min.z)
				};

				if (overlap.x < overlap.y && overlap.x < overlap.z) {
					if (playerAABB.min.x < blockAABB.min.x)
						playerPos.x -= overlap.x + kEpsilon;
					else
						playerPos.x += overlap.x + kEpsilon;
				} if (overlap.y < overlap.x && overlap.y < overlap.z) {
					if (playerAABB.min.y < blockAABB.min.y)
						playerPos.y -= overlap.y + kEpsilon;
					else
						playerPos.y += overlap.y + kEpsilon;
				}

				// プレイヤーの座標を更新
				player_->SetWorldPosition(playerPos);

				// 押し戻し後にAABB更新
				playerAABB = player_->GetAABB();
			}
		}
	}
}