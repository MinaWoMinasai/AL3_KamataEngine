#include "Player.h"
using namespace KamataEngine;
using namespace MathUtility;
#include "LockOn.h"

Player::~Player() {
	// bullet_の開放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Attack() {

	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		//return;
	}
	// Rトリガーを押していたら
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) || input_->TriggerKey(DIK_SPACE) || input_->IsTriggerMouse(0)) {

		// ロックオンしていたら
		if (lockOn_->GetTarget() != nullptr) {
			// ロックオン対象の敵を取得
			Enemy* target = lockOn_->GetTarget();

			// 弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, 0);

			// 自機から敵オブジェクトへのベクトル
			velocity = target->GetWorldPosition() - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);

			// 弾を登録する
			bullets_.push_back(newBullet);

		} else {

			// 弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, 0);

			// 自機から標準オブジェクトへのベクトル
			velocity = GetWorldPosition3DReticle() - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);

			// 弾を登録する
			bullets_.push_back(newBullet);
		}
	}
}

void Player::OnCollision() {}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Model* bulletModel, const KamataEngine::Vector3& position) {

	assert(model);
	assert(bulletModel);
	model_ = model;
	bulletModel_ = bulletModel;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// シングルトンインスタンス
	input_ = Input::GetInstance();

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kCollisionAttributePlayer | kCollisionAttributePlayer);

	// 3Dレティクルワールドトランスフォーム
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2dReticle.png");
	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void Player::Update(const KamataEngine::Camera& viewProjection) {

	POINT mousePosition;
	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2dレティクルのスプライトに代入する
	reticlePos2D = Vector2((float)mousePosition.x, (float)mousePosition.y);
	sprite2DReticle_->SetPosition(reticlePos2D);
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matviewProjectionViewport = viewProjection.matView * viewProjection.matProjection * matViewport;
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matviewProjectionViewport);

	// スクリーン座標
	Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
	Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

	// スクリーンからワールド座標系へ
	posNear = TransformCoord(posNear, matInverseVPV);
	posFar = TransformCoord(posFar, matInverseVPV);

	// プレイヤーのワールド座標を取得
	Vector3 playerPos = GetWorldPosition();

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	rayDirection_ = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	worldTransform3DReticle_.translation_ = posNear + rayDirection_ * kDistanceTestObject;
	WorldTransformUpdate(worldTransform3DReticle_);

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	// ゲームパッドの状態をを得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;

		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;

		// スプライトの座標変更を変更
		sprite2DReticle_->SetPosition(spritePosition);
	}

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}

	// 移動限界座標
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	worldTransform_.translation_ += move;

	Rotate();

	// 攻撃処理
	Attack();

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグが立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);

	WorldTransformUpdate(worldTransform_);
}

void Player::Draw(KamataEngine::Camera& viewProjection) {

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	// 弾の描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	// 描画終了
	Model::PostDraw();
}

void Player::DrawUI() { 
	if (lockOn_->GetTarget() == nullptr) {
		sprite2DReticle_->Draw();
	}
}

KamataEngine::Vector3 Player::GetWorldPosition() const {

	// ワールド座標を入れる
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

KamataEngine::Vector3 Player::GetWorldPosition3DReticle() const {
	// ワールド座標を入れる
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}
