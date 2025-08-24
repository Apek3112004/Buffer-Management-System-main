/**
 * SELECT employee.first_name, employee.last_name, company.name
 * FROM employee
 * INNER JOIN company
 * ON employee.company_id = company.id
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "BufferPoolManager.hpp"
#include "ClockReplacer.hpp"
#include "DiskManager.hpp"
#include "Tables.hpp"
#include "Types.hpp"

const std::size_t BUFFERSIZE = 32;

// Helper to view page data as array of T
template <typename T>
std::array<T, PAGE_SIZE / sizeof(T> )* page_to_array(RPage page) {
    return reinterpret_cast<std::array<T, PAGE_SIZE / sizeof(T> )*>(&page.data);
}

typedef BufferPoolManager<BUFFERSIZE> BPM;

// No lambdas / no nested functions — plain helpers:
static Page blocking_fetch_page(page_id_t page_id, BPM& bpm) {
    static std::mt19937_64 gen((std::random_device())());
    static std::uniform_int_distribution<int> dis(10, 20);

    for (;;) {
        std::optional<Page> page = bpm.fetch_page(page_id);
        if (page.has_value()) return *page;

        std::fputs("No Frame Free\n", stderr);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    }
}

static void delayed_unpin(page_id_t page_id, BPM& bpm) {
    static std::mt19937_64 gen((std::random_device())());
    static std::uniform_int_distribution<int> dis(10, 20);

    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    bpm.unpin_page(page_id);
}

int main() {
    DiskManager disk_manager;
    ClockReplacer<BUFFERSIZE> replacer;
    BPM buffer_pool_manager(&disk_manager, &replacer);

    std::optional<std::vector<page_id_t> > _company_pages  = disk_manager.add_page("./files/company.bin");
    std::optional<std::vector<page_id_t> > _employee_pages = disk_manager.add_page("./files/employee.bin");

    if (!_company_pages.has_value() || !_employee_pages.has_value()) {
        std::fputs("Error opening file\n", stderr);
        return EXIT_FAILURE;
    }

    const std::vector<page_id_t>& company_pages  = *_company_pages;
    const std::vector<page_id_t>& employee_pages = *_employee_pages;

    std::cout << std::left
              << std::setw(30) << "First Name" << '\t'
              << std::setw(30) << "Last Name"  << '\t'
              << std::setw(50) << "Company Name" << "\n\n";

    // Start with first pages
    std::size_t ci = 0, ei = 0;
    if (ei == employee_pages.size() || ci == company_pages.size()) {
        std::cout << "No data.\n";
        return 0;
    }

    Page employee_page = blocking_fetch_page(employee_pages[ei], buffer_pool_manager);
    Page company_page  = blocking_fetch_page(company_pages[ci],  buffer_pool_manager);

    std::array<Employee, PAGE_SIZE / sizeof(Employee)>& earr = *page_to_array<Employee>(employee_page);
    std::array<Company,  PAGE_SIZE / sizeof(Company) >& carr = *page_to_array<Company>(company_page);

    std::size_t eidx = 0, cidx = 0;

    // Merge-join like traversal
    while (ei < employee_pages.size() && ci < company_pages.size()) {
        if (earr[eidx] == Employee() || carr[cidx] == Company()) {
            break;
        }

        if (carr[cidx].id < earr[eidx].company_id) {
            // advance company tuple
            ++cidx;
            if (cidx >= carr.size() || carr[cidx] == Company()) {
                std::thread t(delayed_unpin, company_pages[ci], std::ref(buffer_pool_manager));
                t.detach();
                ++ci;
                if (ci >= company_pages.size()) break;
                company_page = blocking_fetch_page(company_pages[ci], buffer_pool_manager);
                carr = *page_to_array<Company>(company_page);
                cidx = 0;
            }
        } else if (carr[cidx].id > earr[eidx].company_id) {
            // advance employee tuple
            ++eidx;
            if (eidx >= earr.size() || earr[eidx] == Employee()) {
                std::thread t(delayed_unpin, employee_pages[ei], std::ref(buffer_pool_manager));
                t.detach();
                ++ei;
                if (ei >= employee_pages.size()) break;
                employee_page = blocking_fetch_page(employee_pages[ei], buffer_pool_manager);
                earr = *page_to_array<Employee>(employee_page);
                eidx = 0;
            }
        } else {
            // match
            std::cout << std::left
                      << std::setw(30) << earr[eidx].fname.data() << '\t'
                      << std::setw(30) << earr[eidx].lname.data() << '\t'
                      << std::setw(50) << carr[cidx].name.data()  << '\n';

            ++eidx;
            if (eidx >= earr.size() || earr[eidx] == Employee()) {
                std::thread t(delayed_unpin, employee_pages[ei], std::ref(buffer_pool_manager));
                t.detach();
                ++ei;
                if (ei >= employee_pages.size()) break;
                employee_page = blocking_fetch_page(employee_pages[ei], buffer_pool_manager);
                earr = *page_to_array<Employee>(employee_page);
                eidx = 0;
            }
        }
    }

    std::cout << "\nNumber of I/Os: " << disk_manager.num_ios
              << "\nNumber of pages: " << disk_manager.pages.size() << '\n';
    return 0;
}
