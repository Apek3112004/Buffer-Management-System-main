#ifndef _DISK_MANAGER_HPP_
#define _DISK_MANAGER_HPP_

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Types.hpp"
#include "Page.hpp"

struct DiskManager
{
    std::unordered_map<page_id_t, Page> pages; // Pages on the "disk"
    int64_t num_ios;                           // I/O counter

    DiskManager()
    {
        num_ios = 0;
    }

    ~DiskManager() {}

    // Read a page from disk
    std::optional<Page> read_page(page_id_t page_id);

    // Add pages from a file to the disk
    std::optional<std::vector<page_id_t> > add_page(const std::string &filename);
};

// Aliases
typedef DiskManager*       PDiskManager;
typedef const DiskManager* CPDiskManager;
typedef DiskManager&       RDiskManager;
typedef const DiskManager& CRDiskManager;

#endif // _DISK_MANAGER_HPP_
