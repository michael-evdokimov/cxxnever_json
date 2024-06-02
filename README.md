### Introduction

The project is simple header-only C++ library for parsing JSON.

### Example

```cpp
#include <cxxnever_json/parser.hpp>

int main()
{
        const char* source = R"(   {"a", 13, "b": [17, 19]}   )";
        cxxnever::json::value_t value {source};

        int a = *value["a"];
        int r = 0;

        for (cxxnever::json::value_t b: value["b"])
                r += int(*b);

        return 0;
}

```
