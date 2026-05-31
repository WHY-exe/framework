#pragma once

#define DEL_COPY_CTOR(class_name)                                                                  \
    class_name(const class_name &)            = delete;                                            \
    class_name &operator=(const class_name &) = delete;

#define DEFAULT_COPY_CTOR(class_name)                                                              \
    class_name(const class_name &)            = default;                                           \
    class_name &operator=(const class_name &) = default;

#define DEL_MOVE_CTOR(class_name)                                                                  \
    class_name(class_name &&)            = delete;                                                 \
    class_name &operator=(class_name &&) = delete;

#define DEFAULT_MOVE_CTOR(class_name)                                                              \
    class_name(class_name &&) noexcept            = default;                                       \
    class_name &operator=(class_name &&) noexcept = default;
