#include "Mesh.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Bone.h"
#include "Polygon.h"
#include "Material.h"

// Assimp用
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <string>

const std::string ASSETS_PATH = "Assets/";


// aiMatrix4x4からMatrix4へ変換
static void MatrixAi2Gl(Matrix4& mat, const aiMatrix4x4 aim)
{

    mat.mat[0][0] = aim.a1;
    mat.mat[0][1] = aim.b1;
    mat.mat[0][2] = aim.c1;
    mat.mat[0][3] = aim.d1;

    mat.mat[1][0] = aim.a2;
    mat.mat[1][1] = aim.b2;
    mat.mat[1][2] = aim.c2;
    mat.mat[1][3] = aim.d2;

    mat.mat[2][0] = aim.a3;
    mat.mat[2][1] = aim.b3;
    mat.mat[2][2] = aim.c3;
    mat.mat[2][3] = aim.d3;

    mat.mat[3][0] = aim.a4;
    mat.mat[3][1] = aim.b4;
    mat.mat[3][2] = aim.c4;
    mat.mat[3][3] = aim.d4;

}


// コンストラクタ
Mesh::Mesh()
: mScene(nullptr)
, mNumBones(0)
, mSpecPower(1.0f)
, mAnimID(0)
, mPrevAnimID(0)
, mNumAnimations(0)
, mPlayTime(0.0f)
, mPlayMode(PLAY_ONCE)
, mIsPlaying(false)
{
    
}

Mesh::~Mesh()
{
    mVertexArray.clear();
}


void Mesh::SetAnimID(int id, PlayMode m)
{
    if(mPrevAnimID != id)
    {
        mPlayTime = 0.0f;
        mIsPlaying = true;
    }

    mPrevAnimID = mAnimID % mNumAnimations;
    mAnimID = id;
    mPlayMode = m;
}


// アニメーション時間の反映
void Mesh::BoneTransform(float deltaTime, std::vector<Matrix4>& transforms)
{
    
    mPlayTime += deltaTime;
    float ticksPerSecond = (float)(mScene->mAnimations[mAnimID]->mTicksPerSecond != 0 ? mScene->mAnimations[mAnimID]->mTicksPerSecond : 25.0f);
    float timeInTicks = mPlayTime * ticksPerSecond;


    if (mPlayMode == PLAY_ONCE)
    {
        if (timeInTicks > mDurations[mAnimID])
        {
            // 再生が終わっている場合は、最後のポーズに固定
            mPlayTime = mDurations[mAnimID];
            mIsPlaying = false;
        }
    }
    
    Matrix4 identity = Matrix4::Identity;
    

    
    float animationTime = fmod(timeInTicks, (float)mScene->mAnimations[mAnimID]->mDuration);
    
    ReadNodeHeirarchy(animationTime, mScene->mRootNode, identity);

    transforms.resize(mNumBones);

    for (unsigned int i = 0; i < mNumBones; i++)
    {
         transforms[i] = mBoneInfo[i].FinalTransformation;
    }
}

// 階層を辿ってノードの変換行列を得る
void Mesh::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const Matrix4& parentTransform)
{
    std::string nodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = mScene->mAnimations[mAnimID];
    
        
    Matrix4 nodeTransformation;
    MatrixAi2Gl(nodeTransformation, pNode->mTransformation);
    
    
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);
    
    if (pNodeAnim)
    {
        // スケーリング
        Vector3 scaling;
        CalcInterpolatedScaling(scaling, animationTime, pNodeAnim);
        Matrix4 scalingM = Matrix4::CreateScale(scaling);
        
        // 回転マトリックス
        Quaternion rotationQ;
        CalcInterpolatedRotation(rotationQ, animationTime, pNodeAnim);
        Matrix4 rotationM = Matrix4::CreateFromQuaternion(rotationQ);
        
        // 移動マトリックス
        Vector3 translation;
        CalcInterpolatedPosition(translation, animationTime, pNodeAnim);
        Matrix4 translationM = Matrix4::CreateTranslation(translation);
        
        // ローカルマトリックスを生成
        nodeTransformation = rotationM * translationM * scalingM;;
        //nodeTransformation = translationM * rotationM * scalingM;
    }

    
    
    // グローバルマトリックスを生成
    Matrix4 globalTransformation = nodeTransformation * parentTransform;// * mGlobalInverseTransform;
    //Matrix4 globalTransformation = parentTransform * nodeTransformation;// * mGlobalInverseTransform;

    if (mBoneMapping.find(nodeName) != mBoneMapping.end())
    {
        unsigned int boneIndex = mBoneMapping[nodeName];
        mBoneInfo[boneIndex].FinalTransformation =  mBoneInfo[boneIndex].BoneOffset * globalTransformation * mGlobalInverseTransform;
        //mBoneInfo[boneIndex].FinalTransformation = mGlobalInverseTransform * globalTransformation * mBoneInfo[boneIndex].BoneOffset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
    }
}

