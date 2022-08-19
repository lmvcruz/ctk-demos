#include <vector>
#include <exception>

#include "ctk/geometry/point.h"

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
            throw std::runtime_error("[Grid] Cell origin must be non-negative");
        }
        if (width <= 0 || height <= 0)
        {
            throw std::runtime_error("[Grid] Cell size must be positive");
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
