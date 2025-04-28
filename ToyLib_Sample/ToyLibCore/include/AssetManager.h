#pragma once
#include <map>
#include <memory>

class AssetManager
{
public:
    AssetManager();
    
    // メッシュ登録
    std::shared_ptr<class Mesh> GetMesh(const std::string& fileName, bool isRightHanded = false);
    // テクスチャ登録
    std::shared_ptr<class Texture> GetTexture(const std::string& fileName);
    std::shared_ptr<class Texture> GetEmbeddedTexture(const std::string& nameKey, const uint8_t* data, size_t dataSize);
    
    std::shared_ptr<class SoundEffect> GetSoundEffect(const std::string& fileName);
    std::shared_ptr<class Music> GetMusic(const std::string& fileName);
    
    void UnloadData();
private:
    // アセット
    std::unordered_map<std::string, std::shared_ptr<class Texture>> mTextures;
    std::unordered_map<std::string, std::shared_ptr<class Mesh>> mMeshes;
    
    std::unordered_map<std::string, std::shared_ptr<class SoundEffect>> mSoundEffects;
    std::unordered_map<std::string, std::shared_ptr<class Music>> mMusics;

};
