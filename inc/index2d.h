#include <cassert>
#include <algorithm>

namespace overengineered {


template<class T>
class index2d final
{
private:
    T** data;
    int minX, minY;
    int sizeX, sizeY;
    
public:
    index2d() : data{ nullptr } {}

    void set(int x, int y, T* item)
    {
        if (item == nullptr) return;
        ensure_assignable(x, y);

        size_t index = (y - minY) * sizeX + (x - minX);
        assert(index < capacity());
        data[index] = item;
    }

    T* get(int x, int y)
    {
        if (data == nullptr) return nullptr;
        if (x < minX || x >= minX + sizeX) return nullptr;
        if (y < minY || y >= minY + sizeY) return nullptr;
        size_t index = (y - minY) * sizeX + (x - minX);
        assert(index < capacity());
        return data[index];
    }

    size_t capacity()
    {
        return sizeX * sizeY;
    }

    void reserve(int minX, int maxX, int minY, int maxY)
    {
        int xmin = minX, xmax = maxX;
        int ymin = minY, ymax = maxY;
        if (data != nullptr)
        {
            xmin = std::min(xmin, this->minX);
            xmax = std::max(xmax, this->minX + sizeX);
            ymin = std::min(ymin, this->minY);
            ymax = std::max(ymax, this->minY + sizeY);
        }

        resize(xmin, xmax, ymin, ymax);
    }

    template<typename Func>
    void for_each(Func f)
    {
        if (data == nullptr) return;

        int x = minX, y = minY, n = capacity(), xLimit = minX + sizeX;
        for (int index = 0; index < n; index++)
        {
            if (data[index] != nullptr)
                f(x, y, data[index]);
            x++;
            if (x == xLimit)
            {
                x = minX;
                y++;
            }
        }
    }

private:
    void ensure_assignable(int x, int y)
    {
        if (data == nullptr)
        {
            resize(x - 5, x + 5, y - 5, y + 5);
            return;
        }

        int xmin = minX, xmax = minX + sizeX;
        int ymin = minY, ymax = minY + sizeY;
        bool expandX = true, expandY = true;

        if (x < xmin) xmin = x - (sizeX / 3);
        else if (x >= xmax) xmax = x + (sizeX / 3);
        else expandX = false;

        if (y < ymin) ymin = y - (sizeY / 3);
        else if (y >= ymax) ymax = y + (sizeY / 3);
        else expandY = false;

        if (expandX || expandY) resize(xmin, xmax, ymin, ymax);
    }

    void resize(int xmin, int xmax, int ymin, int ymax)
    {
        assert(xmin < xmax);
        assert(ymin < ymax);

        auto oldData = data;
        auto oldSizeX = sizeX;
        auto overlapSizeX = std::max(0, std::min(sizeX, xmax - xmin) + std::min(minX - xmin, 0));
        auto overlapSizeY = std::max(0, std::min(sizeY, ymax - ymin) + std::min(minY - ymin, 0));
        auto sourceOffset = sizeX - overlapSizeX;
        auto targetOffset = std::min(minX - xmin, 0);

        sizeX = xmax - xmin;
        sizeY = ymax - ymin;
        minX = xmin;
        minY = ymin;
        auto length = sizeX * sizeY;
        data = new T*[length];
        memset(data, 0, length * sizeof(T*));

        if (oldData == nullptr) return;

        for (auto i = 0; i < overlapSizeY; i++)
        {
            memcpy(data + targetOffset, oldData + sourceOffset, overlapSizeX * sizeof(T*));
            targetOffset += sizeX;
            sourceOffset += oldSizeX;
        }

        delete[] oldData;
    }
};


}
