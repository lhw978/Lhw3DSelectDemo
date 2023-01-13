#pragma once
#pragma once
#include"all.h"
#include"filesystem"
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(std::vector<std::string> faces);
unsigned int TextureFromFile(const char* path, const std::string& directory);