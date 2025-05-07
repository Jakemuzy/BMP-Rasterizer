
#pragma pack(push, 1) //  No padding in structs,  don't know if this matters

//  14  bytes
struct Header
{
    char signature[2] = {'B', 'M'};
    uint32_t fileSize;
    uint32_t reserved = 0;
    uint32_t dataOffset = 54; //  Total # bytes before actual data
};

//  40 bytes
struct InfoHeader
{
    uint32_t size = 40; //  40 Bytes of info header
    uint32_t width;
    uint32_t height;
    char planes = 1;
    char padding1 = 0;

    char bitCount = 24; //  Pallete, 1 color, 4, 16, 256... bits per pixel
    char padding2 = 0;
    uint32_t compression = 0;
    uint32_t imageSize; //  No compression, 0

    uint32_t xPixelsPerMeter = 0; //  Horizontal and vertical resolution
    uint32_t yPixelsPerMeter = 0;
    uint32_t colorsUsed = 0;
    uint32_t colorsImportant = 0; //  All = 0
};

//  Required if Bits Per Pixel <=8
//  4 * NumColors bytes
struct ColorTable
{
    //std::vector<Color> colors;
};

#pragma pack(pop)

//  Parses the struct and returns the output to filestream
void writeInfoBMP(Header header, InfoHeader infoHeader, ColorTable colorTable, std::ofstream &file)
{
    // Write header (14 bytes)
    file.write(reinterpret_cast<char *>(&header.signature), sizeof(header.signature));
    file.write(reinterpret_cast<char *>(&header.fileSize), sizeof(header.fileSize));
    file.write(reinterpret_cast<char *>(&header.reserved), sizeof(header.reserved));
    file.write(reinterpret_cast<char *>(&header.dataOffset), sizeof(header.dataOffset));

    // Write info header (40 bytes)
    file.write(reinterpret_cast<char *>(&infoHeader.size), sizeof(infoHeader.size));
    file.write(reinterpret_cast<char *>(&infoHeader.width), sizeof(infoHeader.width));
    file.write(reinterpret_cast<char *>(&infoHeader.height), sizeof(infoHeader.height));
    file.write(reinterpret_cast<char *>(&infoHeader.planes), sizeof(infoHeader.planes));
    file.write(reinterpret_cast<char *>(&infoHeader.padding1), sizeof(infoHeader.padding1));
    file.write(reinterpret_cast<char *>(&infoHeader.bitCount), sizeof(infoHeader.bitCount));
    file.write(reinterpret_cast<char *>(&infoHeader.padding2), sizeof(infoHeader.padding2));
    file.write(reinterpret_cast<char *>(&infoHeader.compression), sizeof(infoHeader.compression));
    file.write(reinterpret_cast<char *>(&infoHeader.imageSize), sizeof(infoHeader.imageSize));
    file.write(reinterpret_cast<char *>(&infoHeader.xPixelsPerMeter), sizeof(infoHeader.xPixelsPerMeter));
    file.write(reinterpret_cast<char *>(&infoHeader.yPixelsPerMeter), sizeof(infoHeader.yPixelsPerMeter));
    file.write(reinterpret_cast<char *>(&infoHeader.colorsUsed), sizeof(infoHeader.colorsUsed));
    file.write(reinterpret_cast<char *>(&infoHeader.colorsImportant), sizeof(infoHeader.colorsImportant));

    /*
    for (const auto &color : colorTable.colors)
    {
        file.write(reinterpret_cast<const char *>(&color), sizeof(Color));
    }
    */
}