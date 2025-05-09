#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

#include <CImg.h>   //  OUTSOURCED for bmp image preview

#include <jbmp.h>    //  Custom built bmp library
#include <jrast.h>   //  Custom built rasterizer library
#include <jmat.h>    //  Custom built math library

using namespace cimg_library;

int main()
{

    //  If big endian abort
    isBigEndian();

    //  Buffers
    std::fstream buffer1("dependencies/textures/buffer1.bmp", std::ios::binary);
    std::fstream buffer2("dependencies/textures/buffer2.bmp", std::ios::binary);
    std::fstream buffer3("dependencies/textures/buffer3.bmp", std::ios::binary);

    std::fstream depthMap("dependencies/textures/depthMap.bmp", std::ios::binary | std::ios::in | std::ios::out);
    std::string currentBufferFilename = "dependencies/textures/buffer1.bmp";

    //  Specifies what I want from the bmp

    Header header;
    InfoHeader infoHeader;
    ColorTable colorTable;

    infoHeader.width = 200;
    infoHeader.height = 200;

    int rowPadding = (4 - (infoHeader.width * 3) % 4) % 4;           // Ensure each row is a multiple of 4 bytes
    infoHeader.imageSize = infoHeader.width * infoHeader.height * 3; // 30000 bytes
    header.fileSize = header.dataOffset + infoHeader.imageSize + (infoHeader.height * rowPadding);

    //  Actually writes the BMP header, info header and color table 

    writeInfoBMP(header, infoHeader, colorTable, buffer1);
    buffer1.close();
    writeInfoBMP(header, infoHeader, colorTable, buffer2);
    buffer2.close();
    writeInfoBMP(header, infoHeader, colorTable, buffer3);
    buffer3.close();

    writeInfoBMP(header, infoHeader, colorTable, depthMap);
    depthMap.close();

    // Window display

    CImg<unsigned char> image;
    image.resize(infoHeader.width * 3, infoHeader.height * 3);
    CImgDisplay window(image, "Image", 0);
    window.move(0 + 500, 0 + 200);


    //  Vertices / Triangles

    Vertex Test1 = {5, 5, 5, StandardColors::Red};
    Vertex Test2 = {30, 15, 5, StandardColors::Green};
    Vertex Test3 = {15, 30, 5, StandardColors::Blue};

    Vertex Test4 = {55, 25, 5, StandardColors::Orange};
    Vertex Test5 = {85, 45, 5, StandardColors::Cyan};
    Vertex Test6 = {65, 5, 5, StandardColors::Violet};

    Vertex Test7 = {35, 35, 5, StandardColors::Purple};
    Vertex Test8 = {55, 45, 5, StandardColors::Yellow};
    Vertex Test9 = {25, 75, 5, StandardColors::Green};

    Triangle tri = {Test1, Test2, Test3};
    Triangle tri2 = {Test4, Test5, Test6};
    Triangle tri3 = {Test7, Test8, Test9};
    std::vector<Triangle> triangles = {tri, tri2, tri3};

    // Define 8 cube vertices (all Black)
    Vertex v0 = {-0.5f, -0.5f, -0.5f, StandardColors::Red};   // Bottom-back-left
    Vertex v1 = {0.5f, -0.5f, -0.5f, StandardColors::Orange}; // Bottom-back-right
    Vertex v2 = {0.5f, 0.5f, -0.5f, StandardColors::Yellow};  // Top-back-right
    Vertex v3 = {-0.5f, 0.5f, -0.5f, StandardColors::Green};  // Top-back-left
    Vertex v4 = {-0.5f, -0.5f, 0.5f, StandardColors::Cyan};   // Bottom-front-left
    Vertex v5 = {0.5f, -0.5f, 0.5f, StandardColors::Blue};    // Bottom-front-right
    Vertex v6 = {0.5f, 0.5f, 0.5f, StandardColors::Purple};   // Top-front-right
    Vertex v7 = {-0.5f, 0.5f, 0.5f, StandardColors::Violet};  // Top-front-left

    // Define 12 triangles (two per face)

    // Front face (z = 0.5)
    Triangle t1 = {v4, v5, v6}; 
    Triangle t2 = {v4, v6, v7}; 

    // Back face (z = -0.5)
    Triangle t3 = {v0, v1, v2}; 
    Triangle t4 = {v0, v2, v3}; 

    // Left face (x = -0.5)
    Triangle t5 = {v0, v7, v4}; 
    Triangle t6 = {v0, v3, v7};

    // Right face (x = 0.5)
    Triangle t7 = {v1, v6, v2}; 
    Triangle t8 = {v1, v5, v6};

    // Top face (y = 0.5)
    Triangle t9 = {v3, v7, v6};  
    Triangle t10 = {v3, v6, v2}; 

    // Bottom face (y = -0.5)
    Triangle t11 = {v0, v5, v1};
    Triangle t12 = {v0, v4, v5}; 

    // Add to vector
    std::vector<Triangle> cubeTriangles = {
        t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12};

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

    // Add to vector
    std::vector<Triangle> pyramidTriangles = {
        b1, b2, s1, s2, s3, s4};

    //  Linear Algebra
    float zNear = 0.001f, zFar = 20.0f;
    Mat4 model, view, projection;
    model.ident(); model.scale(0.5f, 0.5f, 0.5f);  
    view.ident(); 
    projection.perspective(60, infoHeader.width / infoHeader.height, zNear, zFar);

    //  Image buffer and depth buffer

    std::vector<uint8_t> framebuffer = {}; 
    std::vector<uint8_t> zBuffer = {};

    framebuffer.resize(infoHeader.width * infoHeader.height * 3);
    zBuffer.resize(infoHeader.width * infoHeader.height);

    //  Rewrite file every frame
    while (!window.is_closed())
    {

        // White backdrop
        for (int y = infoHeader.height - 1; y >= 0; --y)
        {
            for (int x = 0; x < infoHeader.width; ++x)
            {
                
                int zIndex = y * infoHeader.width + x;
                int byteIndex = zIndex * 3;

                //  White background
                framebuffer[byteIndex + 0] = static_cast<uint8_t>(255);
                framebuffer[byteIndex + 1] = static_cast<uint8_t>(255);
                framebuffer[byteIndex + 2] = static_cast<uint8_t>(255);

                //  Write 0 to depth map
                zBuffer[zIndex] = 0;
            }

        }

        //  Calculate screen rendering every frame
        model.rotate(0.15, 0.1, 0);                 //      CREATE DEPTH MAP SAMPLE IT, then INCREASE IMAGE RESOLUTOIN, also fix problem when lines are near horizontal
        for (auto &tri : cubeTriangles)             //      ALSO NEED TO INTERPOLATE Z VALUES FOR DEPTH MAP
        {
            //  Converts world space to clip space
            Triangle screenTriangle;

            Vec4 vert1 = (projection * model * view * tri.vert1);
            Vec4 vert2 = (projection * model * view * tri.vert2);
            Vec4 vert3 = (projection * model * view * tri.vert3);

            //  Convert clip space to NDC to screen space

            clipToNDC(vert1);
            clipToNDC(vert2);
            clipToNDC(vert3);

            ndcToScreen(vert1, infoHeader);
            ndcToScreen(vert2, infoHeader);
            ndcToScreen(vert3, infoHeader);

            screenTriangle.vert1 = {vert1.x, vert1.y, vert1.z, tri.vert1.color};
            screenTriangle.vert2 = {vert2.x, vert2.y, vert2.z, tri.vert2.color};
            screenTriangle.vert3 = {vert3.x, vert3.y, vert3.z, tri.vert3.color};

            //  Rasterize the triangles
            lerpFillTriangle(screenTriangle, infoHeader, header, framebuffer, zBuffer, zNear, zFar);
        }

        //  Start editing after 54 bytes (header info)
        int offset = header.dataOffset; 

        std::fstream file(currentBufferFilename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(offset); file.seekg(offset);

        std::fstream depthMap("dependencies/textures/depthMap.bmp", std::ios::binary | std::ios::in | std::ios::out);
        depthMap.seekp(offset);
        depthMap.seekg(offset);

        // Start writing from buffer to actual file
        for (int y = infoHeader.height - 1; y >= 0; --y)
        {
            int correctedY = (infoHeader.height - 1) - y; // flip Y for BMP

            for (int x = 0; x < infoHeader.width; ++x)
            {
                int zIndex = correctedY * infoHeader.width + x;
                int byteIndex = zIndex * 3;

                //  White background
                file.put(framebuffer[byteIndex + 0]);
                file.put(framebuffer[byteIndex + 1]);
                file.put(framebuffer[byteIndex + 2]);

                //  Write 0 to depth map
                depthMap.put(zBuffer[zIndex]);
                depthMap.put(zBuffer[zIndex]);
                depthMap.put(zBuffer[zIndex]); 
            }

            // Add row padding if needed
            char padding[4] = {0};
            if (rowPadding > 0)
            {
                file.write(padding, rowPadding);
                depthMap.write(padding, rowPadding);
            }
        }

        file.close();
        depthMap.close();

        //  Switch Buffers

        if (currentBufferFilename == "dependencies/textures/buffer1.bmp")
        {
            currentBufferFilename = "dependencies/textures/buffer2.bmp";
            image.load("dependencies/textures/buffer1.bmp");
        }
        else if (currentBufferFilename == "dependencies/textures/buffer2.bmp"){
            currentBufferFilename = "dependencies/textures/buffer3.bmp";
            image.load("dependencies/textures/buffer2.bmp");
        }
        else
        {
            currentBufferFilename = "dependencies/textures/buffer1.bmp";
            image.load("dependencies/textures/buffer3.bmp");
        }

        //  Change the image

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        window.display(image);
        window.wait(1);
    }
    depthMap.close();
    return 0;
}
