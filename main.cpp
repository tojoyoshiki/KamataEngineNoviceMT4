#include <Novice.h>
#include <DirectXMath.h>

using Quaternion = DirectX::XMFLOAT4;

const char kWindowTitle[] = "LE2C_17_トウジョウ_ヨシキ_タイトル";

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

        // 値を表示
        Novice::DrawFormatString(0, 0, 0xFFFFFFFF, "q1: (%f, %f, %f, %f)", q1.x, q1.y, q1.z, q1.w);
        Novice::DrawFormatString(0, 20, 0xFFFFFFFF, "q2: (%f, %f, %f, %f)", q2.x, q2.y, q2.z, q2.w);
        Novice::DrawFormatString(0, 40, 0xFFFFFFFF, "Identity: (%f, %f, %f, %f)", identity.x, identity.y, identity.z, identity.w);
        Novice::DrawFormatString(0, 60, 0xFFFFFFFF, "Conjugate of q1: (%f, %f, %f, %f)", conj.x, conj.y, conj.z, conj.w);
        Novice::DrawFormatString(0, 80, 0xFFFFFFFF, "Inverse of q1: (%f, %f, %f, %f)", inv.x, inv.y, inv.z, inv.w);
        Novice::DrawFormatString(0, 100, 0xFFFFFFFF, "Normalized q1: (%f, %f, %f, %f)", normal.x, normal.y, normal.z, normal.w);
        Novice::DrawFormatString(0, 120, 0xFFFFFFFF, "q1 * q2: (%f, %f, %f, %f)", mul1.x, mul1.y, mul1.z, mul1.w);
        Novice::DrawFormatString(0, 140, 0xFFFFFFFF, "q2 * q1: (%f, %f, %f, %f)", mul2.x, mul2.y, mul2.z, mul2.w);
        Novice::DrawFormatString(0, 160, 0xFFFFFFFF, "Norm of q1: %f", norm);

        ///
        /// ↑更新処理ここまで
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
