#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;
#include <fstream>
#include <iostream>
#include <string>

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに追加する
	enemyBullets_.push_back(enemyBullet);
}

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	// カメラの解放
	delete debugCamera_;
	// 敵キャラの解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	// 敵キャラの弾の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

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
	// 地面の解放
	delete ground_;
	// レールカメラの解放
	delete railCameraController_;
}

void GameScene::Initialize() {
	controlPoints_ = {
	    
		{0.0f, 0.0f, 0.0f}, 
		{0.0f, 0.0f, 10.0f},
		{0.0f, 0.0f, 20.0f},
        {0.0f, 0.0f, 30.0f }, 
		{0.0f, 0.0f, 40.0f }, 
		{0.0f, 0.0f, 50.0f }, 
		{0.0f, 0.0f, 60.0f }, 
	};

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 1000.0f;
	viewProjection_.Initialize();

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

	// レティクルのテクスチャ
	TextureManager::Load("2dReticle.png");

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	Vector3 playerPosition = {0.0f, 0.0f, 15.0f};
	player_->Initialize(playerModel_, textureHandle_, playerBulletModel_, playerPosition);

	// 敵発生データの読み込み
	LoadEnemyPopData();
	
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

	// レールカメラの生成と初期化
	railCameraController_ = new RailCameraController();
	railCameraController_->SetControlPoints(controlPoints_);
	railCameraController_->Initialize(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), &viewProjection_);

	// プレイヤーとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCameraController_->GetWorldTransform());
	player_->SetParent3DReticle(&railCameraController_->GetWorldTransform());

	PrimitiveDrawer::GetInstance()->Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&viewProjection_);
}

void GameScene::Update() {

	// プレイヤーの更新
	player_->Update(viewProjection_);

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

	// 天球の更新
	skydome_->Update();

	// 地面の更新
	ground_->Update();

	if (isDebugCameraActive_) {
		debugCamera_->Update();

		viewProjection_.matView = debugCamera_->GetCamera().matView;
		viewProjection_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {

		railCameraController_->Update();

		viewProjection_.matView = railCameraController_->GetCamera()->matView;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}

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

	std::vector<Vector3> pointsDrawing;
	// 線分の数
	const size_t segmentCount = 100;
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		// 描画用リストに追加
		pointsDrawing.push_back(pos);
	}

	// 先端から2点ずつ取り出して線分を描画
	for (size_t i = 0; i < pointsDrawing.size() - 1; i++) {
		// 線分の始点と終点
		Vector3 start = pointsDrawing[i];
		Vector3 end = pointsDrawing[i + 1];
		// 線分を描画
		PrimitiveDrawer::GetInstance()->DrawLine3d(start, end, {0.0f, 1.0f, 0.0f, 1.0f});
	}
	
	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	// 天球の描画
	skydome_->Draw();

	// 地面の描画
	ground_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	// プレイヤーの描画
	player_->Draw(viewProjection_);

	// 描画終了
	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GameScene::LoadEnemyPopData() {

	// ファイルを開く
	std::ifstream file;
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

			// 敵を発生させる
			EnemyPop({x, y, z});

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

void GameScene::EnemyPop(KamataEngine::Vector3 position) {

	DebugText::GetInstance()->ConsolePrintf("EnemyPop called. this = %p\n", this);

	// 敵キャラの生成
	Enemy* enemy = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	// 敵キャラの初期化
	enemy->SetGameScene(this);
	enemy->Initialize(enemyModel_, enemyTextureHandle_, position, enemyBulletModel_);
	// リストに登録
	enemies_.push_back(enemy);
}
