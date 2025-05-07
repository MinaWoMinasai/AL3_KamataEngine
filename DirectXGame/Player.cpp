#include "Player.h"
#include "ImGui.h"
using namespace KamataEngine;
using namespace MathUtility;


void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {

	// NULLポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	worldTransform_.translation_.x = -3.0f;

}

void Player::Update() {

	// アフィン変換行列を計算してメンバ変数に代入
	KamataEngine::Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransform_.scale_);
	KamataEngine::Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransform_.translation_);
	KamataEngine::Matrix4x4 rotateXMatrix = MakeRotateXMatrix(worldTransform_.rotation_.x);
	KamataEngine::Matrix4x4 rotateYMatrix = MakeRotateYMatrix(worldTransform_.rotation_.y);
	KamataEngine::Matrix4x4 rotateZMatrix = MakeRotateZMatrix(worldTransform_.rotation_.z);
	KamataEngine::Matrix4x4 affineMatrix = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

	worldTransform_.matWorld_ = affineMatrix;

	// 定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}
