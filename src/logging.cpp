/**
 * \file logging.cpp
 *   \author Marcus Ritt <marcus.ritt@inf.ufrgs.br>
 *   \date Time-stamp: <2023-01-20 19:19:13 ritt>
 */
#include "logging.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
using namespace std;

namespace logging {
timer start;
options_counter verbosec, debugc;
} // namespace logging
