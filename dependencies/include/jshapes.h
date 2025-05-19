
struct Color
{
    float r;
    float g;
    float b;

    Color operator*(float scale) const
    {
        Color newCol;

        newCol.r = r * scale;
        newCol.g = g * scale;
        newCol.b = b * scale;

        return newCol;
    }

    Color operator*(const Color& other) const
    {
        Color newCol;

        newCol.r = r * other.r;
        newCol.g = g * other.g;
        newCol.b = b * other.b;

        return newCol;
    }

    Color operator+(const Color &other) const
    {
        Color newCol;

        newCol.r = r + other.r;
        newCol.g = g + other.g;
        newCol.b = b + other.b;

        return newCol;
    }

    friend Color operator*(float scale, const Color &color)
    {
        return color * scale;
    }
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

    // New colors
    constexpr Color LightPink = {255, 182, 193};        // Light Pink
    constexpr Color LightCoral = {240, 128, 128};       // Light Coral
    constexpr Color PeachPuff = {255, 218, 185};        // Peach Puff
    constexpr Color PaleGreen = {152, 251, 152};        // Pale Green
    constexpr Color Aquamarine = {127, 255, 212};       // Aquamarine
    constexpr Color MediumPurple = {147, 112, 219};     // Medium Purple
    constexpr Color Plum = {221, 160, 221};             // Plum
    constexpr Color SlateBlue = {106, 90, 205};         // Slate Blue
    constexpr Color LightSkyBlue = {135, 206, 250};     // Light Sky Blue
    constexpr Color LightGreen = {144, 238, 144};       // Light Green
    constexpr Color MediumSeaGreen = {60, 179, 113};    // Medium Sea Green
    constexpr Color LimeGreen = {50, 205, 50};          // Lime Green
    constexpr Color CornflowerBlue = {100, 149, 237};   // Cornflower Blue
    constexpr Color DarkOliveGreen = {85, 107, 47};     // Dark Olive Green
    constexpr Color RoyalBlue = {65, 105, 225};         // Royal Blue
    constexpr Color SteelBlue = {70, 130, 180};         // Steel Blue
    constexpr Color PaleTurquoise = {175, 238, 238};    // Pale Turquoise
    constexpr Color SkyBlue = {135, 206, 235};          // Sky Blue
    constexpr Color MediumAquamarine = {102, 205, 170}; // Medium Aquamarine
    constexpr Color Turquoise = {64, 224, 208};         // Turquoise
    constexpr Color DarkTurquoise = {0, 206, 209};      // Dark Turquoise
    constexpr Color CadetBlue = {95, 158, 160};         // Cadet Blue
    constexpr Color LightSeaGreen = {32, 178, 170};     // Light Sea Green
    constexpr Color MediumTurquoise = {72, 209, 204};   // Medium Turquoise
}

/*
// Define 5 pyramid vertices
    Vertex base0 = {-0.5f, -0.5f, 0.0f, StandardColors::Cyan};  // Bottom-left
    Vertex base1 = {0.5f, -0.5f, 0.0f, StandardColors::Blue};   // Bottom-right
    Vertex base2 = {0.5f, 0.5f, 0.0f, StandardColors::Green};   // Top-right
    Vertex base3 = {-0.5f, 0.5f, 0.0f, StandardColors::Violet}; // Top-left
    Vertex apex = {0.0f, 0.0f, 1.0f, StandardColors::Red};      // Apex (center above base)

    // Define 2 base triangles (square base)
    Triangle b1 = {base0, base1, base2};
    Triangle b2 = {base0, base2, base3};

    // Define 4 side triangles (triangular faces)
    Triangle s1 = {base0, base1, apex}; // Front face
    Triangle s2 = {base1, base2, apex}; // Right face
    Triangle s3 = {base2, base3, apex}; // Back face
    Triangle s4 = {base3, base0, apex}; // Left face

    std::vector<Triangle> pyramidTriangles = {
        b1, b2, s1, s2, s3, s4};


//  Sphere testing

    std::vector<Vertex> sphereVertices;
    std::vector<Triangle> sphereTriangles;

    const int stacks = 20; // latitude divisions
    const int slices = 20; // longitude divisions
    const float radius = 1.0f;

    std::srand(std::time(0));
    // Generate vertices
    for (int i = 0; i <= stacks; ++i)
    {
        float phi = 3.141519 * i / stacks;
        float y = cos(phi);
        float r = sin(phi);

        for (int j = 0; j <= slices; ++j)
        {
            Color newCol;
            int range = 255 - 0 + 1;
            int red = rand() % range + 0, green = rand() % range + 0, blue = rand() % range + 0;
            newCol.r = red; newCol.b = blue; newCol.g = green;

            float theta = 2 * 3.141519 * j / slices;
            float x = r * cos(theta);
            float z = r * sin(theta);
            sphereVertices.push_back({x * radius, y * radius, z * radius, newCol});
        }
    }

    // Triangulate
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            Vertex v1 = sphereVertices[first];
            Vertex v2 = sphereVertices[second];
            Vertex v3 = sphereVertices[first + 1];
            Vertex v4 = sphereVertices[second + 1];

            // Triangle 1
            sphereTriangles.push_back({v1, v2, v3});

            // Triangle 2
            sphereTriangles.push_back({v3, v2, v4});
        }
    }
*/