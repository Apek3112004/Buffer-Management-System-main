#ifndef _CLOCK_REPLACER_INL_
#define _CLOCK_REPLACER_INL_

#include <vector>
#include <optional>

template <size_t N>
class ClockReplacer
{
public:
    std::vector<Frame>* frames;       // pointer to frame array
    std::vector<bool> reference_bits; // reference bits
    size_t clock_hand;                // current hand

    ClockReplacer()
    {
        frames = NULL;
        clock_hand = 0;
    }

    std::optional<frame_id_t> victim()
    {
        if (frames == NULL)
        {
            return std::nullopt;
        }

        // Find if any unpinned frame exists
        size_t victim_index = frames->size(); 
        for (size_t i = 0; i < frames->size(); i++)
        {
            if ((*frames)[i].pin_count == 0)
            {
                victim_index = i;
                break;
            }
        }

        if (victim_index == frames->size())
        {
            return std::nullopt;
        }

        // Clock loop
        while (true)
        {
            if (reference_bits[clock_hand])
            {
                reference_bits[clock_hand] = false; // second chance
            }
            else
            {
                if ((*frames)[clock_hand].pin_count == 0)
                {
                    return (frame_id_t)clock_hand;
                }
            }
            clock_hand = (clock_hand + 1) % frames->size();
        }
    }
};

#endif // _CLOCK_REPLACER_INL_
