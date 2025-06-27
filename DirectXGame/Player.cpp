#include "Player.h"
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

}

void Player::Update() {

	WorldTrnasformUpdate(worldTransform_);
	
}

void Player::Draw(KamataEngine::Camera& viewProjection) { 
	
	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, viewProjection, textureHandle_); 
		
	// 描画終了
	Model::PostDraw();
}
