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

// 表示
void QuaternionScreenPrintf(int x, int y, const Quaternion& q, const char* label, float t) {
	Novice::ScreenPrintf(x, y, "%s (t=%0.1f) : (%f,%f,%f,%f)", label, t, q.x, q.y, q.z, q.w);
}

// クォータニオンの掛け算
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&lhs);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&rhs);
	DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(q1, q2);
	Quaternion out;
	DirectX::XMStoreFloat4(&out, result);
	return out;
}

// クォータニオンを生成する
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

// 球面線形補間
Quaternion Slerp(const Quaternion& q0Input, const Quaternion& q1Input, float t) {
	Quaternion q0 = q0Input;
	Quaternion q1 = q1Input;

	// 内積を計算
	float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	// 内積が負の場合は符号反転
	if (dot < 0.0f) {
		q1.x = -q1.x;
		q1.y = -q1.y;
		q1.z = -q1.z;
		q1.w = -q1.w;
		dot = -dot;
	}

	// 内積が1に近い場合は線形補間
	if (dot > 0.9995f) {
		return Quaternion{
			q0.x * (1.0f - t) + q1.x * t,
			q0.y * (1.0f - t) + q1.y * t,
			q0.z * (1.0f - t) + q1.z * t,
			q0.w * (1.0f - t) + q1.w * t,
		};
	}

	// 球面線形補間を計算
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
	float scale1 = std::sin(t * theta) / sinTheta;

	return Quaternion{
		q0.x * scale0 + q1.x * scale1,
		q0.y * scale0 + q1.y * scale1,
		q0.z * scale0 + q1.z * scale1,
		q0.w * scale0 + q1.w * scale1,
	};
}

// Windowsアプリのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// クォータニオンの準備
		Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
		Quaternion rotation1 = { -rotation0.x, -rotation0.y, -rotation0.z, -rotation0.w };

		// 補間
		float ts[] = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };
		for (int i = 0; i < 5; ++i) {
			Quaternion interpolate = Slerp(rotation0, rotation1, ts[i]);
			QuaternionScreenPrintf(0, i * 20, interpolate, "Slerp", ts[i]);
		}

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
