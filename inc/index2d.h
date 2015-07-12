// Copyright(c) 2015 Juozas Kontvainis

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
// files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <boost\container\flat_map.hpp>
#include <array>
#include <memory>

#ifdef _MSC_VER
#define OE_NOEXCEPT _NOEXCEPT
#else
#define OE_NOEXCEPT noexcept
#endif

namespace overengineered {


template<class T, uint8_t BlockWidth = 10>
class index2d final
{
private:
    using block = std::array<T*, BlockWidth*BlockWidth>;
    boost::container::flat_map<uint64_t, std::unique_ptr<block>> container;
    
public:
    index2d()
    {
        static_assert(BlockWidth > 0, "Positive number is required for BlockWidth");
    }

    index2d(const index2d& rhs)
    {
        for (const auto& it : rhs.container)
        {
            container.emplace_hint(container.end(),
                it.first, std::unique_ptr<block>(new block(*it.second)));
        }
    }

    index2d(index2d&& rhs)
        : container(std::move(rhs.container))
    {
    }

    void set(int x, int y, T* item)
    {
        int qx, rx, qy, ry;
        modulo(x, BlockWidth, qx, rx);
        modulo(y, BlockWidth, qy, ry);

        int64_t key = ((int64_t)qy << 32) | (qx & 0x00000000FFFFFFFF);

        auto entry = container.find(key);
        if (entry == container.end())
        {
            entry = container.emplace_hint(container.end(),
                key, std::unique_ptr<block>(new block()));
        }

        block& data = *entry->second.get();
        data[rx + ry * BlockWidth] = item;
    }

    T* get(int x, int y) const
    {
        int qx, rx, qy, ry;
        modulo(x, BlockWidth, qx, rx);
        modulo(y, BlockWidth, qy, ry);

        int64_t key = ((int64_t)qy << 32) | (qx & 0x00000000FFFFFFFF);

        auto entry = container.find(key);
        if (entry == container.end()) return nullptr;

        block& data = *entry->second.get();
        return data[rx + ry * BlockWidth];
    }

    size_t capacity() const
    {
        return container.size() * BlockWidth * BlockWidth;
    }

    template<typename Func>
    void for_each(Func f) const
    {
        for (const auto& it : container)
        {
            auto key = it.first;
            int bx = (int)(key & 0xFFFFFFFF);
            int by = (int)((key & 0xFFFFFFFF00000000) >> 32);
            block& data = *it.second.get();
            for (int i = 0; i < BlockWidth; i++)
            {
                for (int j = 0; j < BlockWidth; j++)
                {
                    T* value = data[i + j * BlockWidth];
                    if (value == nullptr) continue;
                    f(bx * BlockWidth + i, by * BlockWidth + j, value);
                }
            }
        }
    }

    index2d& operator=(const index2d& rhs) OE_NOEXCEPT
    {
        if (&rhs == this) return *this;
        container.clear();
        for (const auto& it : rhs.container)
        {
            container.emplace_hint(container.end(),
                it.first, std::unique_ptr<block>(new block(*it.second)));
        }
        return *this;
    }

    index2d& operator=(index2d&& rhs) OE_NOEXCEPT
    {
        if (&rhs == this) return *this;
        container = std::move(rhs.container);
        return *this;
    }

private:
    // calculates positive remainder and accompanying quotient
    static void modulo(int a, int b, int& q, int& r) {
        r = a % b;
        q = a / b;
        if (r < 0)
        {
            q -= 1;
            r += b;
        }
    }
};


}
