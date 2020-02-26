#include <string.h>
#include <utility>
#include <iostream>
#include <vector>

class Baz { // GOOD: polymorphic class suppresses copying
public:
    Baz()=default;
    Baz(const Baz&) = delete;
    Baz& operator=(const Baz&) = delete;
    virtual char get() { return 'B'; }
    // ...
};

class Bar : public Baz {
public:
    char get() override { return 'D'; }
    // ...
};

void f(Baz& b) {
    // auto b2 = b; // ok, compiler will detect inadvertent copying, and protest
}


class FooBase
{
 public:
    FooBase()=default;
    FooBase(const FooBase&) = default;
    FooBase(FooBase&&) = default;
    FooBase& operator=(const FooBase&) = default;
    FooBase& operator=(FooBase&&) = default;
    virtual ~FooBase()=default;
};

class Foo : public FooBase
{
    int n;
    char* str;
public:
    Foo()
    {
        str = new char[1000000];
    }

    ~Foo()
    {
       std::cout << "~Foo" << std::endl;
       delete [] str;
    }

    Foo(const Foo& other) // copy constructor
    {
        strncpy(str, other.str, 100);
    }
 
    Foo(Foo&& other) noexcept // move constructor
    : str(std::exchange(other.str, nullptr))
    {

    }
 
    Foo& operator=(const Foo& other) // copy assignment
    {
         return *this = Foo(other);
    }
 
    Foo& operator=(Foo&& other) noexcept // move assignment
    {
        std::swap(str, other.str);
        return *this;
    }
 
};


int main() {

    Bar xbar;
    f(xbar);
    std::cout << xbar.get() << std::endl;

    Baz xbaz;
    std::cout << xbaz.get() << std::endl;

    Foo* x = new Foo();

    delete x;
    
    return 0;
}
