#ifndef _TABLES_HPP_
#define _TABLES_HPP_

#include <array>
#include <cstdint>

struct Employee
{
    int32_t id;
    int32_t company_id;
    std::array<char, 60> fname;
    std::array<char, 60> lname;

    // Equality: keep original behavior (compares company_id only)
    friend bool operator==(const Employee& lhs, const Employee& rhs)
    {
        return lhs.company_id == rhs.company_id;
    }
};

struct Company
{
    int32_t id;
    std::array<char, 62> name;
    std::array<char, 62> slogan;

    // Equality: keep original behavior (compares id only)
    friend bool operator==(const Company& lhs, const Company& rhs)
    {
        return lhs.id == rhs.id;
    }
};

// Sizes (use const instead of constexpr for old-school style)
const int EMPLOYEE_SIZE = (int)sizeof(Employee);
const int COMPANY_SIZE  = (int)sizeof(Company);

#endif // _TABLES_HPP_
