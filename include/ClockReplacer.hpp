#ifndef _CLOCK_REPLACER_HPP_
#define _CLOCK_REPLACER_HPP_

#include <array>
#include <optional>
#include "Replacer.hpp"

template <size_t N>
struct ClockReplacer : public Replacer<N>
{
    size_t clock_hand;
    std::array<bool, N> reference_bits;

    ClockReplacer()
        : clock_hand(0), reference_bits()
    {
        // std::array<bool,N> default-inits to false; no fill needed.
    }

    virtual ~ClockReplacer() {}

    // Remove victim per policy
    virtual std::optional<frame_id_t> victim();

    // Mark as pinned
    virtual void pin(frame_id_t frame_id);

    // Mark as unpinned
    virtual void unpin(frame_id_t frame_id);
};

#include "../src/ClockReplacer.inl"

#endif // _CLOCK_REPLACER_HPP_
