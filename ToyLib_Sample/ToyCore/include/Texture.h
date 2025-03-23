#pragma once


#include <string>

// テクスチャを管理
class Texture
{
public:
	Texture();
	~Texture();
	
    // テクスチャ読み込み（SDL2_image）
	bool Load(const std::string& fileName);
    
    void CreateForRendering(int w, int h, unsigned int format);
    
    // 削除
	void Unload();
    // 描画用にBind
	void SetActive();
	// 幅と高さ取得
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }

private:
    // OpenGLが管理するID
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};


