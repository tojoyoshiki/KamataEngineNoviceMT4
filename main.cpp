#include <KamataEngine.h>
#include <Novice.h>
#include <DirectXMath.h>
#include <cmath>

const char kWindowTitle[] = "LE2C_17_トウジョウ_ヨシキ_タイトル";

using Quaternion = DirectX::XMFLOAT4;

struct Vector3 {
	float x;
	float y;
	float z;
};

//表示
void QuaternionScreenPrintf(int x, int y, 
	const Quaternion& q, const char* label)
{
	Novice::ScreenPrintf(x, y, "%s : (%f,%f,%f,%f)", label,
		q.x, q.y, q.z, q.w);
}

//せき
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&lhs);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&rhs);
	DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(q1, q2);
	Quaternion out;
	DirectX::XMStoreFloat4(&out, result);
	return out;
}

// クォータニオンを生成する関数
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	float sinHalfAngle = sin(angle / 2.0f);
	float cosHalfAngle = cos(angle / 2.0f);
	return Quaternion{
		axis.x * sinHalfAngle,
		axis.y * sinHalfAngle,
		axis.z * sinHalfAngle,
		cosHalfAngle
	};
}

//球面線形補間
Quaternion Slerp(const Quaternion& q0Input, const Quaternion& q1Input, float t) {
	Quaternion q0 = q0Input;
	Quaternion q1 = q1Input;

	// クォータニオンの内積を計算
	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	// 内積が負の場合、符号を反転して最短経路を選択
	if (dot < 0.0f) {
		q1.x = -q1.x;
		q1.y = -q1.y;
		q1.z = -q1.z;
		q1.w = -q1.w;
		dot = -dot;
	}

	// 内積が1に近い場合（θが非常に小さい場合）
	if (dot > 0.9995f) {
		// 線形補間を使用
		return Quaternion{
			q0.x * (1.0f - t) + q1.x * t,
			q0.y * (1.0f - t) + q1.y * t,
			q0.z * (1.0f - t) + q1.z * t,
			q0.w * (1.0f - t) + q1.w * t,
		};
	}

	// θ を計算
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);

	// 球面線形補間のスケールを計算
	float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
	float scale1 = std::sin(t * theta) / sinTheta;

	// 補間結果を計算
	return Quaternion{
		q0.x * scale0 + q1.x * scale1,
		q0.y * scale0 + q1.y * scale1,
		q0.z * scale0 + q1.z * scale1,
		q0.w * scale0 + q1.w * scale1,
	};
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
		Quaternion rotation1 = { -rotation0.x,-rotation0.y,-rotation0.z,-rotation0.w };

		Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
		Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
		Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
		Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
		Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		QuaternionScreenPrintf(0, 0, interpolate0, "interpolate0");
		QuaternionScreenPrintf(0, 20, interpolate1, "interpolate1");
		QuaternionScreenPrintf(0, 40, interpolate2, "interpolate2");
		QuaternionScreenPrintf(0, 60, interpolate3, "interpolate3");
		QuaternionScreenPrintf(0, 80, interpolate4, "interpolate4");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
