#ifndef _MRU_REPLACER_INL_
#define _MRU_REPLACER_INL_

#include <optional>
#include <forward_list>
#include <unordered_map>

template <size_t N>
std::optional<frame_id_t> MRUReplacer<N>::victim()
{
    if (this->frames == NULL)
    {
        return std::nullopt;
    }

    typename std::forward_list<frame_id_t>::iterator it;
    for (it = this->mru_list.begin(); it != this->mru_list.end(); ++it)
    {
        frame_id_t fid = *it;
        if ((*this->frames)[fid].pin_count == 0)
        {
            this->mru_list.erase_after(this->mru_map[fid]);
            this->mru_map.erase(fid);
            return fid;
        }
    }

    return std::nullopt;
}

template <size_t N>
void MRUReplacer<N>::pin(frame_id_t frame_id)
{
    if (this->mru_map.find(frame_id) != this->mru_map.end())
    {
        this->mru_list.erase_after(this->mru_map[frame_id]);
    }
    this->mru_list.push_front(frame_id);
    this->mru_map[frame_id] = this->mru_list.before_begin();
    (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count + 1;
}

template <size_t N>
void MRUReplacer<N>::unpin(frame_id_t frame_id)
{
    if (this->mru_map.find(frame_id) != this->mru_map.end())
    {
        (*this->frames)[frame_id].pin_count = (*this->frames)[frame_id].pin_count - 1;
    }
}

#endif // _MRU_REPLACER_INL_
