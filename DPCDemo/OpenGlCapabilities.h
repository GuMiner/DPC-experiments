#pragma once
#include <string>
#include <vector>

class OpenGlCapabilities
{
    const int MinCoreCount = 4;

    void GetRendererCapabilities();
    void GetTextureCapabilities();
    void GetUniformCapabilities();
    void LogRetrievedInformation();

public:
    // High-level capabilities
    int logicalCoreCount;

    std::string vendor;
    std::string renderer;
    std::string version;
    std::vector<std::string> shadingLanguageVersions;
    std::vector<std::string> extensions;

    // Texture capabilities
    int maxCombinedTextureImageUnits;
    int maxTextureCoordinateSets;
    int maxTextureSize;

    // Uniform capabilities
    int maxFragmentUniformBlocks;
    int maxVertexUniformBlocks;
    int maxUniformBlockSize;

    void Initialize();

    std::string GetPlatform();
    int GetMaxTextureUnits();
};
