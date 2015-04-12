# index2d
Basic spatial index

Optimized for data that needs to be indexed by two integers and that
is tightly grouped together, but you don't know in advance what values
the indices will have, or how big the range will need to be.

## usage

    using overengineered::index2d;
    
    index2d<Widget> grid;
    grid.set(1, 2, &widget1_2);
    grid.set(3, 10, &widget3_10);
    
    grid.get(0, 0); // nullptr
    grid.get(1, 2); // widget1_2
    
    int itemCount = 0;
    grid.for_each([&itemCount](int x, int y, Widget* item) { itemCount++; });
