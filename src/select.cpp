/**
 * SELECT employee.first_name, employee.last_name, company.name
 * FROM employee, company
 * WHERE employee.company_id = company.id
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
#include "LRUReplacer.hpp"
#include "MRUReplacer.hpp"
#include "Types.hpp"
#include "Tables.hpp"

const std::size_t BUFFERSIZE = 32;

// Helper to view page data as array of T
template <typename T>
std::array<T, PAGE_SIZE / sizeof(T> )* page_to_array(RPage page) {
    return reinterpret_cast<std::array<T, PAGE_SIZE / sizeof(T> )*>(&page.data);
}

// Type alias for brevity
typedef BufferPoolManager<BUFFERSIZE> BPM;

// Block until the page is fetched (no lambdas)
static Page blocking_fetch_page(page_id_t page_id, BPM& bpm) {
    static std::mt19937_64 gen((std::random_device())());
    static std::uniform_int_distribution<int> dis(10, 20);

    for (;;) {
        std::optional<Page> page = bpm.fetch_page(page_id);
        if (page.has_value()) {
            return *page;
        }
        std::fputs("No Frame Free\n", stderr);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    }
}

// Delayed unpin (no lambdas)
static void delayed_unpin(page_id_t page_id, BPM& bpm) {
    static std::mt19937_64 gen((std::random_device())());
    static std::uniform_int_distribution<int> dis(10, 20);

    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    bpm.unpin_page(page_id);
}

int main() {
    DiskManager disk_manager;
    ClockReplacer<BUFFERSIZE> replacer;              // or LRU/MRU
    BPM buffer_pool_manager(&disk_manager, &replacer);

    std::optional<std::vector<page_id_t> > _company_pages = disk_manager.add_page("./files/company.bin");
    std::optional<std::vector<page_id_t> > _employee_pages = disk_manager.add_page("./files/employee.bin");

    if (!_company_pages.has_value() || !_employee_pages.has_value()) {
        std::fputs("Error opening file\n", stderr);
        return EXIT_FAILURE;
    }

    const std::vector<page_id_t>& company_pages  = *_company_pages;
    const std::vector<page_id_t>& employee_pages = *_employee_pages;

    // Header
    std::cout << std::left
              << std::setw(30) << "First Name" << '\t'
              << std::setw(30) << "Last Name"  << '\t'
              << std::setw(50) << "Company Name" << "\n\n";

    // Nested loops join (no range-for, explicit indices)
    for (std::size_t ei = 0; ei < employee_pages.size(); ++ei) {
        page_id_t employee_page_id = employee_pages[ei];
        Page epage = blocking_fetch_page(employee_page_id, buffer_pool_manager);
        std::array<Employee, PAGE_SIZE / sizeof(Employee)>& employees = *page_to_array<Employee>(epage);

        for (std::size_t eidx = 0; eidx < employees.size(); ++eidx) {
            Employee& employee = employees[eidx];
            if (employee == Employee()) break;

            for (std::size_t ci = 0; ci < company_pages.size(); ++ci) {
                page_id_t company_page_id = company_pages[ci];
                Page cpage = blocking_fetch_page(company_page_id, buffer_pool_manager);
                std::array<Company, PAGE_SIZE / sizeof(Company)>& companies = *page_to_array<Company>(cpage);

                for (std::size_t cidx = 0; cidx < companies.size(); ++cidx) {
                    Company& company = companies[cidx];
                    if (company == Company()) break;

                    if (company.id == employee.company_id) {
                        std::cout << std::left
                                  << std::setw(30) << employee.fname.data() << '\t'
                                  << std::setw(30) << employee.lname.data() << '\t'
                                  << std::setw(50) << company.name.data()  << '\n';
                    }
                }

                std::thread t(delayed_unpin, company_page_id, std::ref(buffer_pool_manager));
                t.detach();
            }
        }

        std::thread t(delayed_unpin, employee_page_id, std::ref(buffer_pool_manager));
        t.detach();
    }

    std::cout << "\nNumber of I/Os: " << disk_manager.num_ios
              << "\nNumber of pages: " << disk_manager.pages.size() << '\n';
    return 0;
}
