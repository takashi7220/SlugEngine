#pragma once

#include "core/reflection/ReflectionService.hpp"
#include "core/reflection/Type.hpp"
#include "core/reflection/TypeRegister.hpp"
#include <type_traits>
#include <concepts>


#pragma once

#include <type_traits>

#if defined(__clang__)
#define SLUG_REFLECTION_ANNOTATE(...) \
        [[clang::annotate(__VA_ARGS__)]]
#else
#define SLUG_REFLECTION_ANNOTATE(...)
#endif

namespace slug::core
{
template<typename T>
struct ReflectionTraits
{
    static constexpr bool Enabled = false;
};
template<typename T>
concept HasInternalReflection = requires(const T & obj)
{
    {
        obj.__reflect_marker()
    } -> std::same_as<std::type_identity<T>>;
};

template<typename T>
concept HasExternalReflection = ReflectionTraits<T>::Enabled;

template<typename T>
concept Reflectable = HasInternalReflection<std::remove_cvref_t<T>> || HasExternalReflection<std::remove_cvref_t<T>>;

template<typename T>
inline constexpr bool HasReflection = Reflectable<T>;
}

// クラス内登録
#define SLUG_DECLARE_REFLECTION()                         \
    SLUG_REFLECTION_ANNOTATE("reflection:internal")       \
    constexpr auto GetReflectionMarker() const            \
    {                                                     \
        return std::type_identity<                        \
            std::remove_cvref_t<decltype(*this)>>{};      \
    }

// 外部登録
#define SLUG_EXTERNAL_REFLECTION(...)                     \
    template<>                                            \
    struct slug::core::ReflectionTraits<__VA_ARGS__>      \
    {                                                     \
        using Type = __VA_ARGS__;                         \
        static constexpr bool Enabled = true;             \
                                                          \
        SLUG_REFLECTION_ANNOTATE("reflection:external")   \
        static void GetReflectionMarker();                \
    };

