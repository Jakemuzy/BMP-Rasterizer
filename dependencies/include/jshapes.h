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