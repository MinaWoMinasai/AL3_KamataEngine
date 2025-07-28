#include "LockOn.h"  
using namespace KamataEngine;  
using namespace MathUtility; 

LockOn::~LockOn() { delete spriteLockOn_; }

void LockOn::Initialize() {  

   // レティクル用テクスチャ取得  
   uint32_t textureReticle = TextureManager::Load("2dReticle.png");  
   // スプライト生成  
   spriteLockOn_ = Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});  
}  

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const KamataEngine::Camera& camera) {  

   // ロックオン対象リスト
   std::list<std::pair<float, Enemy*>> targets;

   // ロックオン判定処理  
   for (Enemy* enemy : enemies) {  

       // 自機のワールド座標を取得
	   Vector3 positionWorldPlayer = player->GetWorldPosition();

       // ビュー座標に変換する
	   Vector3 positionViewPlayer = TransformCoord(positionWorldPlayer, camera.matView);

       // 敵のワールド座標取得  
       Vector3 positionWorld = enemy->GetWorldPosition();  
       
       // ビュー座標に変換する
	   Vector3 positionViewEnemy = TransformCoord(positionWorld, camera.matView);

       if (positionViewEnemy.z <= positionViewPlayer.z) {
           // 自機より手前にいる場合は除外
		   continue;
       }

       // ワールド座標からスクリーン座標へ変換  
       Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, camera.matView * camera.matProjection);  

       // Vector2に格納  
       Vector2 positionScreenV2(positionScreen.x, positionScreen.y);  

       // スプライトの中心からの距離  
       float distance = Length2D(player->GetReticlePos2D(), positionScreenV2);  
       // ロックオン距離の限界地(スクリーン座標ベース)
	   const float kDistanceLockOn = 100.0f;
       // 2Dレティクルからのスクリーン座標が規定範囲内ならば
	   if (distance <= kDistanceLockOn) {
          targets.emplace_back(std::make_pair(distance, enemy));
       }
   }  

   // 一時ロックオンを解除
   target_ = nullptr;

   // 対象を絞り込んで座標設定する
   if (!targets.empty()) {
   
       // 距離で昇順にソート
	   targets.sort();

       // 距離が一番小さい敵をロックオン対象とする
	   target_ = targets.front().second;
	   
       // 敵のワールド座標取得
	   Vector3 positionWorld = target_->GetWorldPosition();

	   // ワールド座標からスクリーン座標へ変換
	   Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, camera.matView * camera.matProjection);  

       // ロックオンマークへの座標を設定する
	   spriteLockOn_->SetPosition(Vector2(positionScreen.x, positionScreen.y));
   }

   // プレイヤーに送る
   player->SetLockOn(this);
}

void LockOn::Draw() { 
    
    if (target_ == nullptr) {
        return;
    }

    spriteLockOn_->Draw(); 
}
