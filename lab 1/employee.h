#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>


struct employee
{
    int num;
    char name[10];
    double hours;

    bool write(std::ofstream& out)
    {
        if(out.is_open())
        {
            out.write(reinterpret_cast<const char*>(&num), sizeof(int));
            out.write(name, 10*sizeof(char));
            out.write(reinterpret_cast<const char*>(&hours), sizeof(double));
            return true;
        }
        else
        {
            return false;
        }
    }
    bool read(std::ifstream& in)
    {
        if(!in.is_open()) return false;
        in.read(reinterpret_cast<char*>(&num), sizeof(int));
        in.read(name,10*sizeof(char));
        in.read(reinterpret_cast<char*>(&hours), sizeof(double));
        return in.good();
    }

    bool operator<(employee& r)
    {
        return this->num < r.num;
    }
};
