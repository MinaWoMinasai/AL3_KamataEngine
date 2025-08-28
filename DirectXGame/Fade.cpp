#include "Fade.h"
using namespace KamataEngine;

Fade::~Fade() { delete sprite_; }

void Fade::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("fade.png");

	sprite_ = Sprite::Create(textureHandle_, {0, 0});
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0, 0, 0, 1));

	sHSelect = Audio::GetInstance()->LoadWave("select.wav");
}

void Fade::Update() {

	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		// フェードイン中
		// フェードアウト中
		counter_ += 1.0f / 60.0f;

		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間にちかづくほどアルファ地を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp((1.0f - counter_ / duration_), 0.0f, 1.0f)));

		break;
	case Status::FadeOut:
		// フェードアウト中
		counter_ += 1.0f / 60.0f;

		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間にちかづくほどアルファ地を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	}
}

void Fade::Draw() {

	if (status_ == Status::None) {
		return;
	}

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();

}

void Fade::Start(Status status, float duration) { 
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { 
	status_ = Status::None; }

bool Fade::IsFinished() const {
	
	switch (status_){ 
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ < duration_) {
			return false;
		}

		break;

	case Status::None:
		return false;
	}
	
	return true;

}
