#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <jbmp.h>    //  Custom built bmp library 
#include <jmat.h>    //  Custom built math library
#include <jrast.h>   //  Custom built rasterizer library

int main()
{
    //  If big endian abort

    bool endProgram = false;
    isBigEndian();

    //  Buffers
    std::ofstream buffer1("dependencies/textures/buffer1.bmp", std::ios::binary);
    std::ofstream buffer2("dependencies/textures/buffer2.bmp", std::ios::binary);
    std::string currentBufferFilename = "dependencies/textures/buffer1.bmp";

    //  Specifies what I want from the bmp

    Header header;
    InfoHeader infoHeader;
    ColorTable colorTable;

    infoHeader.width = 100;
    infoHeader.height = 100;

    int rowPadding = (4 - (infoHeader.width * 3) % 4) % 4;           // Ensure each row is a multiple of 4 bytes
    infoHeader.imageSize = infoHeader.width * infoHeader.height * 3; // 30000 bytes
    header.fileSize = header.dataOffset + infoHeader.imageSize + (infoHeader.height * rowPadding);

    //  Actually writes the BMP header, info header and color table 

    writeInfoBMP(header, infoHeader, colorTable, buffer1);
    buffer1.close();
    writeInfoBMP(header, infoHeader, colorTable, buffer2);
    buffer2.close();

    //  Colors 

    Color Black = {0, 0, 0};
    Color White = {255, 255, 255};

    Color Purple = {255, 0, 255}; 
    Color Teal = {255, 255, 0};   

    Color Red = {0, 0, 255};      
    Color Orange = {0, 127, 255};
    Color Yellow = {0, 255, 255}; 
    Color Green = {0, 255, 0};    
    Color Cyan = {255, 255, 0};   
    Color Blue = {255, 0, 0};     
    Color Violet = {255, 0, 139}; 

    //  Vertices / Triangles

    Vertex Test1 = {5, 5, 5, Red};
    Vertex Test2 = {30, 15, 5, Green};
    Vertex Test3 = {15, 30, 5, Blue};

    Vertex Test4 = {55, 25, 5, Orange};
    Vertex Test5 = {85, 45, 5, Cyan};
    Vertex Test6 = {65, 5, 5, Violet};

    Vertex Test7 = {35, 35, 5, Purple};
    Vertex Test8 = {55, 45, 5, Yellow};
    Vertex Test9 = {25, 75, 5, Green};

    std::vector<Vertex> vertices;

    Triangle tri = {Test1, Test2, Test3};
    Triangle tri2 = {Test4, Test5, Test6};
    Triangle tri3 = {Test7, Test8, Test9};
    std::vector<Triangle> triangles = {tri, tri2, tri3};

    //  Rewrite file every frame

    while(!endProgram){
        std::fstream file(currentBufferFilename, std::ios::binary | std::ios::in | std::ios::out);
        int offset = header.dataOffset;                   //  Start editing after 54 bytes (header info)
        file.seekp(offset, std::ios::beg); 
        

        // Black backdrop
        for (int y = infoHeader.height - 1; y >= 0; --y)
        {

            for (int x = 0; x < infoHeader.width; ++x)
            {
                //  White background
                file.put(255);
                file.put(255);
                file.put(255);
            }

            // Add row padding if needed
            char padding[4] = {0};
            if (rowPadding > 0)
            {
                std::cout << "padded";
                file.write(padding, rowPadding);
            }   
        }

        //  Draw the actual image ontop

        for(auto& triangle : triangles){
    
            //  Draw all 3 vertices in the triangle

            int correctedY = (infoHeader.height - 1) - triangle.vert1.y; // flip Y for BMP
            int index = header.dataOffset + triangle.vert1.x * 3 + (correctedY * 3 * infoHeader.width);
            file.seekp(index, std::ios::beg);
            file.put(triangle.vert1.color.b);
            file.put(triangle.vert1.color.g);
            file.put(triangle.vert1.color.r);

            correctedY = (infoHeader.height - 1) - triangle.vert2.y; // flip Y for BMP
            index = header.dataOffset + triangle.vert2.x * 3 + (correctedY * 3 * infoHeader.width);
            file.seekp(index, std::ios::beg);
            file.put(triangle.vert2.color.b);
            file.put(triangle.vert2.color.g);
            file.put(triangle.vert2.color.r);

            correctedY = (infoHeader.height - 1) - triangle.vert3.y; // flip Y for BMP
            index = header.dataOffset + triangle.vert3.x * 3 + (correctedY * 3 * infoHeader.width);
            file.seekp(index, std::ios::beg);
            file.put(triangle.vert3.color.b);
            file.put(triangle.vert3.color.g);
            file.put(triangle.vert3.color.r);

            //lerpVerts(triangle.vert1, triangle.vert2, currentBufferFilename, infoHeader, header, file);
            lerpFillTriangle(triangle, currentBufferFilename, infoHeader, header, file);
        }
        //file.flush();
        file.close();

        //  Switch Buffers

        if (currentBufferFilename == "dependencies/textures/buffer1.bmp")
        {
            currentBufferFilename = "dependencies/textures/buffer2.bmp";
            /*
                Essentially switch to display buffer 1 now, since finished writing to buffer 1
            */
        }
        else
        {
            currentBufferFilename = "dependencies/textures/buffer1.bmp";
            /*
                Switch back to display buffer 2 now because finished writing to buffer 2    
            */
        }
    }
    return 0;
}


void ndcToScreen(float x, float y){
    
    return;
}

