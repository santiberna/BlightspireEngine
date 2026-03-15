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
#undef NDEBUG
#if defined(DISTRIBUTION)
#define NDEBUG
#define BB_DEVELOPMENT false
#else
#define BB_DEVELOPMENT true
#endif

// Platform switches
#define BB_WINDOWS 0
#define BB_LINUX 1

#if defined(_WIN64)
#define BB_PLATFORM BB_WINDOWS
#elif defined(__linux__)
#define BB_PLATFORM BB_LINUX
#else
#error "Unsupported platform"
#endif
