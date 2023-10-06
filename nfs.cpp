#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colors;
    uint32_t importantColors;
};
#pragma pack(pop)

int main() {
    std::string inputImagePath = "photo.bmp";
    std::string clockwiseImagePath = "clockwise.bmp";

    // Открываем файл изображения для чтения
    std::ifstream inputFile(inputImagePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Ошибка при открытии файла." << std::endl;
        return 1;
    }

    // Считываем BMP заголовок
    BMPHeader bmpHeader;
    if (!inputFile.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHeader))) {
        std::cerr << "Ошибка при чтении BMP заголовка." << std::endl;
        return 1;
    }

    // Определение размера строки в байтах
    int bytesPerRow = (bmpHeader.width * bmpHeader.bitsPerPixel + 7) / 8;
    int padding = (4 - (bytesPerRow % 4)) % 4;

    // Создаем вектор для хранения данных изображения
    std::vector<uint8_t> imageData(bytesPerRow * bmpHeader.height);

    // Чтение данных изображения
    if (!inputFile.read(reinterpret_cast<char*>(imageData.data()), imageData.size())) {
        std::cerr << "Ошибка при чтении данных изображения." << std::endl;
        return 1;
    }

    // Поворот изображения на 90 градусов по часовой стрелке
    std::vector<uint8_t> rotatedImage(imageData.size());
    for (int y = 0; y < bmpHeader.height; ++y) {
        for (int x = 0; x < bmpHeader.width; ++x) {
            int srcIndex = (bmpHeader.height - y - 1) * bytesPerRow + x * 3;
            int destIndex = x * bmpHeader.bitsPerPixel / 8 + y * bytesPerRow;
            for (int i = 0; i < bytesPerRow; ++i) {
                rotatedImage[destIndex + i] = imageData[srcIndex + i];
            }
        }
    }

    // Открываем файл для сохранения повернутого изображения
    std::ofstream clockwiseFile(clockwiseImagePath, std::ios::binary);
    if (!clockwiseFile) {
        std::cerr << "Ошибка при создании файла для сохранения результатов." << std::endl;
        return 1;
    }

    // Записываем BMP заголовок в новый файл
    clockwiseFile.write(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHeader));

    // Записываем данные повернутого изображения
    clockwiseFile.write(reinterpret_cast<char*>(rotatedImage.data()), rotatedImage.size());

    // Закрываем файлы
    inputFile.close();
    clockwiseFile.close();

    std::cout << "Изображение успешно повернуто и сохранено." << std::endl;

    return 0;
}
