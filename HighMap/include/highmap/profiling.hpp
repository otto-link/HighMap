#pragma once

// Optional CPU instrumentation. No expressions are evaluated in normal builds.
#ifdef HIGHMAP_ENABLE_TRACY
#include <string_view>
#include <tracy/Tracy.hpp>
#ifndef TRACY_ENABLE
#error HIGHMAP_ENABLE_TRACY requires a Tracy client built with TRACY_ENABLE
#endif
#define HMAP_PROFILE_ZONE(name) ZoneScopedN(name)
#define HMAP_PROFILE_TEXT(text)                                                          \
  do                                                                                     \
  {                                                                                      \
    if (TracyIsConnected)                                                                \
    {                                                                                    \
      const auto            &hmap_profile_text = (text);                                 \
      const std::string_view hmap_profile_view(hmap_profile_text);                       \
      ZoneText(hmap_profile_view.data(), hmap_profile_view.size());                      \
    }                                                                                    \
  } while (false)
#else
#define HMAP_PROFILE_ZONE(name)
#define HMAP_PROFILE_TEXT(text)                                                          \
  do                                                                                     \
  {                                                                                      \
  } while (false)
#endif