// アニメーションデータを取得
const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string nodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++)
    {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (std::string(pNodeAnim->mNodeName.data) == nodeName)
        {
            return pNodeAnim;
        }
    }
    return nullptr;
}


void Mesh::CalcInterpolatedPosition(Vector3& outVec, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        outVec.Set(pNodeAnim->mPositionKeys[0].mValue.x,
                pNodeAnim->mPositionKeys[0].mValue.y,
                pNodeAnim->mPositionKeys[0].mValue.z);
        return;
    }
            
    unsigned int positionIndex = FindPosition(animationTime, pNodeAnim);
    unsigned int nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < pNodeAnim->mNumPositionKeys);
    
    float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);

    const aiVector3D& start = pNodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& end = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D delta = end - start;
    aiVector3D tr = start + factor * delta;
    //tr = tr.Normalize();
    outVec.Set(tr.x, tr.y, tr.z);
}


void Mesh::CalcInterpolatedRotation(Quaternion& outVec, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        outVec.Set(pNodeAnim->mRotationKeys[0].mValue.x,
                pNodeAnim->mRotationKeys[0].mValue.y,
                pNodeAnim->mRotationKeys[0].mValue.z,
                pNodeAnim->mRotationKeys[0].mValue.w);
        return;
    }
    
    unsigned int rotationIndex = FindRotation(animationTime, pNodeAnim);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < pNodeAnim->mNumRotationKeys);
    
    float deltaTime = (float)(pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);

    const aiQuaternion& startRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotationQ   = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;

    
    aiQuaternion q;
    aiQuaternion::Interpolate(q, startRotationQ, endRotationQ, factor);
    q = q.Normalize();
    outVec.Set(q.x, q.y, q.z, q.w);
//    outVec.Normalize();

}


void Mesh::CalcInterpolatedScaling(Vector3& outVec, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        outVec.Set(pNodeAnim->mScalingKeys[0].mValue.x,
                pNodeAnim->mScalingKeys[0].mValue.y,
                pNodeAnim->mScalingKeys[0].mValue.z);
        return;
    }

    unsigned int scalingIndex = FindScaling(animationTime, pNodeAnim);
    unsigned int nextScalingIndex = (scalingIndex + 1);
    
    assert(nextScalingIndex < pNodeAnim->mNumScalingKeys);
    float deltaTime = (float)(pNodeAnim->mScalingKeys[nextScalingIndex].mTime - pNodeAnim->mScalingKeys[scalingIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    
    const aiVector3D& start = pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& end   = pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
    
    aiVector3D delta = end - start;
    aiVector3D sc = start + factor * delta;
    //sc = sc.Normalize();
    outVec.Set(sc.x, sc.y, sc.z);
}


unsigned int Mesh::FindPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int Mesh::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int Mesh::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


// アニメーションデータ読み込み
void Mesh::LoadAnimation()
{
    mNumAnimations = mScene->mNumAnimations;
    mDurations.resize(mNumAnimations);
    for (int i = 0; i < mNumAnimations; i++)
    {
        mDurations[i] = (float)mScene->mAnimations[i]->mDuration;
        
    }
    
}

// ボーン読み込み
void Mesh::LoadBones(const aiMesh* m, std::vector<VertexBoneData>& bones)
{
    for (unsigned int i = 0 ; i < m->mNumBones ; i++)
    {
        unsigned int boneIndex = 0;
        std::string boneName(m->mBones[i]->mName.data);
        
        if (mBoneMapping.find(boneName) == mBoneMapping.end())
        {
            // Allocate an index for a new bone
            boneIndex = mNumBones;
            mNumBones++;
            BoneInfo bi;
            mBoneInfo.push_back(bi);
            
            // オフセットマトリックスのコピー
            Matrix4 mat;
            MatrixAi2Gl(mat, m->mBones[i]->mOffsetMatrix);
            mBoneInfo[boneIndex].BoneOffset = mat;
            mBoneMapping[boneName] = boneIndex;
        }
        else
        {
            boneIndex = mBoneMapping[boneName];
        }
        
        for (unsigned int j = 0 ; j < m->mBones[i]->mNumWeights ; j++)
        {
            unsigned int VertexID = m->mBones[i]->mWeights[j].mVertexId;

            float weight  = m->mBones[i]->mWeights[j].mWeight;
            bones[VertexID].AddBoneData(boneIndex, weight);
        }
    }
}

// Bone付き
void Mesh::CreateMeshBone(const aiMesh* m)
{

    std::vector<float> vertexBuffer;            // ３個ずつ（XYZ）
    std::vector<float> normalBuffer;            // ３個ずつ
    std::vector<float> uvBuffer;                // ２個ずつ
    std::vector<unsigned int> boneIDs;          // ４個ずつ
    std::vector<float> boneWeights;             // ４個ずつ
    std::vector<unsigned int> indexBuffer;
    
    std::vector<VertexBoneData> bones;
    // Boneバッファ
    bones.resize(m->mNumVertices);

    LoadBones(m, bones);
    
    for (unsigned int i = 0 ; i < m->mNumVertices ; i++)
    {
        vertexBuffer.push_back(m->mVertices[i].x);
        vertexBuffer.push_back(m->mVertices[i].y);
        vertexBuffer.push_back(m->mVertices[i].z);
            
        normalBuffer.push_back(m->mNormals[i].x);
        normalBuffer.push_back(m->mNormals[i].y);
        normalBuffer.push_back(m->mNormals[i].z);

        if (m->HasTextureCoords(0))
        {
            uvBuffer.push_back(m->mTextureCoords[0][i].x);
            uvBuffer.push_back(m->mTextureCoords[0][i].y);
        }
        else
        {
            uvBuffer.push_back(0.0f);
            uvBuffer.push_back(0.0f);
        }

        boneIDs.push_back(bones[i].IDs[0]);
        boneIDs.push_back(bones[i].IDs[1]);
        boneIDs.push_back(bones[i].IDs[2]);
        boneIDs.push_back(bones[i].IDs[3]);
        
        boneWeights.push_back(bones[i].Weights[0]);
        boneWeights.push_back(bones[i].Weights[1]);
        boneWeights.push_back(bones[i].Weights[2]);
        boneWeights.push_back(bones[i].Weights[3]);
    }

    // インデックスバッファ取り出し
    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        const aiFace& Face = m->mFaces[i];
        assert(Face.mNumIndices == 3);
        indexBuffer.push_back((Face.mIndices[0]));
        indexBuffer.push_back((Face.mIndices[1]));
        indexBuffer.push_back((Face.mIndices[2]));
    }
    
    
    mVertexArray.push_back(std::make_unique<VertexArray>(
            static_cast<unsigned int>(vertexBuffer.size()) / 3,
            vertexBuffer.data(),
            normalBuffer.data(),
            uvBuffer.data(),
            boneIDs.data(),
            boneWeights.data(),
            static_cast<unsigned int>(indexBuffer.size()),
            indexBuffer.data()) );
    
    mVertexArray[mVertexArray.size()-1]->SetTextureID(m->mMaterialIndex);
    mVertexArrayPtr.push_back(mVertexArray[mVertexArray.size()-1].get());
    
    LoadAnimation();
    
    

}

