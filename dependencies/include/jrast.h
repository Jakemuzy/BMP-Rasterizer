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
};

struct Triangle
{
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;
};

void isBigEndian()
{
    uint16_t num = 1;
    if (*(reinterpret_cast<uint8_t *>(&num)) == 0)
    {
        std::cout << "System is Big Endian: Program will not function correclty\n";
        std::abort();
    }
}

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

//  Fill triangle, including color lerp
void lerpFillTriangle(Triangle tri, InfoHeader &infoHeader, Header &header, std::vector<uint8_t>& frameBuffer, std::vector<uint8_t> &zBuffer, float zNear, float zFar)
{
    //  Colors
    Color col1 = tri.vert1.color;
    Color col2 = tri.vert2.color;
    Color col3 = tri.vert3.color;

    //  Sort by X values then Y to get bounding box
    Vertex v0 = tri.vert1;
    Vertex v1 = tri.vert2;
    Vertex v2 = tri.vert3;

    int startX = std::floor(std::min({v0.x, v1.x, v2.x}));
    int endX = std::ceil(std::max({v0.x, v1.x, v2.x}));

    int startY = std::floor(std::min({v0.y, v1.y, v2.y}));
    int endY = std::ceil(std::max({v0.y, v1.y, v2.y}));

    //  Drawing based on the bound box
    for(int x = startX; x <= endX; x++){
        for(int y = startY; y <= endY; y++)
        {
            //  Pixels off screen
            if (x < 0 || x >= infoHeader.width || y < 0 || y >= infoHeader.height)
                continue;

            float w0, w1, w2;
            barycentric(x + 0.5f, y + 0.5f, v0, v1, v2, w0, w1, w2);

            //  Not in triangle
            if (w0 < 0 || w1 < 0 || w2 < 0) 
            {
                continue;
            }

            //  Draw to the screen
            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP

            int zIndex = correctedY * infoHeader.width + x;
            int index = correctedY * infoHeader.width * 3 + x * 3; // +3 for 3 bytes per pixel
            
            //  Depth testing
            uint8_t currentDepth;
            currentDepth = zBuffer[zIndex];

            float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
            depth = std::clamp(depth, 0.0f, 1.0f);
            uint8_t newDepth = static_cast<uint8_t>(depth * 255.0f);

            if(newDepth <= currentDepth)
            {
                continue;
            }

            //  Draw to the screen buffer

            Color newCol = col1 * w0 + col2 * w1 + col3 * w2;
            newCol.r = std::clamp(newCol.r, 0.0f, 255.0f);
            newCol.g = std::clamp(newCol.g, 0.0f, 255.0f);
            newCol.b = std::clamp(newCol.b, 0.0f, 255.0f);

            zBuffer[zIndex] = newDepth;

            frameBuffer[index + 0] = static_cast<uint8_t>(newCol.b);
            frameBuffer[index + 1] = static_cast<uint8_t>(newCol.g);
            frameBuffer[index + 2] = static_cast<uint8_t>(newCol.r);

        }
    }
}

