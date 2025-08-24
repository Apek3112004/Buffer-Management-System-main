#ifndef _MRU_REPLACER_HPP_
#define _MRU_REPLACER_HPP_

#include <forward_list>
#include <optional>
#include <unordered_map>
#include "Replacer.hpp"

template <size_t N>
struct MRUReplacer : public Replacer<N>
{
    std::forward_list<frame_id_t> mru_list;
    std::unordered_map<frame_id_t, typename std::forward_list<frame_id_t>::iterator> mru_map;

    MRUReplacer() {}
    virtual ~MRUReplacer() {}

    virtual std::optional<frame_id_t> victim();
    virtual void pin(frame_id_t frame_id);
    virtual void unpin(frame_id_t frame_id);
};

#include "../src/MRUReplacer.inl"

#endif // _MRU_REPLACER_HPP_