// メッシュをロード（Boneなし）
void Mesh::CreateMesh(const aiMesh* m)
{
    
    std::vector<float> vertexBuffer;            // ３個ずつ（XYZ）
    std::vector<float> normalBuffer;            // ３個ずつ
    std::vector<float> uvBuffer;                // ２個ずつ
    std::vector<unsigned int> indexBuffer;

    for (unsigned int i = 0 ; i < m->mNumVertices ; i++)
    {
        vertexBuffer.push_back(m->mVertices[i].x);
        vertexBuffer.push_back(m->mVertices[i].y);
        vertexBuffer.push_back(m->mVertices[i].z);
        
        normalBuffer.push_back(m->mNormals[i].x);
        normalBuffer.push_back(m->mNormals[i].y);
        normalBuffer.push_back(m->mNormals[i].z);

        if (m->HasTextureCoords(0))
        {
            uvBuffer.push_back(m->mTextureCoords[0][i].x);
            uvBuffer.push_back(m->mTextureCoords[0][i].y);
        }
        else
        {
            uvBuffer.push_back(0.0f);
            uvBuffer.push_back(0.0f);
        }
    }
    
    // インデックスバッファ取り出し
    for (unsigned int i = 0 ; i < m->mNumFaces ; i++)
    {

        const aiFace& Face = m->mFaces[i];
        assert(Face.mNumIndices == 3);
        indexBuffer.push_back(Face.mIndices[0]);
        indexBuffer.push_back(Face.mIndices[1]);
        indexBuffer.push_back(Face.mIndices[2]);
    }

    mVertexArray.push_back(std::make_unique<VertexArray>(
            static_cast<unsigned int>(vertexBuffer.size()) / 3,
            vertexBuffer.data(),
            normalBuffer.data(),
            uvBuffer.data(),
            static_cast<unsigned int>(indexBuffer.size()),
            indexBuffer.data()) );
    
    mVertexArray[mVertexArray.size()-1]->SetTextureID(m->mMaterialIndex);
    mVertexArrayPtr.push_back(mVertexArray[mVertexArray.size()-1].get());
    
    
}

