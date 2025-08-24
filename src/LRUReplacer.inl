#ifndef _LRU_REPLACER_INL_
#define _LRU_REPLACER_INL_

#include <optional>
#include <list>
#include <unordered_map>

template <size_t N>
std::optional<frame_id_t> LRUReplacer<N>::victim()
{
    if (this->frames == NULL)
    {
        return std::nullopt;
    }

    typename std::list<frame_id_t>::reverse_iterator it;
    for (it = this->lru_list.rbegin(); it != this->lru_list.rend(); ++it)
    {
        frame_id_t fid = *it;
        if ((*this->frames)[fid].pin_count == 0)
        {
            this->lru_list.erase(this->lru_map[fid]);
            this->lru_map.erase(fid);
            return fid;
        }
    }

    return std::nullopt;
}

template <size_t N>
void LRUReplacer<N>::pin(frame_id_t frame_id)
{
    if (this->lru_map.find(frame_id) != this->lru_map.end())
    {
        this->lru_list.erase(this->lru_map[frame_id]);
    }
    this->lru_list.push_front(frame_id);
    this->lru_map[frame_id] = this->lru_list.begin();
    (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count + 1;
}

template <size_t N>
void LRUReplacer<N>::unpin(frame_id_t frame_id)
{
    if (this->lru_map.find(frame_id) != this->lru_map.end())
    {
        (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count - 1;
    }
}

#endif // _LRU_REPLACER_INL_
