#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

#include <CImg.h>   //  OUTSOURCED for bmp image preview

#include <InputHandler.h>   //  Custom built input library
#include <jshapes.h> //  Custom built shapes and colors library
#include <jbmp.h>    //  Custom built bmp library
#include <jmat.h>    //  Custom built math library
#include <jrast.h>   //  Custom built rasterizer library
#include <MovementController.h>  //Custom built movement library

using namespace cimg_library;

void isBigEndian()
{
    uint16_t num = 1;
    if (*(reinterpret_cast<uint8_t *>(&num)) == 0)
    {
        std::cout << "System is Big Endian: Program will not function correclty\n";
        std::abort();
    }
}

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
    window.move(0 + 400, 0 + 200);

    // Define 24 vertices (3 per face, corners reuse colors)
    Vec3 frontNormal = {0.0f, 0.0f, 1.0f};
    Vertex v0 = {-0.5f, -0.5f, 0.5f, StandardColors::Red, 0.0f, 0.0f, frontNormal};
    Vertex v1 = {0.5f, -0.5f, 0.5f, StandardColors::Orange, 1.0f, 0.0f, frontNormal};
    Vertex v2 = {0.5f, 0.5f, 0.5f, StandardColors::Yellow, 1.0f, 1.0f, frontNormal};
    Vertex v3 = {-0.5f, 0.5f, 0.5f, StandardColors::Green, 0.0f, 1.0f, frontNormal};

    Vec3 backNormal = {0.0f, 0.0f, -1.0f};
    Vertex v4 = {0.5f, -0.5f, -0.5f, StandardColors::Cyan, 0.0f, 0.0f, backNormal};
    Vertex v5 = {-0.5f, -0.5f, -0.5f, StandardColors::Blue, 1.0f, 0.0f, backNormal};
    Vertex v6 = {-0.5f, 0.5f, -0.5f, StandardColors::Purple, 1.0f, 1.0f, backNormal};
    Vertex v7 = {0.5f, 0.5f, -0.5f, StandardColors::Violet, 0.0f, 1.0f, backNormal};

    Vec3 leftNormal = {-1.0f, 0.0f, 0.0f};
    Vertex v8 = {-0.5f, -0.5f, -0.5f, StandardColors::Blue, 0.0f, 0.0f, leftNormal};
    Vertex v9 = {-0.5f, -0.5f, 0.5f, StandardColors::Red, 1.0f, 0.0f, leftNormal};
    Vertex v10 = {-0.5f, 0.5f, 0.5f, StandardColors::Green, 1.0f, 1.0f, leftNormal};
    Vertex v11 = {-0.5f, 0.5f, -0.5f, StandardColors::Purple, 0.0f, 1.0f, leftNormal};

    Vec3 rightNormal = {1.0f, 0.0f, 0.0f};
    Vertex v12 = {0.5f, -0.5f, 0.5f, StandardColors::Orange, 0.0f, 0.0f, rightNormal};
    Vertex v13 = {0.5f, -0.5f, -0.5f, StandardColors::Cyan, 1.0f, 0.0f, rightNormal};
    Vertex v14 = {0.5f, 0.5f, -0.5f, StandardColors::Violet, 1.0f, 1.0f, rightNormal};
    Vertex v15 = {0.5f, 0.5f, 0.5f, StandardColors::Yellow, 0.0f, 1.0f, rightNormal};

    Vec3 topNormal = {0.0f, 1.0f, 0.0f};
    Vertex v16 = {-0.5f, 0.5f, 0.5f, StandardColors::Green, 0.0f, 0.0f, topNormal};
    Vertex v17 = {0.5f, 0.5f, 0.5f, StandardColors::Yellow, 1.0f, 0.0f, topNormal};
    Vertex v18 = {0.5f, 0.5f, -0.5f, StandardColors::Violet, 1.0f, 1.0f, topNormal};
    Vertex v19 = {-0.5f, 0.5f, -0.5f, StandardColors::Purple, 0.0f, 1.0f, topNormal};

    Vec3 bottomNormal = {0.0f, -1.0f, 0.0f};
    Vertex v20 = {-0.5f, -0.5f, -0.5f, StandardColors::Blue, 0.0f, 0.0f, bottomNormal};
    Vertex v21 = {0.5f, -0.5f, -0.5f, StandardColors::Cyan, 1.0f, 0.0f, bottomNormal};
    Vertex v22 = {0.5f, -0.5f, 0.5f, StandardColors::Orange, 1.0f, 1.0f, bottomNormal};
    Vertex v23 = {-0.5f, -0.5f, 0.5f, StandardColors::Red, 0.0f, 1.0f, bottomNormal};

    // Each face: two triangles (0-1-2 and 0-2-3 pattern)
    Triangle front1 = {v0, v1, v2};
    Triangle front2 = {v0, v2, v3};

    Triangle back1 = {v4, v5, v6};
    Triangle back2 = {v4, v6, v7};

    Triangle left1 = {v8, v9, v10};
    Triangle left2 = {v8, v10, v11};

    Triangle right1 = {v12, v13, v14};
    Triangle right2 = {v12, v14, v15};

    Triangle top1 = {v16, v17, v18};
    Triangle top2 = {v16, v18, v19};

    Triangle bottom1 = {v20, v21, v22};
    Triangle bottom2 = {v20, v22, v23};

    // Store all triangles for cube faces
    std::vector<Triangle> cubeTriangles = {
        front1, front2, back1, back2, left1, left2, right1, right2, top1, top2, bottom1, bottom2
    };

    //  Linear Algebra
    float zNear = 0.001f, zFar = 20.0f;
    Mat4 model, view, projection;
    model.ident(); model.scale(0.5f, 0.5f, 0.5f);  
    view.ident(); view.rotate(0, 0, 0); view.translate(0.5f, 0, 0.0f);    
    projection.perspective(60, infoHeader.width / infoHeader.height, zNear, zFar);

    //  Image buffer and depth buffer
    std::vector<uint8_t> frameBuffer = {}; 
    std::vector<uint8_t> depthBuffer = {};
    frameBuffer.resize(infoHeader.width * infoHeader.height * 3);
    depthBuffer.resize(infoHeader.width * infoHeader.height * 3);

    //  Texture image in a buffer
    std::vector<uint8_t> tex1;
    int texWidth = 500, texHeight = 500;
    readBMP("dependencies/textures/stone2.bmp", tex1, texWidth, texHeight);

    //  Input Handler
    InputHandler inputHandler(WINDOWS);
    MovementController movementController(inputHandler, view, deltaTime);

    //  Rasterizer
    Rasterizer::enableInterpolation(true);
    Rasterizer::enableTextureMapping(true);
    Rasterizer::enableLighting(true);
    //Rasterizer::useTextureColor(false);

    Rasterizer::attachFramebuffer(frameBuffer);
    Rasterizer::attachDepthBuffer(depthBuffer);

    Rasterizer::setTextureMap(tex1, texWidth, texHeight);
    //Rasterizer::lightingSettings(DIRECTIONAL_LIGHT, 0.2f, 0.5f);

    //  Rewrite file every frame
    while (!window.is_closed() && !inputHandler.getInputs().esc)
    {
        start = std::chrono::high_resolution_clock::now();

        //  Movement
        model.rotate(2 * 0.75 * deltaTime, 2 * 0.5 * deltaTime, 0);     //  Normals are static currently, modify when rotating matrix
        movementController.move();

        Rasterizer::clearScreen();

        //  Object pos - cameraPos;
        Mat4 cameraModel;
        view.inverse(view, cameraModel);    
        Vec3 cameraPos = {cameraModel.m14, cameraModel.m24, cameraModel.m34};

        Rasterizer::setCamPos(cameraPos);

        Mat4 invModel, invTransModel;
        model.inverse(model, invModel);
        invModel.transpose(invModel, invTransModel);

        //  Calculate screen rendering every frame
        for (auto &tri : cubeTriangles)
        {
            //  Update normals 
            Vec3 normal1 = (Mat3(invTransModel) * tri.vert1.normal).normalize();
            Vec3 normal2 = (Mat3(invTransModel) * tri.vert2.normal).normalize();
            Vec3 normal3 = (Mat3(invTransModel) * tri.vert3.normal).normalize();

            //  Converts world space to clip space
            Triangle screenTriangle;

            //  Precomupte Values for perspective correction

            Vec4 vert1Vec = {tri.vert1.x, tri.vert1.y, tri.vert1.z, 1.0f};
            Vec4 vert2Vec = {tri.vert2.x, tri.vert2.y, tri.vert2.z, 1.0f};
            Vec4 vert3Vec = {tri.vert3.x, tri.vert3.y, tri.vert3.z, 1.0f};

            Vec4 viewSpacePos1 = view * model * vert1Vec;
            Vec4 viewSpacePos2 = view * model * vert2Vec;
            Vec4 viewSpacePos3 = view * model * vert3Vec;

            Vec4 vert1 = projection * viewSpacePos1;
            Vec4 vert2 = projection * viewSpacePos2;
            Vec4 vert3 = projection * viewSpacePos3;

            //  Update world pos based on model matrix
            Vec3 worldPos1 = {viewSpacePos1.x, viewSpacePos1.y, viewSpacePos1.z};
            Vec3 worldPos2 = {viewSpacePos2.x, viewSpacePos2.y, viewSpacePos2.z};
            Vec3 worldPos3 = {viewSpacePos3.x, viewSpacePos3.y, viewSpacePos3.z};

            //  Convert clip space to NDC to screen space

            clipToNDC(vert1);
            clipToNDC(vert2);
            clipToNDC(vert3);

            ndcToScreen(vert1, infoHeader);
            ndcToScreen(vert2, infoHeader);
            ndcToScreen(vert3, infoHeader);

            //  Clean this up a bit later
            screenTriangle.vert1 = {vert1.x, vert1.y, vert1.z, tri.vert1.color, tri.vert1.texX, tri.vert1.texY, normal1, worldPos1};
            screenTriangle.vert2 = {vert2.x, vert2.y, vert2.z, tri.vert2.color, tri.vert2.texX, tri.vert2.texY, normal2, worldPos2};
            screenTriangle.vert3 = {vert3.x, vert3.y, vert3.z, tri.vert3.color, tri.vert3.texX, tri.vert3.texY, normal3, worldPos3};

            //  For perspective correction with tex coordinates
            
            if(tri.vert1.hasValue() && tri.vert2.hasValue() && tri.vert3.hasValue() && Rasterizer::isTextureMapped())
            {
                screenTriangle.vert1.invZ = 1.0f / vert1.w;
                screenTriangle.vert1.texX_divZ = tri.vert1.texX.value() / vert1.w;
                screenTriangle.vert1.texY_divZ = tri.vert1.texY.value() / vert1.w;

                screenTriangle.vert2.invZ = 1.0f / vert2.w;
                screenTriangle.vert2.texX_divZ = tri.vert2.texX.value() / vert2.w;
                screenTriangle.vert2.texY_divZ = tri.vert2.texY.value() / vert2.w;

                screenTriangle.vert3.invZ = 1.0f / vert3.w;
                screenTriangle.vert3.texX_divZ = tri.vert3.texX.value() / vert3.w;
                screenTriangle.vert3.texY_divZ = tri.vert3.texY.value() / vert3.w;
            }

            //  Rasterize the triangles
            Rasterizer::drawTriangle(screenTriangle, infoHeader, header, zNear, zFar);
        }

        Rasterizer::updateScreen(currentBufferFilename, header, infoHeader);

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
