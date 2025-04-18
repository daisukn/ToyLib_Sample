#pragma once

#include "MathUtils.h"
#include "Animation.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>


// Mesh管理（アニメーション対応）
class Mesh
{
public:
    Mesh();
    ~Mesh();

    // メッシュファイルロード
    virtual bool Load(const std::string& fileName, class Renderer* renderer, bool isRightHanded = false);
    // アンロード
    void Unload();

    // VertexArrayを取得
    const std::vector<std::shared_ptr<class VertexArray>>& GetVertexArray() { return mVertexArray; }
    
    // マテリアル取得
    std::shared_ptr<class Material> GetMaterial(size_t index);
    
    // シェーダー名取得
    const std::string& GetShaderName() const { return mShaderName; }
    // Specurer（未実装
    float GetSpecPower() const { return mSpecPower; }

    
    const aiScene* GetScene() const { return mScene; }
    
    void ComputePoseAtTime(float animationTime, const aiAnimation* pAnimation, std::vector<Matrix4>& outTransforms);

private:

    // Assimpオブジェクト
    const aiScene* mScene;
    Assimp::Importer mImporter;
    
    void LoadMeshData();
    void LoadMaterials(class Renderer* renderer);
    void LoadAnimations();
    
    // メッシュ生生(Boneなし)
    void CreateMesh(const aiMesh* m);
    // メッシュ生成（Boneあり)
    void CreateMeshBone(const aiMesh* m);
    // Bone情報読み込み
    void LoadBones(const aiMesh* m, std::vector<struct VertexBoneData>& bones);
    // Bone階層を辿る
    void ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const Matrix4& parentTransform, const aiAnimation* pAnimation);
    // アニメーション情報を取得
    const aiNodeAnim* FindNodeAnim (const aiAnimation* pAnimation, const std::string nodeName);
    
    
    // モーションごとのフレーム数
    unsigned int mNumAnimations;
    std::vector<float> mDurations;
    

    // フレームごとのBone姿勢を計算
    void CalcInterpolatedScaling (Vector3& outVec, float animationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation (Quaternion& outVec, float animationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition (Vector3& outV, float animationTime, const aiNodeAnim* pNodeAnim);
    
    unsigned int FindScaling(float animationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float animationTime, const aiNodeAnim* pNodeAnim);


    // Bone情報格納用
    std::map<std::string,unsigned int> mBoneMapping;
    // Bone数
    unsigned int mNumBones;
    // BoneとWeightを格納
    std::vector<struct BoneInfo> mBoneInfo;
    // 逆行列を記憶
    Matrix4 mGlobalInverseTransform;

    // VertexArrayのVector
    std::vector<std::shared_ptr<class VertexArray>> mVertexArray;
    
    // MatrialのVector
    std::vector<std::shared_ptr<class Material>> mMaterials;

    // シェーダー名
    std::string mShaderName;

    // Specurer
    float mSpecPower;
    

};


