#pragma once

#include "MathUtils.h"

#include <GL/glew.h>
#include <string>
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>
#include <iostream>


// シェーダー管理用のクラス
class Shader
{
public:
    Shader();
    ~Shader();
    // シェーダープログラムを読み込み
    bool Load(const std::string& vertName, const std::string& fragName);
    void Unload();
    // OpenGLにセット
    void SetActive();

    // uniform　マトリックス、ベクトル、値
    void SetMatrixUniform(const char* name, const Matrix4& matrix);
    void SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count);
    void SetVectorUniform(const char* name, const Vector3& vector);
    void SetFloatUniform(const char* name, float value);
    void SetBooleanUniform(const char* name, bool value);
    void SetTextureUniform(const char* name, GLuint textureUnit);
    
private:
    // 頂点シェーダーのID
    GLuint mVertexShaderID;
    // フラグメントシェーダーのID
    GLuint mFragShaderID;
    // シェーダープログラムのID
    GLuint mShaderProgramID;

    // シェーダーをコンパイル
    bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);
    // コンパイル済みかチェック
    bool IsCompiled(GLuint shader);
    // リンクできているかチェック
    bool IsValidProgram();
    

};


