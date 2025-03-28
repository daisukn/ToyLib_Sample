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
    std::vector<class VertexArray*> GetVertexArray() { return mVertexArrayPtr; }
    // Textureを取得
    class Texture* GetTexture(size_t index);
    // シェーダー名取得
    const std::string& GetShaderName() const { return mShaderName; }
    // Specurer（未実装
    float GetSpecPower() const { return mSpecPower; }
    
    // ボーン変形を反映
    void BoneTransform(float TimeInSeconds, std::vector<Matrix4>& Transforms);
    // モーションIDを設定
    void SetAnimID(int id, PlayMode m);//{ animID = id; mode = m; playTime = 0.0f; }
    
    // 再生中かどうか
    bool GetIsPlaying() const { return mIsPlaying; }
    

private:

    // Assimpオブジェクト
    const aiScene* mScene;
    Assimp::Importer mImporter;
    
    // メッシュ生生(Boneなし)
    void CreateMesh(const aiMesh* m);
    // メッシュ生成（Boneあり)
    void CreateMeshBone(const aiMesh* m);
    // Bone情報読み込み
    void LoadBones(const aiMesh* m, std::vector<struct VertexBoneData>& bones);
    // Bone階層を辿る
    void ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const Matrix4& parentTransform);
    // アニメーション情報を取得
    const aiNodeAnim* FindNodeAnim (const aiAnimation* pAnimation, const std::string nodeName);

    // モーションID
    unsigned int mAnimID;
    unsigned int mPrevAnimID;
    
    // 再生中かどうか
    bool mIsPlaying;
    
    // 再生中の時間
    float mPlayTime;
    PlayMode mPlayMode;
    // モーションごとのフレーム数
    unsigned int mNumAnimations;
    std::vector<float> mDurations;
    void LoadAnimation();
    

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

    
    // TextureのVector
    std::vector<class Texture*> mTextures;
    // VertexArrayのVector
    std::vector<class VertexArray*> mVertexArrayPtr;
    std::vector<std::unique_ptr<class VertexArray>> mVertexArray;

    // シェーダー名
    std::string mShaderName;

    // Specurer
    float mSpecPower;
    

};


