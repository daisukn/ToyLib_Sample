#version 410

// 頂点シェーダーからの入力
in vec3 fragPosition;  // 頂点の位置
in vec3 fragNormal;    // 頂点の法線（シャドウマップには使わないが、ライティング計算には必要）

// 出力
out vec4 fragPosLightSpace; // ライト空間での位置（シャドウ計算に使用）

// ライトのビュー行列と投影行列
uniform mat4 uLightSpaceMatrix;

void main()
{
    // ライトの視点に変換された位置
    fragPosLightSpace = uLightSpaceMatrix * vec4(fragPosition, 1.0f);
    
    // 頂点位置をカメラの視点から変換
    gl_Position = fragPosLightSpace; // シャドウマップには深度だけが必要なので、位置をそのまま出力
}
