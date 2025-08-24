#ifndef _CLOCK_REPLACER_INL_
#define _CLOCK_REPLACER_INL_

#include <optional>

template <size_t N>
std::optional<frame_id_t> ClockReplacer<N>::victim()
{
    if (this->frames == NULL) return std::nullopt;

    // any unpinned?
    bool any_unpinned = false;
    for (size_t i = 0; i < this->frames->size(); ++i) {
        if ((*this->frames)[i].pin_count == 0) { any_unpinned = true; break; }
    }
    if (!any_unpinned) return std::nullopt;

    // second-chance clock
    for (;;) {
        if (reference_bits[clock_hand]) {
            reference_bits[clock_hand] = false;
        } else {
            if ((*this->frames)[clock_hand].pin_count == 0) {
                return static_cast<frame_id_t>(clock_hand);
            }
        }
        clock_hand = (clock_hand + 1) % this->frames->size();
    }
}

template <size_t N>
void ClockReplacer<N>::pin(frame_id_t frame_id)
{
    // mark as recently used and bump pin count
    reference_bits[frame_id] = true;
    (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count + 1;
}

template <size_t N>
void ClockReplacer<N>::unpin(frame_id_t frame_id)
{
    if ((*this->frames)[frame_id].pin_count > 0) {
        (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count - 1;
    }
    // do not clear reference bit here; next victim pass will handle it
}

#endif // _CLOCK_REPLACER_INL_
