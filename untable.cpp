#include <fstream>
#include <iostream>
#include <iomanip>   // for setw, left

#include "Tables.hpp"

int main() {
    try {
        // ----- Employees -----
        std::ifstream file("./files/employee.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file\n";
            return 1;
        }

        Employee employee;
        while (file.read(reinterpret_cast<char*>(&employee), sizeof(Employee))) {
            std::cout << std::left
                      << std::setw(3)  << employee.id        << '\t'
                      << std::setw(3)  << employee.company_id << '\t'
                      << std::setw(60) << employee.fname.data() << '\t'
                      << std::setw(60) << employee.lname.data() << '\n';
        }

        file.close();
        file.clear();

        // ----- Companies -----
        file.open("./files/company.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file\n";
            return 1;
        }

        Company company;
        while (file.read(reinterpret_cast<char*>(&company), sizeof(Company))) {
            std::cout << std::left
                      << std::setw(3)  << company.id            << '\t'
                      << std::setw(62) << company.name.data()   << '\t'
                      << std::setw(62) << company.slogan.data() << '\n';
        }

        file.close();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
