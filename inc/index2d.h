#include <cassert>
#include <algorithm>

#ifdef _MSC_VER
#define OE_NOEXCEPT _NOEXCEPT
#else
#define OE_NOEXCEPT noexcept
#endif

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

    ~index2d() OE_NOEXCEPT
    {
        if (data != nullptr) delete[] data;
    }

    index2d(const index2d& rhs) OE_NOEXCEPT
        : minX{ rhs.minX }, minY{ rhs.minY }, sizeX{ rhs.sizeX }, sizeY{ rhs.sizeY }
    {
        auto length = capacity();
        data = new T*[length];
        memcpy(data, rhs.data, length * sizeof(T*));
    }

    index2d(index2d&& rhs) OE_NOEXCEPT
        : data{ rhs.data }, minX{ rhs.minX }, minY{ rhs.minY }, sizeX{ rhs.sizeX }, sizeY{ rhs.sizeY }
    {
        rhs.data = nullptr;
    }

    void set(int x, int y, T* item)
    {
        if (item == nullptr) return;
        ensure_assignable(x, y);

        size_t index = (y - minY) * sizeX + (x - minX);
        assert(index < capacity());
        data[index] = item;
    }

    T* get(int x, int y) const
    {
        if (data == nullptr) return nullptr;
        if (x < minX || x >= minX + sizeX) return nullptr;
        if (y < minY || y >= minY + sizeY) return nullptr;
        size_t index = (y - minY) * sizeX + (x - minX);
        assert(index < capacity());
        return data[index];
    }

    size_t capacity() const
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
    void for_each(Func f) const
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

    index2d& operator=(const index2d& rhs) OE_NOEXCEPT
    {
        if (&rhs == this) return *this;

        if (rhs.data == nullptr)
        {
            if (data != nullptr) delete[] data;
            data = nullptr;
            return *this;
        }

        bool reallocate = capacity() != rhs.capacity();
        minX = rhs.minX; minY = rhs.minY;
        sizeX = rhs.sizeX; sizeY = rhs.sizeY;

        int length = capacity();
        if (reallocate)
        {
            if (data != nullptr) delete[] data;
            data = new T*[length];
        }

        memcpy(data, rhs.data, length * sizeof(T*));
        return *this;
    }

    index2d& operator=(index2d&& rhs) OE_NOEXCEPT
    {
        if (&rhs == this) return *this;
        data = rhs.data;
        rhs.data = nullptr;
        minX = rhs.minX; minY = rhs.minY;
        sizeX = rhs.sizeX; sizeY = rhs.sizeY;
        return *this;
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
        auto targetOffset = std::max(minX - xmin, 0) + std::max(minY - ymin, 0) * (xmax - xmin);

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
