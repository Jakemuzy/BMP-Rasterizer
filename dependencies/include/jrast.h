#include <optional>

enum LightType {
    POINT_LIGHT, SPOT_LIGHT, DIRECTIONAL_LIGHT
};

struct Vertex
{
    float x;    //  Make this vec3
    float y;
    float z;
    Color color;

    //  Optional texture coords
    std::optional<float> texX;
    std::optional<float> texY;

    // Optional Normals
    Vec3 normal;

    // World Pos
    Vec3 worldPos;

    //  Perspective corection (calculated before projection matrix applied)
    float invZ;
    float texX_divZ;
    float texY_divZ;

    // Value Checker
    bool hasValue() const
    {
        return (texX.has_value() && texY.has_value());
    }


};

struct Triangle
{
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;
};

struct PixelData
{
    Color color;
    Vec2 texCoords;
    Vec3 normal;
    float depth;

    // World Pos for lighting
    Vec3 worldPos;
    Vec2 screenCoords;
};

void writeImgBuffer(std::vector<uint8_t>& texBuffer, std::string fileName, InfoHeader infoHeader, Header header){
    std::ifstream texFile(fileName);
    texFile.seekg(header.dataOffset);

    texBuffer.resize(infoHeader.imageSize);
    texFile.read(reinterpret_cast<char *>(texBuffer.data()), infoHeader.imageSize);

    texFile.close();
}

//  Rasterizer Settings and file specific functions
namespace 
{
    //  Boolean flags
    bool interpolateColor = false, useTextureMap = false, useLighting = false, depthTesting = true;

    //  Buffers
    std::vector<uint8_t> *frameBuffer = nullptr;
    std::vector<uint8_t> *depthBuffer = nullptr;
    std::vector<uint8_t> *textureMap = nullptr;
    std::vector<uint8_t> *normalMap = nullptr;

    int texWidth = 0, texHeight = 0;

    //  Lighting Settings
    //  Light type (point, spot, etc)
    
    LightType lightType;
    float ambientStrength = 0.2f;
    float diffuseStrength = 0.5f;
    float specularStrength = 0.5f;
    float shininess = 32;
    Vec3 lightColor = {1.0f, 1.0f, 1.0f};
    Vec3 lightPos = {2.0f, 1.0f, 3.0f};
    Vec3 camPos; 


    //Functions

