#include <optional>
struct Color
{
    float r;
    float g;
    float b;

    Color operator*(float scale) const{
        Color newCol;

        newCol.r = r*scale;
        newCol.g = g*scale;
        newCol.b = b*scale;

        return newCol;
    }

    Color operator+(const Color& other) const{
        Color newCol;

        newCol.r = r + other.r;
        newCol.g = g + other.g;
        newCol.b = b + other.b;

        return newCol;
    }
};

struct Vertex
{
    float x;
    float y;
    float z;
    Color color;

    //  Optional texture coords
    std::optional<float> texX;
    std::optional<float> texY;

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

//  Colors

namespace StandardColors
{
    constexpr Color Black = {0, 0, 0};
    constexpr Color White = {255, 255, 255};
    constexpr Color Red = {255, 0, 0};
    constexpr Color Orange = {255, 127, 0};
    constexpr Color Yellow = {255, 255, 0};
    constexpr Color Green = {0, 255, 0};
    constexpr Color Cyan = {0, 255, 255};
    constexpr Color Blue = {0, 0, 255};
    constexpr Color Purple = {128, 0, 128};
    constexpr Color Violet = {148, 0, 211};
    constexpr Color Teal = {0, 128, 128};
}

void writeImgBuffer(std::vector<uint8_t>& texBuffer, std::string fileName, InfoHeader infoHeader, Header header){
    std::ifstream texFile(fileName);
    texFile.seekg(header.dataOffset);

    texBuffer.resize(infoHeader.imageSize);
    texFile.read(reinterpret_cast<char *>(texBuffer.data()), infoHeader.imageSize);

    texFile.close();
}




//  Texture Calculations

/*
void mapTexture(Vertex v0, std::vector<uint8_t>& texImage, InfoHeader& infoHeader){
    float width = infoHeader.width;
    float height = texImage.size() /= infoHeader.width

    float ratio = infoHeader.width /= width
    v0.texX
}

*/

//  Lighting Calculations

/*
    1.) Have a triangle, each vertex with an optional normal value and optional TexCoords
    2.)

    Main Code

        bool flags interpolateColors, texMap, normals;

        Vertex(Vec3 Pos, Vec3 Normal, Vec3 Texture);    Normal and Tex are optoinal

        Vertex::InterpolateColors(true);
        Vertex::TexMap(true);
        Vertex::Normals(true)

    Vertex Processer Code

        static class VertexProcces()
        {
            private:
                bool flags interpolateColors, texMap, normals;

            public:

            //Flags
                void InterpolateColors(bool value)
                {
                    interpolateColors = true;
                }

                void TexMap(bool value) 
                {
                    texMap = true;
                }

                void normals(bool value)
                {
                    normals = true;
                }

            //Functions
                void processVertex(Triangle tri, Buffer frameBuffer, Buffer, depthBuffer)
                {

                }

        }

    Process
        1.) Check Flags
            1a.)    if(interpCol) then
                1aa.)   if(texMap) then combine colors and texmap
            1b.)    if(texMap) then do texture mapping
            1c.)    if(normals) then ALWAYS do lighting calculations 
        2.) Depending on flags do differnet interpolation functions
            2a.)    if(texMap && )



*/

//  Rasterizer Settings and file specific functions
namespace 
{
    //  Boolean flags
    static bool interpolateColor = false, useTextureMap = false, useNormals = false;

    //  Buffers
    std::vector<uint8_t> *frameBuffer = nullptr;
    std::vector<uint8_t> *depthBuffer = nullptr;
    std::vector<uint8_t> *textureMap = nullptr;
    std::vector<uint8_t> *normalMap = nullptr;

    int texWidth = 0, texHeight = 0;

    //Functions 

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

