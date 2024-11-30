#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "MT4_01_04_Basic";

struct Vector3 {
	float x, y, z;
};

struct Quaternion {
	float x, y, z, w;

	// コンストラクタ
	Quaternion(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
		: w(w), x(x), y(y), z(z) {}

	// クォータニオンの掛け算
	Quaternion operator*(const Quaternion& other) const {
		float newW = w * other.w - x * other.x - y * other.y - z * other.z;
		float newX = w * other.x + x * other.w + y * other.z - z * other.y;
		float newY = w * other.y - x * other.z + y * other.w + z * other.x;
		float newZ = w * other.z + x * other.y - y * other.x + z * other.w;
		return Quaternion(newW, newX, newY, newZ);
	}
};

struct Matrix4x4 {
	float m[4][4];
};

Vector3 Normalize(const Vector3& v) {
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0) return v;  // ゼロベクトルの場合
	return Vector3(v.x / length, v.y / length, v.z / length);
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Vector3 normalizedAxis = Normalize(axis);  // 軸ベクトルを正規化
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);
	float cosHalfAngle = cos(halfAngle);

	return Quaternion(cosHalfAngle, normalizedAxis.x * sinHalfAngle, normalizedAxis.y * sinHalfAngle, normalizedAxis.z * sinHalfAngle);
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	// クォータニオンをベクトル部分とスカラー部分に分ける
	Quaternion qConjugate = Quaternion(quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z);
	Quaternion qVector(0.0f, vector.x, vector.y, vector.z);  // (w=0, vector)

	// 回転: q * v * q*
	Quaternion result = quaternion * qVector * qConjugate;

	// 結果からwを除いたベクトル部分を返す
	return Vector3(result.x, result.y, result.z);
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 r{};

	r.m[0][0] = (quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) -
		(quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	r.m[0][1] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	r.m[0][2] = 2 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	r.m[0][3] = 0;


	r.m[1][0] = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	r.m[1][1] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) +
		(quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	r.m[1][2] = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	r.m[1][3] = 0;


	r.m[2][0] = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	r.m[2][1] = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	r.m[2][2] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) -
		(quaternion.y * quaternion.y) + (quaternion.z * quaternion.z);
	r.m[2][3] = 0;

	r.m[0][3] = 0;
	r.m[1][3] = 0;
	r.m[2][3] = 0;
	r.m[3][3] = 1;

	return r;
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.03f",
				matrix.m[row][column]);
		}
	}
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void QuaternionScreenPrintf(int x, int y, Quaternion quaternion, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", quaternion.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", quaternion.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", quaternion.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%.02f", quaternion.w);
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 v1{ 1.0f, 3.0f, -5.0f };
	Vector3 v2{ 4.0f, -1.0f, 2.0f };
	//float k = { 4.0f };

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


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{ 1.0f, 0.4f, -0.2f }), 0.45f);
		Vector3 pointY = { 2.1f, -0.9f, 1.3f };
		Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
		Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
		Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

		QuaternionScreenPrintf(0, kRowHeight * 0, rotation, "   : rotation");
		MatrixScreenPrintf(0, kRowHeight * 1, rotateMatrix, "rotateMatrix");
		VectorScreenPrintf(0, kRowHeight * 6, rotateByQuaternion, "   : rotateByQuaternion");
		VectorScreenPrintf(0, kRowHeight * 7, rotateByMatrix, "   : rotateByMatrix");

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