    void pixelShader(PixelData& data, InfoHeader& infoHeader)
    {

        int correctedY = (infoHeader.height - 1) - data.screenCoords.y;          // flip Y for BMP
        int index = correctedY * infoHeader.width * 3 + data.screenCoords.x * 3; // +3 for 3 bytes per pixel

        //  Blending weight (Should add up to 1)
        float colorWeight = 0.05f;
        float texWeight = 0.5f;
        float lightWeight = 0.5f;

        //  Depth testing before anything
        if (depthTesting)
        {
            (*depthBuffer)[index + 0] = data.depth;
            (*depthBuffer)[index + 1] = data.depth;
            (*depthBuffer)[index + 2] = data.depth;
        }

        Color lerpColor = StandardColors::White;
        Color texColor = StandardColors::White;
        
        // Blend all the colors
        Color finalColor = {0, 0, 0};
        float totalWeight = 0.0f;  

        if(interpolateColor)
        {

            lerpColor.r = std::clamp(data.color.r, 0.0f, 255.0f);
            lerpColor.g = std::clamp(data.color.g, 0.0f, 255.0f);
            lerpColor.b = std::clamp(data.color.b, 0.0f, 255.0f);

            finalColor = finalColor + lerpColor * colorWeight;
            totalWeight += colorWeight;
        }
        else    // Red if no color interpolation            //TESTING FLAT COLORS
        {
            lerpColor.r = 255;
            lerpColor.g = 0;
            lerpColor.b = 0;

            //  No weight, flat shading
            finalColor = finalColor + lerpColor * colorWeight;
            totalWeight += colorWeight;
        }
            

        if (useTextureMap)
        {
            int texIndex = data.texCoords.y * texWidth * 3 + data.texCoords.x * 3;

            texColor.b = std::clamp((int)(*textureMap)[texIndex + 0], 0, 255);
            texColor.g = std::clamp((int)(*textureMap)[texIndex + 1], 0, 255);
            texColor.r = std::clamp((int)(*textureMap)[texIndex + 2], 0, 255);

            finalColor = finalColor + texColor * texWeight;
            totalWeight += texWeight;
        }

        if (useLighting)
        {
            //  Could use normal map 

            //  Ambient Lighting
            Vec3 objColor = {finalColor.r, finalColor.g, finalColor.b};
            objColor = objColor.normalize();
            Vec3 ambient = objColor * lightColor * ambientStrength;

            //  Diffuse Lighting
            Vec3 norm = data.normal.normalize();
            Vec3 lightDirection = (lightPos - data.worldPos).normalize();

            //std::cout << norm.x << " " << norm.y << " " << norm.z << " " << " \n";

            float diff = norm.dot(lightDirection);

            if(diff < 0)
                diff = 0;
            Vec3 diffuse = lightColor * diff * diffuseStrength;

            //  Specular
            Vec3 reflectDir = (norm * (2.0f * norm.dot(-lightDirection))) - lightDirection;
            reflectDir = reflectDir.normalize();

            Vec3 viewDir = (camPos - data.worldPos).normalize();
            float spec = std::max(viewDir.dot(reflectDir), 0.0f);
            spec = pow(spec, shininess);
            Vec3 specular = lightColor * specularStrength * spec;

            Vec3 newColor = objColor * (ambient + diffuse + specular);
            newColor.x = std::clamp(newColor.x, 0.0f, 1.0f);
            newColor.y = std::clamp(newColor.y, 0.0f, 1.0f);
            newColor.z = std::clamp(newColor.z, 0.0f, 1.0f);

            finalColor = finalColor + Color{newColor.x, newColor.y, newColor.z} * (255 * lightWeight);

            //std::cout << finalColor.r << " " << finalColor.g << " " << finalColor.b << "\n";
            totalWeight += lightWeight;
        }

        // Normalize the final color
        if (totalWeight > 0.0f)
        {
            finalColor = finalColor * (1.0f / totalWeight);
        }

        //  Clamp and write colors
        finalColor.r = std::clamp(finalColor.r, 0.0f, 255.0f);
        finalColor.g = std::clamp(finalColor.g, 0.0f, 255.0f);
        finalColor.b = std::clamp(finalColor.b, 0.0f, 255.0f);
        (*frameBuffer)[index + 0] = static_cast<uint8_t>(finalColor.b);
        (*frameBuffer)[index + 1] = static_cast<uint8_t>(finalColor.g);
        (*frameBuffer)[index + 2] = static_cast<uint8_t>(finalColor.r);

    }

    //  Weights for each point inside a triangle
    void barycentric(float px, float py, const Vertex &v0, const Vertex &v1, const Vertex &v2, float &w0, float &w1, float &w2)
    {
        // Compute the area of the triangle (using the determinant)
        float denom = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);

        //  No divide by 0
        if (std::abs(denom) < 1e-5f)
        {
            w0 = w1 = w2 = -1.0f;
            return;
        }

        // Compute the barycentric weights
        w0 = ((v1.y - v2.y) * (px - v2.x) + (v2.x - v1.x) * (py - v2.y)) / denom;
        w1 = ((v2.y - v0.y) * (px - v2.x) + (v0.x - v2.x) * (py - v2.y)) / denom;
        w2 = 1.0f - w0 - w1;

