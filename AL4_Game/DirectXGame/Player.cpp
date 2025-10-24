#include "Player.h"
using namespace KamataEngine;
using namespace MathUtility;

Player::~Player() {
	// bullet_の開放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.z -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.z += kRotSpeed;
	}
}

void Player::Attack() {

	if (input_->TriggerKey(DIK_SPACE) || input_->IsTriggerMouse(0)) {

		// 弾の速度
		const float kBulletSpeed = 0.5f;
		Vector3 velocity = dir * kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, worldTransform_.translation_, velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::RotateToMouse(const KamataEngine::Camera& viewProjection) {
	// --- 1. マウス座標取得 ---
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// --- 2. 逆変換用の行列を準備 ---
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPV = viewProjection.matView * viewProjection.matProjection * matViewport;
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// --- 3. マウス座標をワールドに変換 ---
	Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
	Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

	posNear = TransformCoord(posNear, matInverseVPV);
	posFar = TransformCoord(posFar, matInverseVPV);

	// --- 4. レイと Z=0 平面の交差 ---
	Vector3 mouseDirection = posFar - posNear;
	Vector3 rayDir = Normalize(mouseDirection);
	float t = -posNear.z / rayDir.z;
	Vector3 target = posNear + rayDir * t;

	// --- 5. プレイヤーの位置と方向ベクトル ---
	Vector3 playerPos = worldTransform_.translation_;
	Vector3 targetPos = target - playerPos;
	dir = Normalize(targetPos);

	// --- 6. 回転角度を算出 ---
	float angle = atan2(dir.y, dir.x);
	worldTransform_.rotation_.z = angle;
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelBullet) {

	assert(model);
	model_ = model;
	assert(modelBullet);
	modelBullet_ = modelBullet;
	worldTransform_.Initialize();

	// シングルトンインスタンス
	input_ = Input::GetInstance();

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kCollisionAttributePlayer | kCollisionAttributePlayer);
}

void Player::Update(const KamataEngine::Camera& viewProjection) {

	ImGui::Begin("Player");
	ImGui::DragFloat3("playerPosition", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat("playerSpeed", &kCharacterSpeed, 0.1f);
	ImGui::End();
	
	RotateToMouse(viewProjection);

	move_ = {0, 0, 0};

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A))
		move_.x -= kCharacterSpeed;
	if (input_->PushKey(DIK_D))
		move_.x += kCharacterSpeed;
	if (input_->PushKey(DIK_W))
		move_.y += kCharacterSpeed;
	if (input_->PushKey(DIK_S))
		move_.y -= kCharacterSpeed;

	// 移動限界座標
	//const float kMoveLimitY = 18.0f;
	//const float kMoveLimitX = 34.0f;

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
	//worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	//worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);

	//WorldTransformUpdate(worldTransform_);
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

	model_->Draw(worldTransform_, viewProjection);

	// 描画終了
	Model::PostDraw();
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

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

Vector2 Player::GetMoveInput(){
	Vector2 move = {0, 0};
	if (input_->PushKey(DIK_A))
		move.x -= kCharacterSpeed;
	if (input_->PushKey(DIK_D))
		move.x += kCharacterSpeed;
	if (input_->PushKey(DIK_W))
		move.y += kCharacterSpeed;
	if (input_->PushKey(DIK_S))
		move.y -= kCharacterSpeed;
	return move;
}

void Player::OnCollision() {}
