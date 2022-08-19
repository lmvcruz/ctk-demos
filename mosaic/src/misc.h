#include <vector>
#include <string>

#include "ctk/misc/filesys.h"

std::vector<std::string> getImagesPathFromFolder(const std::string &folderPath)
{
    std::vector<std::string> extensionFilters = {"png", "PNG", "jpg", "jpeg", "JPEG", "JPG"};
    return ctk::ListFilesContainingAnyExpressions(folderPath, extensionFilters);
}

ctk::RgbImage initializeMosaicImageFromGridSize(const Grid &grid)
{
    auto gridSize = grid.calculateGridSize();

    ctk::RgbImage mosaicImg;
    mosaicImg.Create(gridSize.first, gridSize.second);
    mosaicImg.Fill(255, 255, 255);
    return mosaicImg;
}

void resizeAndCropImagePreservingAspectRatio(ctk::RgbImage &img, int expectedWidth, int expectedHeight)
{
    int imageWidth = img.GetWidth();
    int imageHeight = img.GetHeight();
    double diw = static_cast<double>(imageWidth);
    double dih = static_cast<double>(imageHeight);

    int newImageWidth = imageWidth;
    int newImageHeight = imageHeight;
    int ox = 0;
    int oy = 0;

    double expRatio = static_cast<double>(expectedWidth) / static_cast<double>(expectedHeight);
    double curRatio = diw / dih;

    if (curRatio >= expRatio)
    {
        newImageWidth = static_cast<int>(diw * expRatio / curRatio);
        ox = (imageWidth - newImageWidth) / 2;
    }
    else
    {
        newImageHeight = static_cast<int>(dih * curRatio / expRatio);
        oy = (imageHeight - newImageHeight) / 2;
    }

    img.SelfCrop(ox, oy, newImageWidth, newImageHeight);
    img = img.Resize(expectedWidth, expectedHeight);
}

void addImageInMosaic(ctk::RgbImage &mosaic, const RetangularCell &cell, const std::string &imgPath)
{
    ctk::RgbImage aux;
    aux.Open(imgPath);
    resizeAndCropImagePreservingAspectRatio(aux, cell.width, cell.height);

    constexpr int XOrigInMosaic = 0;
    constexpr int YOrigInMosaic = 0;
    mosaic.CopyFrom(aux, cell.orig.GetX(), cell.orig.GetY(), XOrigInMosaic, YOrigInMosaic, cell.width, cell.height);
}
