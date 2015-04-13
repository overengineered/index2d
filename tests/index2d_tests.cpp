#include "stdafx.h"
#include "CppUnitTest.h"
#include "../inc/index2d.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using overengineered::index2d;


namespace Index2d_Tests
{		
    TEST_CLASS(Index2d_Tests)
    {
    public:
        
        TEST_METHOD(ItemAdded_CanBeRetrieved)
        {
            auto grid = index2d<double>{};
            auto item = 3.14159;

            grid.set(1, 1, &item);
            Assert::AreEqual(&item, grid.get(1, 1));
        }

        TEST_METHOD(AddingAnItem_IncreasesCapacity)
        {
            auto grid = index2d<std::string>{};
            auto item = std::string{ "hello" };
            grid.set(-500, -500, &item);
            Assert::IsTrue(grid.capacity() > 0);
        }

        TEST_METHOD(AccessingUnsetItem_ReturnsNull)
        {
            auto grid = index2d<uint8_t>{};
            auto item = static_cast<uint8_t>(3);
            grid.set(0, 1, &item);
            grid.set(1000, 0, &item);
            Assert::IsTrue(nullptr == grid.get(990, 1));
        }

        TEST_METHOD(AddedItems_CanBeIterated)
        {
            const auto itemCount = 100;
            auto grid = index2d<int>{};

            int buffer[itemCount];
            for (auto i = 0; i < itemCount; i++)
            {
                buffer[i] = i;
                grid.set(-i, -i, buffer + i);
            }

            auto index = 0;
            grid.for_each([&index, &grid](int x, int y, int* item)
            {
                Assert::AreEqual(*item, -x);
                Assert::AreEqual(*item, -y);
                Assert::AreEqual(index, *grid.get(-index, -index));
                index++;
            });

            Assert::AreEqual(itemCount, index);
        }

    };
}