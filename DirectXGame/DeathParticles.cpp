#include "DeathParticles.h"
#include <cmath>
#include <cassert>
using namespace KamataEngine;
using namespace MathUtility;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録
	model_ = model;

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

}

void DeathParticles::Update() {

	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; i++) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸回り回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	counter_ += 1.0f / 60.0f;
	color_.w = std::clamp(1.0f - counter_, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);

	// カウンターを1フレーム分の秒数進める
	if (counter_ >= kDuraction) {
		counter_ = kDuraction;
		// 終了扱いにする
		isFinished_ = true;
	} 

	for (auto& worldTransform : worldTransforms_) {
		WorldTrnasformUpdate(worldTransform);
	}
}

void DeathParticles::Draw() {

	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	for (auto& worldTransform : worldTransforms_) {

		// DirectXCommonのインスタンスの取得
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		// 3Dモデルの描画
		model_->Draw(worldTransform, *camera_, &objectColor_);

		// 3Dモデル描画後処理
		Model::PostDraw();
	}
}
