struct Color
{
    float r;
    float g;
    float b;
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
        abort;
    }
}

//  Takes 2 colors, and the change per frame
Color lerpColor(Color col1, Color col2, Color col3, float blendRatio[3])
{

    float newColB = (col1.b / 255) * blendRatio[0] + (col2.b / 255) * blendRatio[1] + (col3.b / 255) * blendRatio[2];
    float newColG = (col1.g / 255) * blendRatio[0] + (col2.g / 255) * blendRatio[1] + (col3.g / 255) * blendRatio[2];
    float newColR = (col1.r / 255) * blendRatio[0] + (col2.r / 255) * blendRatio[1] + (col3.r / 255) * blendRatio[2];

    Color newCol = {newColB * 255, newColG * 255, newColR * 255};

    return newCol;
}

//
void lerpFillTriangle(Triangle tri, std::string currentBuffer, InfoHeader &infoHeader, Header &header, std::fstream &file)
{
    //  Colors
    Color col1 = tri.vert1.color;
    Color col2 = tri.vert2.color;
    Color col3 = tri.vert3.color;

    //  Sort by Y values
    Vertex v0 = tri.vert1;
    Vertex v1 = tri.vert2;
    Vertex v2 = tri.vert3;

    if (v1.y < v0.y)
        std::swap(v0, v1);
    if (v2.y < v0.y)
        std::swap(v0, v2);
    if (v2.y < v1.y)
        std::swap(v1, v2);

    //  Separate Triangle type
    if (v0.y == v1.y) // Flat top
    {
        //  Slope for left and right side of triangle
        float m1 = (float)(v2.x - v0.x) / (v2.y - v0.y);
        float m2 = (float)(v2.x - v1.x) / (v2.y - v1.y);

        //  Rasterize the triangle horizontally (Scanline) from the bottom up
        for (int y = v0.y; y <= v2.y; y++)
        { // might not need <= here because vert already rendered I think

            int xStart = v0.x + m1 * (y - v0.y);
            int xEnd = v1.x + m2 * (y - v1.y);

            if (xEnd < xStart)
            {
                std::swap(xStart, xEnd);
            }

            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP
            for (int x = xStart; x <= xEnd; x++)
            {

                int rowSize = ((infoHeader.width * 3 + 3) / 4) * 4; // each row padded to multiple of 4
                int index = header.dataOffset + correctedY * rowSize + static_cast<int>(x) * 3;
                file.seekp(index, std::ios::beg);

                //  Calculate color blending by distance to each vert
                float dist1 = sqrt((v0.x - x) * (v0.x - x) + (v0.y - y) * (v0.y - y));
                float dist2 = sqrt((v1.x - x) * (v1.x - x) + (v1.y - y) * (v1.y - y));
                float dist3 = sqrt((v2.x - x) * (v2.x - x) + (v2.y - y) * (v2.y - y));

                float inv1 = 1.0 / (dist1 + 1e-6f); // add epsilon to avoid div-by-zero
                float inv2 = 1.0 / (dist2 + 1e-6f);
                float inv3 = 1.0 / (dist3 + 1e-6f);

                float totalInvDist = inv1 + inv2 + inv3;

                float colBlend[3] = {inv1 / totalInvDist, inv2 / totalInvDist, inv3 / totalInvDist};
                Color newCol = lerpColor(col1, col2, col3, colBlend); // Ratio for each color to take

                file.put(newCol.b);
                file.put(newCol.g);
                file.put(newCol.r);
            }
        }
    }
    else if (v1.y == v2.y) // Flat bottom
    {

        //  Slope for left and right side of triangle
        float m1 = (float)(v0.x - v1.x) / (v0.y - v1.y);
        float m2 = (float)(v0.x - v2.x) / (v0.y - v2.y);

        //  Rasterize the triangle horizontally (Scanline) from the top down
        for (int y = v0.y; y <= v1.y; y++)
        {

            int xStart = v1.x + m1 * (y - v1.y);
            int xEnd = v2.x + m2 * (y - v2.y);

            if (xEnd < xStart)
            {
                std::swap(xStart, xEnd);
            }

            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP
            for (int x = xStart; x <= xEnd; x++)
            {

                int rowSize = ((infoHeader.width * 3 + 3) / 4) * 4; 
                int index = header.dataOffset + correctedY * rowSize + static_cast<int>(x) * 3;
                file.seekp(index, std::ios::beg);

                //  Calculate color blending by distance to each vert
                float dist1 = sqrt((v0.x - x) * (v0.x - x) + (v0.y - y) * (v0.y - y));
                float dist2 = sqrt((v1.x - x) * (v1.x - x) + (v1.y - y) * (v1.y - y));
                float dist3 = sqrt((v2.x - x) * (v2.x - x) + (v2.y - y) * (v2.y - y));

                float inv1 = 1.0 / (dist1 + 1e-6f); // add epsilon to avoid div-by-zero
                float inv2 = 1.0 / (dist2 + 1e-6f);
                float inv3 = 1.0 / (dist3 + 1e-6f);

                float totalInvDist = inv1 + inv2 + inv3;

                float colBlend[3] = {inv1 / totalInvDist, inv2 / totalInvDist, inv3 / totalInvDist};
                Color newCol = lerpColor(col1, col2, col3, colBlend); // Ratio for each color to take

                file.put(newCol.b);
                file.put(newCol.g);
                file.put(newCol.r);
            }
        }
    }
    else //  Split into two triangles by adding extra vert, one flat top, one flat bottom
    {

        //Calculate where to split triangle
        float m0 = (float)(v0.x - v2.x) / (v0.y - v2.y);
        float y0 = v1.y;
        float x0 = v0.x + m0 * (y0 - v0.y);

        float inv1 = 1.0 / (sqrt((v0.x - x0) * (v0.x - x0) + (v0.y - y0) * (v0.y - y0)) + 1e-6f);
        float inv2 = 1.0 / (sqrt((v1.x - x0) * (v1.x - x0) + (v1.y - y0) * (v1.y - y0)) + 1e-6f);
        float inv3 = 1.0 / (sqrt((v2.x - x0) * (v2.x - x0) + (v2.y - y0) * (v2.y - y0)) + 1e-6f);

        float totalInvDist = inv1 + inv2 + inv3;

        float colBlend[3] = {inv1 / totalInvDist, inv2 / totalInvDist, inv3 / totalInvDist};

        Vertex vi = {x0, v1.y, 0, lerpColor(col1, col2, col3, colBlend)};   //  Only need lerp 2 colors 

        //  Calculate flat bottom

        float m1 = (float)(v0.x - vi.x) / (v0.y - vi.y);
        float m2 = (float)(v0.x - v1.x) / (v0.y - v1.y);

        for (int y = v0.y; y <= v1.y; y++)
        {

            int xStart = v0.x + m1 * (y - v0.y);
            int xEnd = v0.x + m2 * (y - v0.y);

            if (xEnd < xStart)
            {
                std::swap(xStart, xEnd);
            }

            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP
            for (int x = xStart; x <= xEnd; x++)
            {

                int rowSize = ((infoHeader.width * 3 + 3) / 4) * 4;
                int index = header.dataOffset + correctedY * rowSize + static_cast<int>(x) * 3;
                file.seekp(index, std::ios::beg);

                //  Calculate color blending by distance to each vert
                float dist1 = sqrt((v0.x - x) * (v0.x - x) + (v0.y - y) * (v0.y - y));
                float dist2 = sqrt((v1.x - x) * (v1.x - x) + (v1.y - y) * (v1.y - y));
                float dist3 = sqrt((v2.x - x) * (v2.x - x) + (v2.y - y) * (v2.y - y));

                float inv1 = 1.0 / (dist1 + 1e-6f); // add epsilon to avoid div-by-zero
                float inv2 = 1.0 / (dist2 + 1e-6f);
                float inv3 = 1.0 / (dist3 + 1e-6f);

                float totalInvDist = inv1 + inv2 + inv3;

                float colBlend[3] = {inv1 / totalInvDist, inv2 / totalInvDist, inv3 / totalInvDist};
                Color newCol = lerpColor(col1, col2, col3, colBlend); // Ratio for each color to take

                file.put(newCol.b);
                file.put(newCol.g);
                file.put(newCol.r);
            }
        }

        //  Calculate flat top

        m1 = (float)(v2.x - vi.x) / (v2.y - vi.y);
        m2 = (float)(v2.x - v1.x) / (v2.y - v1.y);

        for (int y = vi.y; y <= v2.y; y++)
        { 

            int xStart = vi.x + m1 * (y - vi.y);
            int xEnd = v1.x + m2 * (y - v1.y);

            if (xEnd < xStart)
            {
                std::swap(xStart, xEnd);
            }

            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP
            for (int x = xStart; x <= xEnd; x++)
            {

                int rowSize = ((infoHeader.width * 3 + 3) / 4) * 4; // each row padded to multiple of 4
                int index = header.dataOffset + correctedY * rowSize + static_cast<int>(x) * 3;
                file.seekp(index, std::ios::beg);

                //  Calculate color blending by distance to each vert
                float dist1 = sqrt((v0.x - x) * (v0.x - x) + (v0.y - y) * (v0.y - y));
                float dist2 = sqrt((v1.x - x) * (v1.x - x) + (v1.y - y) * (v1.y - y));
                float dist3 = sqrt((v2.x - x) * (v2.x - x) + (v2.y - y) * (v2.y - y));

                float inv1 = 1.0 / (dist1 + 1e-6f); // add epsilon to avoid div-by-zero
                float inv2 = 1.0 / (dist2 + 1e-6f);
                float inv3 = 1.0 / (dist3 + 1e-6f);

                float totalInvDist = inv1 + inv2 + inv3;

                float colBlend[3] = {inv1 / totalInvDist, inv2 / totalInvDist, inv3 / totalInvDist};
                Color newCol = lerpColor(col1, col2, col3, colBlend); // Ratio for each color to take

                file.put(newCol.b);
                file.put(newCol.g);
                file.put(newCol.r);
            }
        }
    }
}

