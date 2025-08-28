#pragma once
#include <KamataEngine.h>
#include <algorithm>

class Fade {

public:
	
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	~Fade();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フェード開始
	/// </summary>
	void Start(Status status, float duration);

	// フェード開始
	void Stop();

	// フェード終了判定
	bool IsFinished() const;

private:

	KamataEngine::Sprite* sprite_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

	// 選択SE
	uint32_t sHSelect = 0;
};
