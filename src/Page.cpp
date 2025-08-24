#include "Page.hpp"

page_id_t Page::next_id = 0;

page_id_t Page::generate_page_id() {
    return next_id++;   // post-increment: return then increment
}
