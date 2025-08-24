#include "DiskManager.hpp"
#include <fstream>
#include <array>
#include <vector>
#include <optional>
#include <string>
#include <cstring>   // for memset

using namespace std;

optional<Page> DiskManager::read_page(page_id_t page_id)
{
    num_ios = num_ios + 1;

    // Check if the page exists
    if (pages.find(page_id) != pages.end())
    {
        return pages[page_id];
    }

    return nullopt;
}

optional<vector<page_id_t> > DiskManager::add_page(const string &filename)
{
    ifstream file(filename.c_str(), ios::binary);
    if (!file.is_open())
    {
        return nullopt;
    }

    array<char, PAGE_SIZE> data;
    data.fill(0);

    vector<page_id_t> page_ids;

    // Read full pages
    while (file.read(data.data(), PAGE_SIZE))
    {
        Page page;                      // create a page
        page.id = Page::generate_page_id();

        // copy data into page
        for (size_t i = 0; i < PAGE_SIZE; ++i)
        {
            page.data[i] = data[i];
        }

        pages[page.id] = page;
        page_ids.push_back(page.id);

        data.fill(0);
    }

    // Handle last partial page
    if (file.gcount() > 0)
    {
        Page page;
        page.id = Page::generate_page_id();

        for (size_t i = 0; i < PAGE_SIZE; ++i)
        {
            page.data[i] = data[i];
        }

        pages[page.id] = page;
        page_ids.push_back(page.id);
    }

    return page_ids;
}