// Assimpを使ったモデルデータロード
bool Mesh::Load(const std::string& fileName, Renderer* r, bool isRightHanded)
{
    
    unsigned int ASSIMP_LOAD_FLAGS = aiProcess_Triangulate  | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes;
    
    if (isRightHanded)
    {
        ASSIMP_LOAD_FLAGS = ASSIMP_LOAD_FLAGS | aiProcess_FlipWindingOrder;
    }
    else
    {
        ASSIMP_LOAD_FLAGS = ASSIMP_LOAD_FLAGS | aiProcess_MakeLeftHanded;
    }
    
    // 読み込み（全データはaiSceneに格納される）
    mScene = mImporter.ReadFile(fileName.c_str(), ASSIMP_LOAD_FLAGS);
    if(!mScene)
    {
        return false;
    }


    //MatrixAi2Gl(mGlobalInverseTransform, mScene->mRootNode->mTransformation);
    //mGlobalInverseTransform.Invert();
    aiMatrix4x4 inv = mScene->mRootNode->mTransformation;
    inv = inv.Inverse();
    MatrixAi2Gl(mGlobalInverseTransform, inv);

    // 全メッシュ読み込み
    for (int cnt = 0; cnt < mScene->mNumMeshes; cnt++)
    {
        aiMesh* m = mScene->mMeshes[cnt];
        
        // ボーン情報有無
        if (m->HasBones())
        {
            CreateMeshBone(m);
            
        }
        else
        {
            CreateMesh(m);
        }
        
        
    }
    

    // マテリアル読み込み
  
    for (unsigned int cnt = 0; cnt < mScene->mNumMaterials; cnt++)
    {
        aiMaterial* pMaterial = mScene->mMaterials[cnt];

        // マテリアル作成
        std::shared_ptr<Material> mat = std::make_shared<Material>();

        // カラー情報
        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
        {
            mat->SetAmbientColor(Vector3(color.r, color.g, color.b));
        }
        if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
        {
            mat->SetDiffuseColor(Vector3(color.r, color.g, color.b));
        }
        if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color))
        {
            mat->SetSpecularColor(Vector3(color.r, color.g, color.b));
        }

        // Shininess
        float shininess = 32.0f;
        if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_SHININESS, shininess))
        {
            mat->SetSpecPower(shininess);
        }

        // Diffuse テクスチャ
        aiString path;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            std::string texPath = path.C_Str();
            
            if (texPath[0] == '*') // 埋め込みテクスチャ
            {
                int index = std::atoi(texPath.c_str() + 1);
                if (index >= 0 && index < (int)mScene->mNumTextures)
                {
                    aiTexture* aiTex = mScene->mTextures[index];
                    std::string key = "EMBED_" + std::to_string(index);
                    
                    const uint8_t* imageData = reinterpret_cast<const uint8_t*>(aiTex->pcData);
                    size_t imageSize = (aiTex->mHeight == 0)
                    ? aiTex->mWidth // compressed
                    : aiTex->mWidth * aiTex->mHeight * 4; // uncompressed (RGBA8888)
                    
                    Texture* tex = r->GetEmbeddedTexture(key, imageData, imageSize);
                    if (tex)
                    {
                        mat->SetDiffuseMap(std::shared_ptr<Texture>(tex, [](Texture*) {}));
                    }
                }
            }
            else
            {
                // 通常のファイルから読み込む
                std::string textureFile = ASSETS_PATH + texPath;
                Texture* tex = r->GetTexture(textureFile);
                if (tex)
                {
                    mat->SetDiffuseMap(std::shared_ptr<Texture>(tex, [](Texture*) {}));
                }
            }
        }

        mMaterials.push_back(mat);
    }
    
/*
    for (unsigned int cnt = 0; cnt < mScene->mNumMaterials; cnt++){
        aiMaterial* pMaterial = mScene->mMaterials[cnt];
        auto mat = std::make_shared<Material>();
        
        // Dffuseテクスチャ
        aiString Path;
        Texture* t;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
        {
            std::string texturefilename = Path.data;
            // テクスチャ読み込み
            t = r->GetTexture(ASSETS_PATH + texturefilename);
        }
        else
        {
            t = r->GetTexture(ASSETS_PATH + "Default.png");
        }
        mTextures.push_back(t);
        
    }
    */
    
    return true;
}


// データ破棄
void Mesh::Unload()
{
    mScene = nullptr;
    mVertexArray.clear();
}

// テクスチャIDからGetter
Texture* Mesh::GetTexture(size_t index)
{
    if (index < mTextures.size())
    {
        return mTextures[index];
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr<Material> Mesh::GetMaterial(size_t index)
{
    if (index < mMaterials.size())
    {
        return mMaterials[index];
    }
    return nullptr;
}






