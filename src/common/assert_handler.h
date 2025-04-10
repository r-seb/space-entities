#pragma once

#define ASSERT(expression) ((expression) ? (void)0 : assert_failed(__FILE__, __LINE__))

_Noreturn void assert_failed(char const* const module, int const id);
