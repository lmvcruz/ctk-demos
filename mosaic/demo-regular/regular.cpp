#include <iostream>
#include <vector>
#include <exception>

#include <argparse/argparse.hpp>

#include "ctk/geometry/point.h"
#include "ctk/image/rgb_image.h"


#include "grid.h"
#include "misc.h"

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

Grid createRegularGrid(int gridWidth, int gridHeight, int cellWidth, int cellHeight, int offset)
{
    constexpr int fullCellWidth = cellWidth + offset;
    constexpr int fullCellHeight= cellHeight + offset;

    Grid grid;
    for (int gx = 0; gx < gridWidth; ++gx)
    {
        int ox = offset + gx * fullCellWidth;
        for (int gy = 0; gy < gridHeight; ++gy)
        {
            int oy = offset + gy * fullCellHeight;
            grid.createAndAddCell({ox, oy}, cellWidth, cellHeight);
        }
    }
    return grid;
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
        return 1;
    }
    return 0;
}
