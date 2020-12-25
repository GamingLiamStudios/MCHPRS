#pragma once

#include <memory>
#include "moodycamel/concurrentqueue.h"

namespace mpsc
{
    template<typename T>
    using Sender = std::weak_ptr<moodycamel::ConcurrentQueue<T>>;

    template<typename T>
    using Receiver = std::shared_ptr<moodycamel::ConcurrentQueue<T>>;
}    // namespace mpsc
