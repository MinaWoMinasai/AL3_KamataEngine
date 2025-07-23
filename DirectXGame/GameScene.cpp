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
	// 地面の解放
	delete ground_;
	// レールカメラの解放
	delete railCameraController_;
}

void GameScene::Initialize() {
	controlPoints_ = {
	    {0.0f,   0.0f,  0.0f  }, // スタート
	    {10.0f,  2.0f,  10.0f }, // ゆるく上昇
	    {20.0f,  5.0f,  25.0f }, // 右へカーブ
	    {15.0f,  10.0f, 40.0f }, // 左へ戻る
	    {0.0f,   15.0f, 55.0f }, // 高低差のピーク
	    {-10.0f, 10.0f, 70.0f }, // 左へ下る
	    {-15.0f, 5.0f,  85.0f }, // 地面に近づく
	    {0.0f,   0.0f,  100.0f}, // ゴール付近（直線）
	    {5.0f,   0.0f,  110.0f}, // エンド（確認用）
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

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	Vector3 playerPosition = {0.0f, 0.0f, 15.0f};
	player_->Initialize(playerModel_, textureHandle_, playerBulletModel_, playerPosition);

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

	// レールカメラの生成と初期化
	railCameraController_ = new RailCameraController();
	railCameraController_->SetControlPoints(controlPoints_);
	railCameraController_->Initialize(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), &viewProjection_);

	// プレイヤーとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCameraController_->GetWorldTransform());

	PrimitiveDrawer::GetInstance()->Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&viewProjection_);
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
