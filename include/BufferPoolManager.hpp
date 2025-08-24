#ifndef _BUFFER_POOL_MANAGER_HPP_
#define _BUFFER_POOL_MANAGER_HPP_

#include <array>
#include <optional>
#include <unordered_map>

#include "Types.hpp"
#include "DiskManager.hpp"
#include "Frame.hpp"
#include "Page.hpp"
#include "Replacer.hpp"

template <size_t N = 10>
struct BufferPoolManager
{
    // Simple free-list helper (no lambdas, no iota)
    struct FreeList
    {
        std::array<frame_id_t, N> free_list;
        size_t size;

        FreeList()
        {
            // Fill as {N-1, N-2, ..., 0} so pop() returns 0,1,...,N-1
            for (size_t i = 0; i < N; ++i)
            {
                free_list[i] = static_cast<frame_id_t>(N - 1 - i);
            }
            size = N;
        }

        // Pop a frame id from the free list
        frame_id_t pop()
        {
            // (Assumes caller checks size > 0)
            --size;
            return free_list[size];
        }

        // Push a frame id back to the free list
        void push(frame_id_t frame_id)
        {
            free_list[size] = frame_id;
            ++size;
        }
    } free_list; // The free list of frames in the buffer pool

    std::array<Frame, N> frames;                           // The buffer pool
    std::unordered_map<page_id_t, frame_id_t> page_table;  // page_id -> frame_id
    PDiskManager disk_manager;                              // Owned by caller
    PReplacer<N> replacer;                                  // Owned by caller

    // Creates a new BufferPoolManager
    explicit BufferPoolManager(PDiskManager disk_mgr, PReplacer<N> repl);

    // Destroys the BufferPoolManager
    ~BufferPoolManager() = default;

    // Fetch a page and pin it
    std::optional<Page> fetch_page(page_id_t page_id);

    // Unpin a page
    bool unpin_page(page_id_t page_id);
};

#include "../src/BufferPoolManager.inl"

#endif // _BUFFER_POOL_MANAGER_HPP_
