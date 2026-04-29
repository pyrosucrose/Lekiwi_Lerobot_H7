//
// Created by Glucose_carbide on 2026-04-28.
//

#pragma once

#include <cmath>
#include <array>

/// target是否在num1和num2之间，区间会往两边扩tolerance以适配误差
template<typename T>
[[nodiscard]] constexpr bool IsBetween(const T target, const T num1, const T num2, const T tolerance)
{
    return (target >= std::min(num1, num2) - tolerance) && (target <= std::max(num1, num2) + tolerance);
}

template<std::size_t size>
void ScaleArray(std::array<float, size>& array, float limit)
{
    limit = fmaxf(fabsf(limit), 1e-6f);
    float max_abs = 0.0f;
    for (const auto num : array)
        max_abs = fmaxf(max_abs, fabsf(num));
    if (max_abs <= limit)
        return;

    const float scale = limit / max_abs;
    for (auto& num : array)
        num *= scale;
}

template <typename T, typename Cfg, std::size_t... I>
static std::array<T, sizeof...(I)> MakeArrayImpl(const std::array<Cfg, sizeof...(I)>& cfgs,
                                                 std::index_sequence<I...>)
{
    return { T(cfgs[I])... };
}

template <typename T, typename Cfg, std::size_t N>
static std::array<T, N> MakeArray(const std::array<Cfg, N>& cfgs)
{
    return MakeArrayImpl<T>(cfgs, std::make_index_sequence<N>{});
}