//  Linearly interpolate between vertices, including color and new pixel pos            -- CONVERT TO SCREEN COORDS BEFORE, THEN LERP
void lerpVerts(Vertex vert1, Vertex vert2, std::string currentBuffer, InfoHeader &infoHeader, Header &header, std::fstream &file)
{

    // Edge 1

    //  Lerp between two vertices

    int y1 = vert1.y;
    int y2 = vert2.y;
    Color col1 = vert1.color;

    int x1 = vert1.x;
    int x2 = vert2.x;
    Color col2 = vert2.color;

    // Ensure x1 <= x2
    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(col2, col1);
    }
    float m = (y2 - y1) / (float)(x2 - x1);
    float distX = x2 - x1;
    float distY = y2 - y1;

    //  Differences in colors
    int redDiff = col2.r - col1.r;
    int greenDiff = col2.g - col1.g;
    int blueDiff = col2.b - col1.b;

    if (distX == 0 && distY != 0)
    {

        for (int i = y1 + 1; i < y2; i++)
        {
            int newX = x1;
            int newY = i;

            int correctedY = (infoHeader.height - 1) - newY; // flip Y for BMP
            int index = header.dataOffset + newX * 3 + (correctedY * 3 * infoHeader.width);
            file.seekp(index, std::ios::beg);
            file.put(col1.b + ((i - x1 + 1) * blueDiff) / distY);
            file.put(col1.g + ((i - x1 + 1) * greenDiff) / distY);
            file.put(col1.r + ((i - x1 + 1) * redDiff) / distY);
        }
    }
    else
    {
        //  Not including already drawn
        for (int i = x1 + 1; i < x2; i++)
        {
            int newX = i;
            int newY = y1 + (m * (i - x1 + 1));

            int correctedY = (infoHeader.height - 1) - newY; // flip Y for BMP
            int index = header.dataOffset + newX * 3 + (correctedY * 3 * infoHeader.width);
            file.seekp(index, std::ios::beg);
            file.put(col1.b + ((i - x1 + 1) * blueDiff) / distX);
            file.put(col1.g + ((i - x1 + 1) * greenDiff) / distX);
            file.put(col1.r + ((i - x1 + 1) * redDiff) / distX);
        }
    }
}
