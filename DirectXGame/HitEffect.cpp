#include "HitEffect.h"
#include <cmath>
#include <cassert>
using namespace KamataEngine;

// 静的メンバ変数の実体
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(const KamataEngine::Vector3& position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.Initialize();
		worldTransform.scale_ = {3.0f, 0.2f, 1.0f};
		worldTransform.rotation_ = {0.0f, 0.0f, GetRamdom(-std::numbers::pi_v<float>, std::numbers::pi_v<float>)};
		worldTransform.translation_ = position;
		
	}
	status_ = Status::Expansion;
}

void HitEffect::Update() {

	switch (status_) {
	case Status::None:
	case Status::Death:
		// 何もしない
		break;
	case Status::Expansion:
		// 拡大中
		counter_ += 1.0f / 60.0f;

		// 継続時間に達したら打ち止め
		if (counter_ >= kDuration) {
			counter_ = 0.0f;
			// フェードアウトに移行する
			status_ = Status::FadeOut;
		}

		circleWorldTransform_.scale_.x *= 1.01f;
		circleWorldTransform_.scale_.y *= 1.01f;
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale_.x *= 1.01f;
			worldTransform.scale_.y *= 1.01f;
		}

		model_->SetAlpha(1.0f);
		break;
	case Status::FadeOut:
		// フェードアウト中
		counter_ += 1.0f / 60.0f;

		// 0.0fから1.0fの間で、経過時間がフェード継続時間にちかづくほどアルファ地を大きくする
		model_->SetAlpha(std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f));

		// フェード継続時間に達したら打ち止め
		if (counter_ >= kDuration) {
			counter_ = 0.0f;
			// デス状態に移行する
			status_ = Status::Death;
		}
		break;
	}

	WorldTrnasformUpdate(circleWorldTransform_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		WorldTrnasformUpdate(worldTransform);
	}
}

void HitEffect::Draw() {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(circleWorldTransform_, *camera_);

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}

	// 3Dモデル描画後処理
	Model::PostDraw();
}

float HitEffect::GetRamdom(const float& min, const float& max) {

	std::uniform_real_distribution<float> dist(min, max);
	return dist(randomEngine);
}

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	// インスタンス生成
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Initialize(position);
	// 初期化↓インスタンスを返す
	return instance;
}
