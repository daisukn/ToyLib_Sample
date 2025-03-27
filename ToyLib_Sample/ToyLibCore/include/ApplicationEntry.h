#pragma once

#include <memory>
#include "Application.h"

// CreateUserApplication() の宣言が必要！
std::unique_ptr<Application> CreateUserApplication();

// マクロ定義（引数なし/あり 両方可）
#define TOYLIB_REGISTER_APP(AppType)                          \
    std::unique_ptr<Application> CreateUserApplication() {    \
        return std::make_unique<AppType>();                   \
    }

#define TOYLIB_REGISTER_APP_WITH_ARGS(AppType, ...)           \
    std::unique_ptr<Application> CreateUserApplication() {    \
        return std::make_unique<AppType>(__VA_ARGS__);        \
    }
