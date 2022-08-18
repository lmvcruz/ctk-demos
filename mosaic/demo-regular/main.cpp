#include <iostream>
#include <vector>
#include <exception>

#include <argparse/argparse.hpp>

#include "ctk/geometry/point.h"
#include "ctk/image/rgb_image.h"
#include "ctk/misc/filesys.h"

struct Params
{
    std::string folder;
    std::string output;
    std::vector<int> gridInput;
    std::vector<int> cellInput;
    int offset;
};

argparse::ArgumentParser
setupParserArguments()
{
    argparse::ArgumentParser program("mosaic");
    program.add_argument("-f", "--folder").required().help("Folder containing source images");
    program.add_argument("-o", "--output").required().help("File path to resultant mosaic image");
    program.add_argument("-g", "--grid").required().nargs(2).scan<'i', int>().help("Grid size (width an height)");
    program.add_argument("-c", "--cell").required().nargs(2).scan<'i', int>().help("Cell size (width an height)");
    program.add_argument("--offset").default_value(0).scan<'i', int>().help("Offset value (distance in between cells, and image border");
    return program;
}

Params readParams(int argc, char *argv[])
{
    auto program = setupParserArguments();
    program.parse_args(argc, argv);

    Params params;
    params.folder = program.get<std::string>("--folder");
    params.output = program.get<std::string>("--output");
    params.gridInput = program.get<std::vector<int>>("--grid");
    params.cellInput = program.get<std::vector<int>>("--cell");
    params.offset = program.get<int>("--offset");
    return params;
}

std::vector<std::string> getImagesPathFromFolder(const std::string &folderPath)
{
    std::vector<std::string> extensionFilters = {"png", "PNG", "jpg", "jpeg", "JPEG", "JPG"};
    return ctk::ListFilesContainingAnyExpressions(folderPath, extensionFilters);
}

struct RetangularCell
{
    ctk::PointI orig;
    int width;
    int height;
};

class Grid
{
public:
    void createAndAddCell(const ctk::PointI &orig, int width, int height)
    {
        if (orig.GetX() < 0 || orig.GetY() < 0)
        {
            throw std::runtime_error("[RetangularCell] Cell origin must be non-negative");
        }
        if (width <= 0 || height <= 0)
        {
            throw std::runtime_error("[RetangularCell] Cell size musst be positive");
        }
        cells.push_back({orig, width, height});
    }

    const int size() const
    {
        return cells.size();
    }

    const RetangularCell &getCell(int idx) const
    {
        return cells[idx];
    }

    std::pair<int, int> calculateGridSize() const
    {
        int width = 0;
        int height = 0;
        for (const auto &cell : cells)
        {
            auto cx = cell.orig.GetX();
            auto cy = cell.orig.GetY();
            auto cw = cell.width;
            auto ch = cell.height;
            assert(cx >= 0);
            assert(cy >= 0);
            assert(cw > 0);
            assert(ch > 0);

            width = std::max(width, cx + cw);
            height = std::max(height, cy + ch);
        }
        return std::make_pair(width, height);
    }

private:
    std::vector<RetangularCell> cells;
};

Grid createRegularGrid(int gridWidth, int gridHeight, int cellWidth, int cellHeight, int offset)
{
    std::cout << "Init: " << gridWidth << " " << gridHeight << " " << cellWidth << " " << cellHeight << " " << offset << std::endl;
    Grid grid;
    for (int gx = 0; gx < gridWidth; ++gx)
    {
        int ox = offset + gx * (cellWidth + offset);
        for (int gy = 0; gy < gridHeight; ++gy)
        {
            int oy = offset + gy * (cellHeight + offset);
            grid.createAndAddCell({ox, oy}, cellWidth, cellHeight);
        }
    }
    return grid;
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

ctk::RgbImage createAndFillMosaicImage(const std::vector<std::string> &sourceImages, const Grid &grid)
{
    auto mosaic = initializeMosaicImageFromGridSize(grid);
    for (int i = 0; i < grid.size(); ++i)
    {
        addImageInMosaic(mosaic, grid.getCell(i), sourceImages[i]);
    }
    return mosaic;
}

int main(int argc, char *argv[])
{
    try
    {
        auto params = readParams(argc, argv);
        auto sourceImages = getImagesPathFromFolder(params.folder);
        auto grid = createRegularGrid(params.gridInput[0], params.gridInput[1], params.cellInput[0], params.cellInput[1], params.offset);
        auto mosaic = createAndFillMosaicImage(sourceImages, grid);
        mosaic.Save(params.output);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        std::exit(1);
    }
    return 0;
}