        return;
    }

    bool depthTest(int index, float zFar, const Vertex &v0, const Vertex &v1, const Vertex &v2, float w0, float w1, float w2, uint8_t& newDepth)
    {
        //  Depth testing
        uint8_t currentDepth;
        currentDepth = (*depthBuffer)[index];

        float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
        depth = std::clamp(depth, 0.0f, 1.0f);
        newDepth = static_cast<uint8_t>(depth * 255.0f);

        if (newDepth <= currentDepth || newDepth < zFar)
            return false;

        return true;
    }

    void interpolateData(const Triangle &tri, InfoHeader &infoHeader, float zNear, float zFar)
    {
        //  Colors
        Color col1 = tri.vert1.color;
        Color col2 = tri.vert2.color;
        Color col3 = tri.vert3.color;

        //  Bounding Box
        const Vertex &v0 = tri.vert1;
        const Vertex &v1 = tri.vert2;
        const Vertex &v2 = tri.vert3;

        int startX = std::floor(std::min({v0.x, v1.x, v2.x}));
        int endX = std::ceil(std::max({v0.x, v1.x, v2.x}));

        int startY = std::floor(std::min({v0.y, v1.y, v2.y}));
        int endY = std::ceil(std::max({v0.y, v1.y, v2.y}));

        //  Drawing based on the bound box
        for (int x = startX; x <= endX; x++)
        {
            for (int y = startY; y <= endY; y++)
            {

                PixelData pixel;

                //  Pixels off screen
                if (x < 0 || x >= infoHeader.width || y < 0 || y >= infoHeader.height)
                    continue;

                float w0, w1, w2;
                barycentric(x + 0.5f, y + 0.5f, v0, v1, v2, w0, w1, w2);

                //  Not in triangle
                if (w0 < 0 || w1 < 0 || w2 < 0)
                    continue;

                //  Draw to the screen
                int correctedY = (infoHeader.height - 1) - y;          // flip Y for BMP
                int index = correctedY * infoHeader.width * 3 + x * 3; // +3 for 3 bytes per pixel

                //  Depth testing
                
                uint8_t newDepth;
                if (!depthTest(index, zFar, v0, v1, v2, w0, w1, w2, newDepth))
                    continue;
                pixel.depth = newDepth;

                //  Each Pixel in world pos
                pixel.worldPos.x = w0 * v0.worldPos.x + w1 * v1.worldPos.x + w2 * v2.worldPos.x;
                pixel.worldPos.y = w0 * v0.worldPos.y + w1 * v1.worldPos.y + w2 * v2.worldPos.y;
                pixel.worldPos.z = w0 * v0.worldPos.z + w1 * v1.worldPos.z + w2 * v2.worldPos.z;

                //  Color data
                Color newCol;
                newCol = col1 * w0 + col2 * w1 + col3 * w2;
                pixel.color = newCol;

                //  Texture coordinates if enabled
                if ((v0.hasValue() && v1.hasValue() && v2.hasValue() && useTextureMap))
                {
                    Vec2 newTexCoords;
                    float invZ_interp = w0 * v0.invZ + w1 * v1.invZ + w2 * v2.invZ;
                    if (invZ_interp < 1e-5f)
                        continue;

                    float texX = (w0 * v0.texX_divZ + w1 * v1.texX_divZ + w2 * v2.texX_divZ) / invZ_interp;
                    float texY = (w0 * v0.texY_divZ + w1 * v1.texY_divZ + w2 * v2.texY_divZ) / invZ_interp;

                    // Convert to pixel coords
                    int texX_px = std::clamp(static_cast<int>(texX * texWidth), 0, texWidth - 1);
                    int texY_px = std::clamp(static_cast<int>(texY * texHeight), 0, texHeight - 1);
                    newTexCoords = {(float)texX_px, (float)texY_px};
                    pixel.texCoords = newTexCoords;
                }

                //  Normal values if enabled
                if (useLighting && normalMap == nullptr) 
                {
                    //  Also have settings for normal map
                    Vec3 newNormal;
                    newNormal = (v0.normal * w0 + v1.normal * w1 + v2.normal * w2).normalize();
                    pixel.normal = newNormal;
                }
                else if (useLighting && normalMap != nullptr)
                {

                }

                Vec2 screenPos = {(float)x, (float)y};
                pixel.screenCoords = screenPos;
                pixelShader(pixel, infoHeader);
            }
        }
    }
}

