#pragma once

// Class Decorations

// NOLINTBEGIN
#define NON_COPYABLE(ClassName)           \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define NON_MOVABLE(ClassName)       \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define DEFAULT_MOVABLE(ClassName)             \
    ClassName(ClassName&&) noexcept = default; \
    ClassName& operator=(ClassName&&) noexcept = default;
// NOLINTEND

// Development and debug switches
#if defined(BB_DEVELOPMENT)
#undef BB_DEVELOPMENT
#define BB_DEVELOPMENT 1
#else
#undef BB_DEVELOPMENT
#define BB_DEVELOPMENT 0
#endif

// Platform switches
#define BB_WINDOWS 0
#define BB_LINUX 1

#if defined(_WIN64)
#define BB_PLATFORM BB_WINDOWS
#elif defined(__linux__)
#define BB_PLATFORM BB_LINUX
#else
#error "Blightspire does not support current platform"
#endif

#if !defined(__LP64__) && !defined(_WIN64) && (!defined(__x86_64__) || defined(__ILP32__))
#error "Blightspire does not support x32 architectures"
#endif