#ifndef _REPLACER_HPP_
#define _REPLACER_HPP_

#include <array>
#include <cstdint>
#include <optional>

#include "Frame.hpp"
#include "Types.hpp"

template <size_t N>
struct Replacer
{
    std::array<Frame, N>* frames;

    Replacer()
    {
        frames = NULL;
    }

    virtual ~Replacer() {}

    // Remove the victim frame as defined by the replacement policy.
    virtual std::optional<frame_id_t> victim() = 0;

    // Pin a frame (should not be victimized).
    virtual void pin(frame_id_t frame_id) = 0;

    // Unpin a frame (can be victimized).
    virtual void unpin(frame_id_t frame_id) = 0;
};

// Aliases
template <size_t N> using PReplacer  = Replacer<N>*;
template <size_t N> using CPReplacer = const Replacer<N>*;
template <size_t N> using RReplacer  = Replacer<N>&;
template <size_t N> using CRReplacer = const Replacer<N>&;

#endif // _REPLACER_HPP_
