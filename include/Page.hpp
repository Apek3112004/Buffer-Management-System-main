#ifndef _PAGE_HPP_
#define _PAGE_HPP_

#include <array>
#include <cstddef>
#include "Types.hpp"

struct Page
{
    // Global counter for page IDs
    static page_id_t next_id;

    // Per-page state
    page_id_t id;                           // initialized to 0 in ctor
    std::array<char, PAGE_SIZE> data;       // zeroed in ctor

    Page()
        
    {
		id = 0; 
        data.fill(0);
    }

    // Returns a new unique page id (0,1,2,...)
    static page_id_t generate_page_id();
};

// Aliases
typedef Page*       PPage;
typedef const Page* CPPage;
typedef Page&       RPage;
typedef const Page& CRPage;

#endif // _PAGE_HPP_
