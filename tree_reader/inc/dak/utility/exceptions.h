#pragma once

#include <functional>

namespace dak::utility
{
   // Call the function and catch all exceptions.
   void with_no_exceptions(const std::function<void()>& func);
}
