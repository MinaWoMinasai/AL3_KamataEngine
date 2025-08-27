#include "Skydome.h"
#include <cmath>
#include <cassert>
using namespace KamataEngine;
using namespace MathUtility;

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	// NULLポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

}

void Skydome::Update() { WorldTrnasformUpdate(worldTransform_); }

void Skydome::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, *camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}