    void displayTexture(const Triangle& tri, InfoHeader &infoHeader, float zNear, float zFar)
    {
        //  Bounding Box
        const Vertex &v0 = tri.vert1;
        const Vertex &v1 = tri.vert2;
        const Vertex &v2 = tri.vert3;

        int startX = std::floor(std::min({v0.x, v1.x, v2.x}));
        int endX = std::ceil(std::max({v0.x, v1.x, v2.x}));

        int startY = std::floor(std::min({v0.y, v1.y, v2.y}));
        int endY = std::ceil(std::max({v0.y, v1.y, v2.y}));

        if (!(v0.hasValue() && v1.hasValue() && v2.hasValue()))
        {
            std::cerr << "RASTERIZER ERROR: Texture mapping enabled, but texture coordinates not set \n";
            std::abort();
        }

        for(int x = startX; x < endX; x++){
            for(int y = startY; y < endY; y++){
                //  Pixels off screen
                if (x < 0 || x >= infoHeader.width || y < 0 || y >= infoHeader.height)
                    continue;

                float w0, w1, w2;
                barycentric(x + 0.5f, y + 0.5f, v0, v1, v2, w0, w1, w2);

                //  Not in triangle
                if (w0 < 0 || w1 < 0 || w2 < 0)
                    continue;

                int correctedY = (infoHeader.height - 1) - y;          // flip Y for BMP
                int index = correctedY * infoHeader.width * 3 + x * 3; 

                //  Depth testing
                uint8_t currentDepth;
                currentDepth = (*depthBuffer)[index];

                float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
                depth = std::clamp(depth, 0.0f, 1.0f);
                uint8_t newDepth = static_cast<uint8_t>(depth * 255.0f);

                if (newDepth <= currentDepth || newDepth < zFar)
                    continue;

                (*depthBuffer)[index + 0] = newDepth;
                (*depthBuffer)[index + 1] = newDepth;
                (*depthBuffer)[index + 2] = newDepth;

                //  Map texture to triangle
                Color newCol;

                float invZ_interp = w0 * v0.invZ + w1 * v1.invZ + w2 * v2.invZ;
                if (invZ_interp < 1e-5f)
                    continue;

                float texX = (w0 * v0.texX_divZ + w1 * v1.texX_divZ + w2 * v2.texX_divZ) / invZ_interp;
                float texY = (w0 * v0.texY_divZ + w1 * v1.texY_divZ + w2 * v2.texY_divZ) / invZ_interp;

                // Convert to pixel coords
                int texX_px = std::clamp(static_cast<int>(texX * texWidth), 0, texWidth - 1);
                int texY_px = std::clamp(static_cast<int>(texY * texHeight), 0, texHeight - 1);
                int texIndex = texY_px * texWidth * 3 + texX_px * 3;

                newCol.g = std::clamp((int)(*textureMap)[texIndex + 0], 0, 255);
                newCol.b = std::clamp((int)(*textureMap)[texIndex + 1], 0, 255);
                newCol.r = std::clamp((int)(*textureMap)[texIndex + 2], 0, 255);

                (*frameBuffer)[index + 0] = static_cast<uint8_t>(newCol.b);
                (*frameBuffer)[index + 1] = static_cast<uint8_t>(newCol.g);
                (*frameBuffer)[index + 2] = static_cast<uint8_t>(newCol.r);
            }
        }
    }

    void lerpColor(const Triangle& tri,InfoHeader& infoHeader, float zNear, float zFar)
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
                uint8_t currentDepth;
                currentDepth = (*depthBuffer)[index];

                float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
                depth = std::clamp(depth, 0.0f, 1.0f);
                uint8_t newDepth = static_cast<uint8_t>(depth * 255.0f);

                //  Depth map rejection, near and far plane rejection       (Near not as important, ignoring for now )
                if (newDepth <= currentDepth || newDepth < zFar)
                    continue;

                //  Draw to the screen buffer

                (*depthBuffer)[index + 0] = newDepth;
                (*depthBuffer)[index + 1] = newDepth;
                (*depthBuffer)[index + 2] = newDepth;

                //  If textures are enabled calculate color differently
                Color newCol;
                newCol = col1 * w0 + col2 * w1 + col3 * w2;

                newCol.r = std::clamp(newCol.r, 0.0f, 255.0f);
                newCol.g = std::clamp(newCol.g, 0.0f, 255.0f);
                newCol.b = std::clamp(newCol.b, 0.0f, 255.0f);

                (*frameBuffer)[index + 0] = static_cast<uint8_t>(newCol.b);
                (*frameBuffer)[index + 1] = static_cast<uint8_t>(newCol.g);
                (*frameBuffer)[index + 2] = static_cast<uint8_t>(newCol.r);
            }
        }
    }
}


//  Static rasterizer for rasterization settings and functions 
namespace Rasterizer 
{
    
    //  Rasterizer Settings 
    static void useInterpolation(bool value)
    {
        interpolateColor = value;
    }

    static void useTextureMapping(bool value)
    {
        useTextureMap = value;
    }

    static void useLighting(bool value) 
    {
        useNormals = value;
    }

    static void lightingSettings()
    {

    }

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
            std::cerr << "RASTERIZER ERROR: Texture mapping not enabled\n";
        }

        textureMap = &_texMap;
        texWidth = width;
        texHeight = height;
    }

    void setNormalMap(std::vector<uint8_t> &_depthBuffer, int width, int height)
    {
        if (!useNormals)
        {
            std::cerr << "RASTERIZER ERROR: Normals not enabled\n";
        }
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
        //if(useInterpolation)
            //lerpColor(tri, infoHeader, zNear, zFar);
        if(useTextureMapping)
            displayTexture(tri, infoHeader, zNear, zFar);
    }

    
};