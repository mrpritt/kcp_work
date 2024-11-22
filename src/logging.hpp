/**
 * \file logging.hpp
 *   \author Marcus Ritt <marcus.ritt@inf.ufrgs.br>
 *   \date Time-stamp: <2024-08-17 08:01:21 ritt>
 */
#pragma once

#include <iostream>
#include <vector>

#include <boost/any.hpp>

#include "timer.hpp"

#define FMT_HEADER_ONLY
#include "fmt/color.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include "fmt/ranges.h"

using osb_iterator = std::ostreambuf_iterator<char>;

struct options_counter {
  int count = 0;
};
inline void validate(boost::any &v, std::vector<std::string> const &xs, options_counter *, long) {
  if (v.empty())
    v = options_counter{1};
  else
    ++boost::any_cast<options_counter &>(v).count;
}

namespace logging {
extern timer start;
extern options_counter verbosec, debugc;
inline double elapsed() { return start.elapsed(); }
} // namespace logging

inline bool verbose(int level = 1) { return logging::verbosec.count >= level; }
inline bool debug(int level = 1) { return logging::debugc.count >= level; }

// helpers for verbose and timestamped output
template <typename... Args> void vtprint(int level, std::ostream &os, std::string_view format_str, Args &&...args) {
  if (logging::verbosec.count >= level) {
    fmt::print(os, "({:7.2f}) ", logging::elapsed());
    fmt::vprint(os, format_str, fmt::make_format_args(args...));
  }
}

template <typename... Args> void vtprint(int level, const fmt::text_style &ts, std::string format_str, Args &&...args) {
  if (logging::verbosec.count >= level) {
    fmt::print(ts, "({:7.2f}) ", logging::elapsed());
    fmt::vformat_to(osb_iterator(std::cout), ts, fmt::detail::to_string_view(format_str), fmt::make_format_args(args...));
  }
}

template <typename... Args> void vtprint(int level, std::string_view format_str, Args &&...args) { vtprint(level, std::cout, format_str, std::forward<Args>(args)...); }

template <typename... Args> void vprint(int level, std::ostream &os, std::string_view format_str, Args &&...args) {
  if (logging::verbosec.count >= level)
    fmt::vprint(os, format_str, fmt::make_format_args(args...));
}

template <typename... Args> void vprint(int level, const fmt::text_style &ts, std::string format_str, Args &&...args) {
  if (logging::verbosec.count >= level)
    fmt::vformat_to(osb_iterator(std::cout), ts, fmt::detail::to_string_view(format_str), fmt::make_format_args(args...));
}

template <typename... Args> void vprint(int level, std::string_view format_str, Args &&...args) {
  if (logging::verbosec.count >= level)
    fmt::vprint(format_str, fmt::make_format_args(args...));
}

// debug
template <typename... Args> void dtprint(int level, std::ostream &os, std::string_view format_str, Args &&...args) {
#if !defined(NDEBUG)
  if (logging::debugc.count >= level) {
    fmt::print(os, "({:7.2f}) ", logging::elapsed());
    fmt::vprint(os, format_str, fmt::make_format_args(args...));
  }
#endif
}

template <typename... Args> void dtprint(int level, const fmt::text_style &ts, std::string_view format_str, Args &&...args) {
#if !defined(NDEBUG)
  if (logging::debugc.count >= level) {
    fmt::print(ts, "({:7.2f}) ", logging::elapsed());
    fmt::vformat_to(osb_iterator(std::cout), ts, fmt::detail::to_string_view(format_str), fmt::make_format_args(args...));
  }
#endif
}

template <typename... Args> void dtprint(int level, std::string_view format_str, Args &&...args) { dtprint(level, std::cout, format_str, std::forward<Args>(args)...); }

template <typename... Args> void dprint(int level, std::ostream &os, std::string_view format_str, Args &&...args) {
#if !defined(NDEBUG)
  if (logging::debugc.count >= level)
    fmt::vprint(os, format_str, fmt::make_format_args(args...));
#endif
}

template <typename... Args> void dprint(int level, const fmt::text_style &ts, std::string_view format_str, Args &&...args) {
#if !defined(NDEBUG)
  if (logging::debugc.count >= level) {
    fmt::vformat_to(osb_iterator(std::cout), ts, fmt::detail::to_string_view(format_str), fmt::make_format_args(args...));
  }
#endif
}

template <typename... Args> void dprint(int level, std::string_view format_str, Args &&...args) {
#if !defined(NDEBUG)
  if (logging::debugc.count >= level)
    fmt::vprint(format_str, fmt::make_format_args(args...));
#endif
}

// timestamps
template <typename... Args> void tprint(std::ostream &os, std::string_view format_str, Args &&...args) {
  fmt::print(os, "({:7.2f}) ", logging::elapsed());
  fmt::vprint(os, format_str, fmt::make_format_args(args...));
}

template <typename... Args> void tprint(const fmt::text_style &ts, std::string_view format_str, Args &&...args) {
  fmt::print(ts, "({:7.2f}) ", logging::elapsed());
  fmt::vformat_to(osb_iterator(std::cout), ts, fmt::detail::to_string_view(format_str), fmt::make_format_args(args...));
}

template <typename... Args> void tprint(std::string_view format_str, Args &&...args) { tprint(std::cout, format_str, std::forward<Args>(args)...); }

std::string get_date_string(Timepoint t);
