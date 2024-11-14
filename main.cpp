#include <Novice.h>
#include <DirectXMath.h>
#include <cmath>
#include <assert.h>

using Quaternion = DirectX::XMFLOAT4;

const char kWindowTitle[] = "LE2C_17_トウジョウ_ヨシキ_タイトル";

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Matrix4x4 {
    float m[4][4];
};

// Vector3用の正規化関数
Vector3 Normalize(const Vector3& vector) {
    float length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    return Vector3{ vector.x / length, vector.y / length, vector.z / length };
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

// ベクトルを回転させる関数
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&vector));
    DirectX::XMVECTOR quat = DirectX::XMLoadFloat4(&quaternion);
    DirectX::XMVECTOR rotatedVec = DirectX::XMVector3Rotate(vec, quat);

    Vector3 result;
    DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&result), rotatedVec);
    return result;
}

// クォータニオンを回転行列に変換する関数
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    Matrix4x4 result;
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), rotationMatrix);
    return result;
}

// Quaternion用の正規化関数
float Norm(const Quaternion& quaternion) {
    return std::sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y +
        quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion) {
    float norm = Norm(quaternion);
    return Quaternion{ quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

Vector3 RotateByMatrix(const Vector3& vector, const Matrix4x4& matrix) {
    return Vector3{
        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0],
        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1],
        vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2]
    };
}

// 表示関数
void QuaternionScreenPrintf(int x, int y, const Quaternion& q, const char* label) {
    Novice::ScreenPrintf(x, y, "%s: (%f, %f, %f, %f)", label, q.x, q.y, q.z, q.w);
}

void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label) {
    Novice::ScreenPrintf(x, y, "%s: (%f, %f, %f)", label, v.x, v.y, v.z);
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

        Vector3 axis = Normalize(Vector3{ 1.0f, 0.4f, -0.2f });
        Quaternion rotation = MakeRotateAxisAngleQuaternion(axis, 0.45f);
        Vector3 pointY = { 2.1f, -0.9f, 1.3f };
        Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
        Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
        Vector3 rotateByMatrix = RotateByMatrix(pointY, rotateMatrix);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        QuaternionScreenPrintf(0, kRowHeight * 0, rotation, "Rotation");
        MatrixScreenPrintf(0, kRowHeight * 1, rotateMatrix, "RotateMatrix");
        VectorScreenPrintf(0, kRowHeight * 6, rotateByQuaternion, "RotateByQuaternion");
        VectorScreenPrintf(0, kRowHeight * 7, rotateByMatrix, "RotateByMatrix");

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
