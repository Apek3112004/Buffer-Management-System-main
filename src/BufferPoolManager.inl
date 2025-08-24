#ifndef _BUFFER_POOL_MANAGER_INL_
#define _BUFFER_POOL_MANAGER_INL_

#include <cstdio>
#include <iostream>
#include <optional>

template <size_t N>
BufferPoolManager<N>::BufferPoolManager(PDiskManager disk_mgr, PReplacer<N> repl)
{
    disk_manager = disk_mgr;
    replacer     = repl;
    replacer->frames = &this->frames;   // replacers expect std::array<Frame,N>*
    // free_list is already constructed by its own default ctor
}

template <size_t N>
std::optional<Page> BufferPoolManager<N>::fetch_page(page_id_t page_id)
{
    typename std::unordered_map<page_id_t, frame_id_t>::iterator it = page_table.find(page_id);
    if (it != page_table.end())
    {
        frame_id_t frame_id = it->second;
        replacer->pin(frame_id);
        std::fprintf(stderr, "Hit    Page %4u\n", page_id);
        return frames[frame_id].page;
    }

    // Not resident: minimal version (your original behavior) — don’t load from disk here.
    return std::nullopt;
}

template <size_t N>
bool BufferPoolManager<N>::unpin_page(page_id_t page_id)
{
    typename std::unordered_map<page_id_t, frame_id_t>::iterator it = page_table.find(page_id);
    if (it != page_table.end())
    {
        frame_id_t frame_id = it->second;
        replacer->unpin(frame_id);
        return true;
    }
    return false;
}

#endif // _BUFFER_POOL_MANAGER_INL_
