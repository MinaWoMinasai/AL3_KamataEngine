#include "GameScene.h"
#include "ImGui.h"
using namespace KamataEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	// サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");

	// サウンドの再生
	Audio::GetInstance()->PlayWave(soundDataHandle_);

	// 音声再生
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

// 更新
void GameScene::Update() {

	// スプライトの位置を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を{ 2, 1 }だけ移動
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	// スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// サウンドの再生
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

	debugCamera_->Update();

#ifdef _DEBUG

	// デバッグテキストの表示
	ImGui::Begin("Debug1");
	// float3入力ボックス
	ImGui::InputFloat3("inputFloat3", inputFloat3);
	// float3スライダー
	ImGui::SliderFloat3("inputFloat3", inputFloat3, 0.0f, 1.0f);
	ImGui::Text("Hello, KamataEngine! %d", 30);
	ImGui::End();

	// デモウィンドウの表示を有効化
	ImGui::ShowDemoWindow();

#endif
}

// 描画
void GameScene::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	// ラインを描画する
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1, 0, 0, 1});

	// 3Dモデル描画後処理
	Model::PostDraw();

	//// スプライト描画前処理
	// Sprite::PreDraw(dxCommon->GetCommandList());
	//
	// sprite_->Draw();
	//
	//// スプライト描画後処理
	// Sprite::PostDraw();
}
