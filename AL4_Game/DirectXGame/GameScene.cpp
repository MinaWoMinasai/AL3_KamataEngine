#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;
#include <fstream>
#include <iostream>
#include <string>

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	delete debugCamera_;
	//delete enemy_;
	delete playerModel_;
	delete enemyModel_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	// 敵キャラの弾の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	// 衝突マネージャの解放
	delete collisionManager_;
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
	modelBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("bullet", true));

	mapChip_ = std::make_unique<MapChip>();
	mapChip_->LoadMapChipCsv("Resources/map.csv");
	GeneratteBlocks();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, modelBullet_.get());

	// 敵発生データの読み込み
	LoadEnemyPopData();

	// 衝突マネージャの生成
	collisionManager_ = new CollisionManager();

	// 敵キャラの生成
	//enemy_ = new Enemy();
	//// 敵キャラの初期化
	//Vector3 enemyPosition = {2.0f, 0.0f, 0.0f};
	//enemy_->Initialize(enemyModel_, enemyTextureHandle_, enemyPosition);

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

	// 弾とブロックの当たり判定
	CheckCollisionBulletsAndBlocks();

    // X軸移動
	Vector3 playerPos = player_->GetWorldPosition();
	playerPos.x += player_->GetMove().x;
	player_->SetWorldPosition(playerPos);
	CheckCollisionPlayerAndBlocks('x');

	// Y軸移動
	playerPos = player_->GetWorldPosition();
	playerPos.y += player_->GetMove().y;
	player_->SetWorldPosition(playerPos);
	CheckCollisionPlayerAndBlocks('y');

	// 敵の更新
	//enemy_->Update();

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

	// 敵発生コマンドの更新
	UpdateEnemyPopCommands();

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	// 敵の弾の更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

	// 衝突マネージャの更新
	collisionManager_->CheckAllCollisions(player_, enemies_, enemyBullets_);

	// デスフラグが立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// デスフラグが立った球を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_LSHIFT)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
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
	for (Enemy* enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	// 敵の描画
	//enemy_->Draw(viewProjection_);

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
				//block.worldTransform->scale_ = {0.5f, 0.5f, 0.5f};


				// 中心座標
				const auto& pos = block.worldTransform->translation_;

				// AABB設定
				block.aabb.min = {pos.x - mapChip_->kBlockWidth / 2.0f, pos.y - mapChip_->kBlockHeight / 2.0f, pos.z - mapChip_->kBlockWidth / 2.0f};
				block.aabb.max = {pos.x + mapChip_->kBlockWidth / 2.0f, pos.y + mapChip_->kBlockHeight / 2.0f, pos.z + mapChip_->kBlockWidth / 2.0f};
			} else {
				// 空白セルはnullptrにしておく
				blocks_[i][j].worldTransform = nullptr;
			}
		}
	}
}

void GameScene::CheckCollisionPlayerAndBlocks(char axis) {

	AABB playerAABB = player_->GetAABB();
	Vector3 playerPos = player_->GetWorldPosition();

	for (auto& line : blocks_) {
		for (auto& block : line) {
			if (!block.worldTransform)
				continue;

			const AABB& blockAABB = block.aabb;
			const float kEpsilon = 0.01f;

			if (IsCollision(playerAABB, blockAABB)) {

				Vector3 overlap = {
				    std::min(playerAABB.max.x, blockAABB.max.x) - std::max(playerAABB.min.x, blockAABB.min.x),
				    std::min(playerAABB.max.y, blockAABB.max.y) - std::max(playerAABB.min.y, blockAABB.min.y),
				    std::min(playerAABB.max.z, blockAABB.max.z) - std::max(playerAABB.min.z, blockAABB.min.z)};

				if (axis == 'x') {
					if (playerAABB.min.x < blockAABB.min.x)
						playerPos.x -= overlap.x + kEpsilon;
					else
						playerPos.x += overlap.x + kEpsilon;
				} else if (axis == 'y') {
					if (playerAABB.min.y < blockAABB.min.y)
						playerPos.y -= overlap.y + kEpsilon;
					else
						playerPos.y += overlap.y + kEpsilon;
				}

				player_->SetWorldPosition(playerPos);
				playerAABB = player_->GetAABB();
			}
		}
	}
}