//  Static rasterizer for rasterization settings and functions 
namespace Rasterizer 
{
    
    //  Rasterizer Settings 
    static void enableInterpolation(bool value)
    {
        interpolateColor = value;
    }

    static void enableTextureMapping(bool value)
    {
        useTextureMap = value;
    }

    static void enableLighting(bool value) 
    {
        useLighting = value;
    }

    static void lightingSettings(LightType type, float diffuse, float ambient, float specular)
    {
        if(!useLighting)
        {
            std::cerr << "RASTERIZER ERROR: lighting not enabled, but lighting settings are specified\n";
        }
        lightType = type;
        diffuseStrength = diffuse;
        ambientStrength = ambient;
        specularStrength = specular;
    }

    bool isIntepolated() { return interpolateColor; }
    bool isTextureMapped() { return useTextureMap; }
    bool isUsingLighting() { return useLighting; }

    //  Setting buffers

    void attachFramebuffer(std::vector<uint8_t>& _frameBuffer)
    {
        frameBuffer = &_frameBuffer;
    }

    void attachDepthBuffer(std::vector<uint8_t>& _depthBuffer)
    {
        depthBuffer = &_depthBuffer;
    }

    void setTextureMap(std::vector<uint8_t>& _texMap, int width, int height)
    {
        if (!useTextureMap)
        {
            std::cerr << "RASTERIZER ERROR: Texture mapping not enabled, but texture map is being set\n";
        }

        textureMap = &_texMap;
        texWidth = width;
        texHeight = height;
    }

    void setNormalMap(std::vector<uint8_t> &_depthBuffer, int width, int height)
    {
        if (!useLighting)
        {
            std::cerr << "RASTERIZER ERROR: Normals not enabled\n";
        }
    }

    void setCamPos(Vec3 _camPos)
    {
        if (!useLighting)
        {
            std::cerr << "RASTERIZER ERROR: lighting not enabled, but atempting to set camera direction\n";
        }
        camPos = _camPos;
    }

    //  Functions

    void clearScreen() 
    {
        //  Resets framebuffer and depthBuffer
        std::fill((*frameBuffer).begin(), (*frameBuffer).end(), 255);
        std::fill((*depthBuffer).begin(), (*depthBuffer).end(), 0);
    }

    void updateScreen(std::string currentBufferFilename, Header& header, InfoHeader& infoHeader)
    {
        //  Start editing after 54 bytes (header info)
        int offset = header.dataOffset;

        std::fstream file(currentBufferFilename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(offset);
        file.seekg(offset);
        file.write(reinterpret_cast<const char *>((*frameBuffer).data()), (*frameBuffer).size());

        std::fstream depthMap("dependencies/frames/depthMap.bmp", std::ios::binary | std::ios::in | std::ios::out);
        depthMap.seekp(offset);
        depthMap.seekg(offset);
        depthMap.write(reinterpret_cast<const char *>((*depthBuffer).data()), (*depthBuffer).size());
        depthMap.write(reinterpret_cast<const char *>((*frameBuffer).data()), (*frameBuffer).size());

        file.close();
        depthMap.close();
    }

    //  Fill triangle, including color lerp
    void drawTriangle(const Triangle& tri, InfoHeader &infoHeader, Header &header, float zNear, float zFar)
    {

        interpolateData(tri, infoHeader, zNear, zFar);

        //Interpolate Data
            //Interpolate Color
            //Interpolate Normal
            //Interpolate TexCoords

        //PixelShader
            //Based on flags choose to do these calculations or not

        return; 
    }

    
};