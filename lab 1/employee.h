#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>


struct employee
{
    int num;
    char name[10];
    double hours;

    bool write(std::string filename)
    {
        std::ofstream out(filename,std::ios::binary);
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

    bool read(std::string filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if(in.is_open())
        {
            in.read(reinterpret_cast<char*>(&num), sizeof(int));
            in.read(name,10*sizeof(char));
            in.read(reinterpret_cast<char*>(&hours), sizeof(double));
            return true;
        }
        else
        {
            return false;
        }
    }
};