void GameScene::CheckCollisionBulletsAndBlocks() {

	for (auto bulletIter = player_->GetBullets().begin(); bulletIter != player_->GetBullets().end();) {
		PlayerBullet* bullet = *bulletIter;
		if (!bullet) {
			bulletIter = player_->GetBullets().erase(bulletIter);
			continue;
		}

		Vector3 bulletPos = bullet->GetWorldPosition();
		float radius = bullet->GetRadius(); // ← 新規: 弾の半径を取得する関数を用意
		Sphere bulletSphere{bulletPos, radius};
		bool isCollided = false;
		float nearestDist = std::numeric_limits<float>::max();
		Block* nearestBlock = nullptr;
		Vector3 nearestClosestPoint{};
		Vector3 nearestNormal{};

		Vector3 velocity = bullet->GetMove();
		Vector3 velocityDir = Normalize(velocity);

					// 修正後
					ImGui::DragFloat3("dir", reinterpret_cast<float*>(&velocityDir), 0.1f);
					ImGui::DragFloat3("normal", reinterpret_cast<float*>(&normal_), 0.1f);
					ImGui::End();

		for (auto& line : blocks_) {
			for (auto& block : line) {
				if (!block.worldTransform)
					continue;

				const AABB& blockAABB = block.aabb;

				if (IsCollision(blockAABB, bulletSphere)) {
					isCollided = true;

					// --- 衝突処理 ---
					Vector3 closestPoint{
					    std::clamp(bulletSphere.center.x, blockAABB.min.x, blockAABB.max.x), std::clamp(bulletSphere.center.y, blockAABB.min.y, blockAABB.max.y),
					    std::clamp(bulletSphere.center.z, blockAABB.min.z, blockAABB.max.z)};

					// 球の中心→接触点へのベクトル（法線方向）
					Vector3 normal = bulletSphere.center - closestPoint;
					normal = Normalize(normal);
					normal_ = normal;
					float dist = Length(bulletSphere.center - closestPoint);

					if (dist < nearestDist) {
						nearestDist = dist;
						nearestBlock = &block;
						nearestClosestPoint = closestPoint;
						nearestNormal = normal;
						isCollided = true;
					}
				}
			}
			if (isCollided)
				break;
		}

		if (isCollided && nearestBlock) {
			bulletPos = nearestClosestPoint + nearestNormal * (radius + 0.01f);

			Vector3 vel = bullet->GetMove();
			float dot = Dot(vel, nearestNormal);
			vel = vel - 2.0f * dot * nearestNormal; // 反射式 R = V - 2(V・N)N

			bullet->SetVelocity(vel);
			bullet->SetWorldPosition(bulletPos);
		}

		// 弾削除処理
		if (bullet->IsDead()) {
			bulletIter = player_->GetBullets().erase(bulletIter);
			delete bullet;
		} else {
			++bulletIter;
		}
	}
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに追加する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::LoadEnemyPopData() {

	// ファイルを開く
	std::ifstream file; // std::ifstreamの型エラーを解消
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	// 待機処理
	if (isWaiting_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			isWaiting_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (std::getline(enemyPopCommands, line)) {
		// 一行分の文字列をストリームの変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		std::getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {

			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {

			// x座標
			std::getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			std::getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			std::getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			std::string behavior;
			if (std::getline(line_stream, behavior, ',')) {
				// 空じゃなければ渡す
				EnemyPop({x, y, z}, behavior);
			} else {
				// なければデフォルト
				EnemyPop({x, y, z}, "Approach");
			}

			// WAITコマンド
		} else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = std::atoi(word.c_str());

			// 待機開始
			isWaiting_ = true;
			waitTime_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::EnemyPop(KamataEngine::Vector3 position, const std::string& behaviorName) {

	DebugText::GetInstance()->ConsolePrintf("EnemyPop called. this = %p\n", this);

	// 敵キャラの生成
	Enemy* enemy = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	// 敵キャラの初期化
	enemy->SetGameScene(this);
	enemy->Initialize(enemyModel_, enemyTextureHandle_, position, enemyModel_, behaviorName);
	// リストに登録
	enemies_.push_back(enemy);
}