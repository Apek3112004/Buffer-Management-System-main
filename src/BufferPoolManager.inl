#ifndef _BUFFER_POOL_MANAGER_INL_
#define _BUFFER_POOL_MANAGER_INL_

#include <iostream>
#include <optional>
#include <map>
#include <cstdio>
#include "DiskManager.hpp"
#include "Frame.hpp"
#include "Page.hpp"
#include "Replacer.hpp"

// Template Buffer Pool Manager
template <size_t N>
class BufferPoolManager
{
private:
    PDiskManager disk_manager;
    PReplacer<N> replacer;
    Frame frames[N];
    std::map<page_id_t, int> page_table;
    FreeList free_list;

public:
    BufferPoolManager(PDiskManager _disk_manager, PReplacer<N> _replacer)
    {
        disk_manager = _disk_manager;
        replacer = _replacer;
        replacer->frames = &this->frames;
    }

    std::optional<Page> fetch_page(page_id_t page_id)
    {
        typename std::map<page_id_t, int>::iterator it = page_table.find(page_id);
        if (it != page_table.end())
        {
            int frame_id = it->second;
            Frame &frame = frames[frame_id];
            replacer->pin(frame_id);

            std::fprintf(stderr, "Hit    Page %4u\n", page_id);
            return frame.page;
        }

        return std::nullopt;
    }

    bool unpin_page(page_id_t page_id)
    {
        typename std::map<page_id_t, int>::iterator it = page_table.find(page_id);
        if (it != page_table.end())
        {
            int frame_id = it->second;
            Frame &frame = frames[frame_id];
            replacer->unpin(frame_id);
            return true;
        }
        return false;
    }

    /*
    Responsibilities:
    - Maintain fixed-size pool of frames (frames[N]), each holds one page.
    - Map page_id -> frame index using page_table.
    - Use replacer (LRU/MRU/Clock) to decide victims, pin/unpin pages.
    - Use free_list to allocate free frames quickly.
    - Provide APIs fetch_page() and unpin_page().
    */
};

#endif // _BUFFER_POOL_MANAGER_INL_
