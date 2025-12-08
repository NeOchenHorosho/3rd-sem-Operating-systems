#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
using std::cerr;
using std::cin;
using std::cout;
using std::string;
using std::fstream;

const string request_fifo_prefix = "/request_fifo_";
const string response_fifo_prefix = "/response_fifo_";
struct Employee
{
    int num;
    char name[10];
    double hours;
};

struct Response_package
{
    bool is_valid;
    Employee employee;
};

struct Request_package
{
    bool is_write;
    Employee employee;
};


bool is_valid_int(const int val, const int min, const int max) noexcept
{
    if (val < min || val > max)
        return false;
    return true;
}