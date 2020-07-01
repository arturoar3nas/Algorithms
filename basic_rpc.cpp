#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <functional>

std::unordered_map<std::string, std::any> myMap;

struct Foo
{
    static
    int bar(int x, float y, bool z)
    {
        std::cout << "bar: " << x << " " << y << " " << z << '\n';
        return 0;
    }
};

void func1()
{
    std::cout << "Function1 has been called.\n";
}

int func2(int a)
{
    std::cout << "func2 has been called, a = " << a << "\n";
    return a + 10;
}

template <typename T>
void attach(T const & f, std::string str)
{
  myMap[str] = f;
}

template <typename T, typename... Args >
void attach_(T const & f, std::string str, Args... args)
{
  myMap[str] = std::bind(f, std::forward<Args>(args)...);
}


void call(std::string s)
{
  auto mapIter = myMap.find(s);
  if(s.compare("func1") == 0)
    std::any_cast <void (*) ()> (mapIter->second) ();
  if(s.compare("func2") == 0)
    int result = std::any_cast <int (*) (int)> (mapIter->second) (5);
  if(s.compare("bar") == 0)
    std::any_cast <int (*) (int, float, bool)> (mapIter->second) (1, 2.0, true);
}



int main()
{  

    attach(func1, "func1");
    call("func1");

    attach(func2, "func2");
    call("func2");

    attach(Foo::bar, "bar");
    call("bar");

}
