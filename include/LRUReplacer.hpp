#ifndef _LRU_REPLACER_HPP_
#define _LRU_REPLACER_HPP_

#include <list>
#include <optional>
#include <unordered_map>
#include "Replacer.hpp"

template <size_t N>
struct LRUReplacer : public Replacer<N>
{
    std::list<frame_id_t> lru_list;
    std::unordered_map<frame_id_t, typename std::list<frame_id_t>::iterator> lru_map;

    LRUReplacer() {}
    virtual ~LRUReplacer() {}

    virtual std::optional<frame_id_t> victim();
    virtual void pin(frame_id_t frame_id);
    virtual void unpin(frame_id_t frame_id);
};

#include "../src/LRUReplacer.inl"

#endif // _LRU_REPLACER_HPP_
