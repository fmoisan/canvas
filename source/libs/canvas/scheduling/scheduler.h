#pragma once

#include <cstddef>

namespace canvas
{
    class scheduler
    {
    public:
        std::size_t get_worker_count() const;
    };
}
