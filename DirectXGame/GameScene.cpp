#include "GameScene.h"
using namespace KamataEngine;
using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
}

void GameScene::Initialize() {

	// ビュープロジェクションの初期化
	//viewProjection_.farZ = 10.0f;
	viewProjection_.Initialize();
	
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	playerModel_ = Model::Create();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, textureHandle_);
}

void GameScene::Update() {

	// プレイヤーの更新
	player_->Update();

}

void GameScene::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	// プレイヤーの描画
	player_->Draw(viewProjection_);
	
	// 描画終了
	Model::PostDraw();

}
