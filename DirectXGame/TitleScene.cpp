#include "TitleScene.h"
using namespace KamataEngine;
using namespace MathUtility;

TitleScene::~TitleScene() { 
	delete modelPlayer_;
	delete modelText_;
	delete fade_;
}

void TitleScene::Initialize() {

	// モデルの読み込み
	modelPlayer_ = Model::CreateFromOBJ("Player", true);
	modelText_ = Model::CreateFromOBJ("TitleText", true);

	// ワールドトランスフォームを初期化
	playerWorldTransform.Initialize();
	textWorldTransform.Initialize();

	playerWorldTransform.translation_ = { 0.0f, 0.0f, -30.0f};
	playerWorldTransform.rotation_.y = std::numbers::pi_v<float> * (3.0f / 2.0f);
	textWorldTransform.translation_ = {0.0f, 0.0f, 100.0f};

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, kDuration);

	// カメラの初期化
	camera_.farZ = 1000.0f;
	camera_.Initialize();

}

void TitleScene::Update() {

	time += 1.0f / 60.0f;
	float amplitude = 10.0f;
	float speed = 2.0f;
	const float baseY = 0.0f;

	// 揺らすY座標
	textWorldTransform.translation_.y = baseY + std::sin(time * speed) * amplitude;

	WorldTrnasformUpdate(playerWorldTransform);
	WorldTrnasformUpdate(textWorldTransform);

	// フェード
	switch (phase_) {
	
		case Phase::kFadeIn:
			
			fade_->Update();
		    if (fade_->IsFinished()) {
			    fade_->Stop();
			    phase_ = Phase::kMain;
			}

			break;
	    case Phase::kMain:

			if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			    fade_->Start(Fade::Status::FadeOut, kDuration);
			    phase_ = Phase::kFadeOut;
			}

			break;

		case Phase::kFadeOut:
		   
			if (fade_->IsFinished()) {
			    finished_ = true;
			    fade_->Stop();
		    }
			fade_->Update();

			break;
	}
}

void TitleScene::Draw() {

	// directXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// テキストの描画
	modelText_->Draw(textWorldTransform, camera_);

	// プレイヤーの描画
	modelPlayer_->Draw(playerWorldTransform, camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}
