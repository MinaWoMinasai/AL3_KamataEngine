#include "GameScene.h"
#include "ImGui.h"
using namespace KamataEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
}

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// カメラの初期化
	camera_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
}

// 更新
void GameScene::Update() {

	player_->Update();

}

// 描画
void GameScene::Draw() { 

	player_->Draw(); 

}
