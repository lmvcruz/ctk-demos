#include <iostream>
#include <vector>
#include <exception>

#include <argparse/argparse.hpp>

#include "ctk/geometry/point.h"
#include "ctk/image/rgb_image.h"

#include "grid.h"
#include "misc.h"

struct Level
{
    int quantity;
    int maxSize;
};

struct Params
{
    std::string folder;
    std::string output;
    std::vector<int> imageSizeInput;
    std::vector<Level> levels;
};

std::vector<Level> createAllLevelsFromArgs(const std::vector<int> &levelsInput)
{
    if (levelsInput.size() % 2 != 0)
    {
        throw std::runtime_error("[Level] Args for level specification must contain two integers");
    }

    std::vector<Level> levels;
    levels.reserve(levelsInput.size() / 2);
    for (int i = 0; i < levelsInput.size(); i += 2)
    {
        // levelsInput[i]: quantity
        // levelsInput[i + 1]: maxSize
        levels.push_back({levelsInput[i], levelsInput[i + 1]});
    }
    return levels;
}

argparse::ArgumentParser
setupParserArguments()
{
    argparse::ArgumentParser program("mosaic");
    program.add_argument("-f", "--folder").required().help("Folder containing source images");
    program.add_argument("-o", "--output").required().help("File path to resultant mosaic image");
    program.add_argument("-s", "--size").required().nargs(2).scan<'i', int>().help("Image size (width an height)");
    program.add_argument("--level").nargs(2).scan<'i', int>().append().help("Quantity of images in a level and their maximum size");
    return program;
}

Params readParams(int argc, char *argv[])
{
    auto program = setupParserArguments();
    program.parse_args(argc, argv);

    Params params;
    params.folder = program.get<std::string>("--folder");
    params.output = program.get<std::string>("--output");
    params.imageSizeInput = program.get<std::vector<int>>("--size");
    params.levels = createAllLevelsFromArgs(program.get<std::vector<int>>("--level"));
    return params;
}

Grid createGridWithLevels(const std::vector<Level> &levels, int mosaicWidth, int mosaicHeight)
{
    Grid grid;
    for (const auto &level : levels)
    {
        auto cellMaxSize = level.maxSize;
        for (int i = 0; i < level.quantity; ++i)
        {
            int ox = rand() % (mosaicWidth - cellMaxSize);
            int oy = rand() % (mosaicHeight - cellMaxSize);
            grid.createAndAddCell({ox, oy}, cellMaxSize, cellMaxSize);
        }
    }
    return grid;
}

ctk::RgbImage createAndFillMosaicImage(const std::vector<std::string> &sourceImages, const Grid &grid)
{
    auto mosaic = initializeMosaicImageFromGridSize(grid);
    for (int i = 0; i < grid.size(); ++i)
    {
        addImageInMosaic(mosaic, grid.getCell(i), sourceImages[i % sourceImages.size()]);
    }
    return mosaic;
}

int main(int argc, char *argv[])
{
    try
    {
        auto params = readParams(argc, argv);
        auto sourceImages = getImagesPathFromFolder(params.folder);
        auto grid = createGridWithLevels(params.levels, params.imageSizeInput[0], params.imageSizeInput[1]);
        auto mosaic = createAndFillMosaicImage(sourceImages, grid);
        mosaic.Save(params.output);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    return 0;
}
