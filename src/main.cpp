#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

#include <CImg.h>   //  OUTSOURCED for bmp image preview

#include <InputHandler.h>   //  Custom built input library
#include <jbmp.h>    //  Custom built bmp library
#include <jrast.h>   //  Custom built rasterizer library
#include <jmat.h>    //  Custom built math library
#include <MovementController.h>  //Custom built movement library

using namespace cimg_library;

int main()
{

    //  Time stuff
    auto start = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;

    //  If big endian abort
    isBigEndian();

    // Clear buffers 
    {
        std::ofstream buffer1("dependencies/frames/buffer1.bmp", std::ios::binary | std::ios::trunc);
        std::ofstream buffer2("dependencies/frames/buffer2.bmp", std::ios::binary | std::ios::trunc);
        std::ofstream buffer3("dependencies/frames/buffer3.bmp", std::ios::binary | std::ios::trunc);
        std::ofstream depthMap("dependencies/frames/depthMap.bmp", std::ios::binary | std::ios::trunc);
    }

    // Buffers
    std::fstream buffer1("dependencies/frames/buffer1.bmp", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream buffer2("dependencies/frames/buffer2.bmp", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream buffer3("dependencies/frames/buffer3.bmp", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream depthMap("dependencies/frames/depthMap.bmp", std::ios::binary | std::ios::in | std::ios::out);

    std::string currentBufferFilename = "dependencies/frames/buffer1.bmp";

    //  Specifies what I want from the bmp
    Header header;
    InfoHeader infoHeader;
    ColorTable colorTable;

    infoHeader.width = 300;
    infoHeader.height = 300;

    int rowPadding = (4 - (infoHeader.width * 3) % 4) % 4;           // Ensure each row is a multiple of 4 bytes
    infoHeader.imageSize = infoHeader.width * infoHeader.height * 3; 
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

    // Window display for bmp file
    CImg<unsigned char> image;
    image.resize(infoHeader.width * 2, infoHeader.height * 2);
    CImgDisplay window(image, "Image", 0);
    window.move(0 + 500, 0 + 200);


    //  Vertices / Triangles
    Vertex Test1 = {5, 5, 5, StandardColors::Red, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test2 = {30, 15, 5, StandardColors::Green, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test3 = {15, 30, 5, StandardColors::Blue, std::optional<float>(1.0f), std::optional<float>(1.0f)};

    Vertex Test4 = {55, 25, 5, StandardColors::Orange, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test5 = {85, 45, 5, StandardColors::Cyan, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test6 = {65, 5, 5, StandardColors::Violet, std::optional<float>(1.0f), std::optional<float>(1.0f)};

    Vertex Test7 = {35, 35, 5, StandardColors::Purple, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test8 = {55, 45, 5, StandardColors::Yellow, std::optional<float>(1.0f), std::optional<float>(1.0f)};
    Vertex Test9 = {25, 75, 5, StandardColors::Green, std::optional<float>(1.0f), std::optional<float>(1.0f)};

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

    std::vector<Triangle> cubeTriangles = {
        t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12
    };

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
    zBuffer.resize(infoHeader.width * infoHeader.height * 3);

    //  Texture image in a buffer
    std::vector<uint8_t> tex1 = {};

    //  Input Handler
    InputHandler inputHandler(WINDOWS);
    MovementController movementController(inputHandler, view, deltaTime);

    //  Rewrite file every frame
    while (!window.is_closed() && !inputHandler.getInputs().esc)
    {
        start = std::chrono::high_resolution_clock::now();

        //  Resets framebuffer and zB
        std::fill(framebuffer.begin(), framebuffer.end(), 255);
        std::fill(zBuffer.begin(), zBuffer.end(), 0);

        //  Movement
        model.rotate(1.5 * deltaTime, 1.0 * deltaTime, 0);
        movementController.move();

        //  Calculate screen rendering every frame
        for (auto &tri : cubeTriangles)
        {
            //  Converts world space to clip space
            Triangle screenTriangle;

            Vec4 vert1 = (projection * view * model * tri.vert1);
            Vec4 vert2 = (projection * view * model * tri.vert2);
            Vec4 vert3 = (projection * view * model * tri.vert3);

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
            lerpFillTriangle(screenTriangle, infoHeader, header, framebuffer, zBuffer, tex1, zNear, zFar);
        }

        //  Start editing after 54 bytes (header info)
        int offset = header.dataOffset; 

        std::fstream file(currentBufferFilename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(offset); file.seekg(offset);
        file.write(reinterpret_cast<const char *>(framebuffer.data()), framebuffer.size());

        std::fstream depthMap("dependencies/frames/depthMap.bmp", std::ios::binary | std::ios::in | std::ios::out);
        depthMap.seekp(offset); depthMap.seekg(offset);
        depthMap.write(reinterpret_cast<const char *>(zBuffer.data()), zBuffer.size());
        depthMap.write(reinterpret_cast<const char*>(framebuffer.data()), framebuffer.size());

        file.close();
        depthMap.close();

        //  Switch Buffers

        if (currentBufferFilename == "dependencies/frames/buffer1.bmp")
        {
            currentBufferFilename = "dependencies/frames/buffer2.bmp";
            image.load("dependencies/frames/buffer1.bmp");
        }
        else if (currentBufferFilename == "dependencies/frames/buffer2.bmp"){
            currentBufferFilename = "dependencies/frames/buffer3.bmp";
            image.load("dependencies/frames/buffer2.bmp");
        }
        else
        {
            currentBufferFilename = "dependencies/frames/buffer1.bmp";
            image.load("dependencies/frames/buffer3.bmp");
        }

        //  Change the image
        window.display(image);
        window.wait(1);

        //  Delta Time

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        deltaTime = duration.count() / 1000.0f;     //  Seconds
    }
    depthMap.close();
    return 0;
}
