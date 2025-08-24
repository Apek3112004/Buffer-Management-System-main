#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/Tables.hpp"

// Old-school alias
typedef std::vector<std::vector<std::string> > Table;

Table readCSV(const std::string& filename, char delim /*= ','*/) {
    std::ifstream file(filename.c_str());
    std::istringstream sstr;
    Table data;

    std::string line;
    std::string word;

    // skip header (first line)
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::vector<std::string> vec;
        sstr.str(line);

        while (std::getline(sstr, word, delim)) {
            // trim leading/trailing spaces and quotes
            std::string::size_type start = word.find_first_not_of(" \t\n\r\"");
            if (start == std::string::npos) start = 0;
            std::string::size_type end = word.find_last_not_of(" \t\n\r\"");
            if (end == std::string::npos) end = word.size() - 1;

            word = word.substr(start, end - start + 1);
            vec.push_back(word);
        }

        data.push_back(vec);
        sstr.clear();
    }

    file.close();
    return data;
}

// Comparators (no lambdas)
static bool employee_less(const Employee& a, const Employee& b) {
    return a.company_id < b.company_id; // consistent with your join logic
}

static bool company_less(const Company& a, const Company& b) {
    return a.id < b.id;
}

int main() {
    std::cout << "Size of Employee: " << sizeof(Employee) << '\n';
    std::cout << "Size of Company:  " << sizeof(Company) << '\n';

    try {
        Table data = readCSV("./files/employee.csv", ',');

        std::vector<Employee> employees;

        for (std::size_t i = 0; i < data.size(); ++i) {
            const std::vector<std::string>& row = data[i];
            if (row.size() < 4) continue;

            Employee employee; // zero-inited only if aggregate; fields set below
            employee.id = static_cast<int32_t>(std::stoi(row[0]));
            std::strcpy(&employee.fname[0], row[1].c_str());
            std::strcpy(&employee.lname[0], row[2].c_str());
            employee.company_id = static_cast<int32_t>(std::stoi(row[3]));

            employees.push_back(employee);
        }

        std::sort(employees.begin(), employees.end(), employee_less);

        std::ofstream file("./files/employee.bin", std::ios::binary);
        for (std::size_t i = 0; i < employees.size(); ++i) {
            const Employee& employee = employees[i];
            file.write(reinterpret_cast<const char*>(&employee), sizeof(Employee));
        }
        file.close();
        file.clear();

        // Read companies.csv
        data = readCSV("./files/company.csv", ';');

        std::vector<Company> companies;

        for (std::size_t i = 0; i < data.size(); ++i) {
            const std::vector<std::string>& row = data[i];
            if (row.size() < 3) continue;

            Company company;
            company.id = static_cast<int32_t>(std::stoi(row[0]));
            std::strcpy(&company.name[0], row[1].c_str());
            std::strcpy(&company.slogan[0], row[2].c_str());

            companies.push_back(company);
        }

        std::sort(companies.begin(), companies.end(), company_less);

        file.open("./files/company.bin", std::ios::binary);
        for (std::size_t i = 0; i < companies.size(); ++i) {
            const Company& company = companies[i];
            file.write(reinterpret_cast<const char*>(&company), sizeof(Company));
        }
        file.close();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
