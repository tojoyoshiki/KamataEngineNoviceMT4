#include <Novice.h>
#include <DirectXMath.h>
#include <cmath>
#include <assert.h>

using Quaternion = DirectX::XMFLOAT4;

const char kWindowTitle[] = "LE2C_17_トウジョウ_ヨシキ_タイトル";

struct Vector3 {
    float x, y, z;
};

struct Matrix4x4 {
    float m[4][4];
};

// クォータニオンの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
    DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&lhs);
    DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&rhs);
    DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(q1, q2);
    Quaternion out;
    DirectX::XMStoreFloat4(&out, result);
    return out;
}

// 単位クォータニオン
Quaternion IdentityQuaternion() {
    return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
}

// クォータニオンの共役
Quaternion Conjugate(const Quaternion& quaternion) {
    return Quaternion{ -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

// クォータニオンのノルム
float Norm(const Quaternion& quaternion) {
    return std::sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y +
        quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

// クォータニオンの正規化
Quaternion Normalize(const Quaternion& quaternion) {
    float norm = Norm(quaternion);
    return Quaternion{ quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

// クォータニオンの逆
Quaternion Inverse(const Quaternion& quaternion) {
    Quaternion conjugate = Conjugate(quaternion);
    float normSquared = Norm(quaternion);
    normSquared *= normSquared;
    return Quaternion{ conjugate.x / normSquared, conjugate.y / normSquared,
                       conjugate.z / normSquared, conjugate.w / normSquared };
}

// クォータニオンの内容を表示
void QuaternionScreenPrintf(int x, int y, const Quaternion& q, const char* label) {
    Novice::ScreenPrintf(x, y, "%s: (%f, %f, %f, %f)", label, q.x, q.y, q.z, q.w);
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

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

        Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
        Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };
        Quaternion identity = IdentityQuaternion();
        Quaternion conj = Conjugate(q1);
        Quaternion inv = Inverse(q1);
        Quaternion normal = Normalize(q1);
        Quaternion mul1 = Multiply(q1, q2);
        Quaternion mul2 = Multiply(q2, q1);
        float norm = Norm(q1);

        ///
        /// ↑更新処理ここまで
        ///
        
        /// 
        /// ↓描画処理ここから
        ///

        // クォータニオンの演算結果を表示
        QuaternionScreenPrintf(0, 0, q1, "q1");
        QuaternionScreenPrintf(0, 20, q2, "q2");
        QuaternionScreenPrintf(0, 40, identity, "Identity");
        QuaternionScreenPrintf(0, 60, conj, "Conjugate of q1");
        QuaternionScreenPrintf(0, 80, inv, "Inverse of q1");
        QuaternionScreenPrintf(0, 100, normal, "Normalized q1");
        QuaternionScreenPrintf(0, 120, mul2, "Multiply(q1 * q2)");
        QuaternionScreenPrintf(0, 140, mul1, "Multiply(q2 * q1)");
        Novice::ScreenPrintf(0, 160, "Norm of q1: %f", norm);

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
