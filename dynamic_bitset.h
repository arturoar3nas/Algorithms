#include <stdio.h>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <ostream>

template<int size>
class dynamic_bitset
{
    std::bitset<size> bset1;
public:
    dynamic_bitset()=default;
    
    void push_back(int b) {
        if(b >1 || b < 0) {
            throw std::runtime_error("error: bad value in push back function dynamic_bitset\n");    
        }
        
        bset1 <<= 1;
        bset1[0] = b;
    }
    
    //returns number of not set bits in bitset 
    int not_set_bits() const 
    {
        return bset1.size() - bset1.count();
    }
    
    //returns number of set bits in bitset 
    int set_bits() const
    {
        return bset1.count();
    }
    
    //any function returns true, if atleast 1 bit 
    bool none() const
    {
        return bset1.none();
    }
    
    void reset()
    {
        bset1.reset();
    }
};


int main()
{

    dynamic_bitset<8> bset;
    
    std::cout << "bset" << " has " << bset.set_bits() << " ones and "
     << bset.not_set_bits() << " zeros\n";
     
    //any function returns true, if atleast 1 bit 
    if (bset.none()) 
        std::cout << "bsbset has no bit set.\n";
     
    bset.push_back(2);
     
    std::cout << "bset" << " has " << bset.set_bits() << " ones and "
     << bset.not_set_bits() << " zeros\n";
    
}
