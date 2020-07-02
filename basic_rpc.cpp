#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <functional>
#include <map>

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

struct Baz
{
    void qux()
    {
        std::cout << "Function4 has been called.\n";
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

template <typename T1, typename T2>
void __attach(T1 const &f, T2 const &object, std::string str)
{
  // myMap[str] = std::make_any<T2>(object);
  myMap[str] = std::function<void()>(std::bind(f, object)); // std::bind returns an unspecified type then need a wrap like std::function
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
    
  if(s.compare("baz") == 0)
  {
    try
    { 
      // std::cout << typeid(mapIter->second).name() << '\n';
      // std::any_cast <Baz&> (mapIter->second).qux();
      std::any_cast<std::function<void()>>(mapIter->second)();
    }
    catch (const std::bad_any_cast& e) {
      std::cout << e.what() << ": tried to cast from "
      << (mapIter->second).type().name() << '\n';
    }
  }
}

void test_1()
{
    std::map<std::string, std::any> map;

    map["bar"] = std::function<void()>([]() { std::cout << "hello \n"; });
    std::any_cast<std::function<void()>>(map["bar"])();

    Baz baz;
    auto var__ = std::function<void()>(std::bind(&Baz::qux, baz));
    map["baz"] = var__;
    // std::cout << typeid(var__).name() << '\n';
    std::any_cast<std::function<void()>>(map["baz"])();
}

struct thud
{
    int a, b;

    thud(int x, int y) : a(x), b(y) { }

    void Print() { std::cout << a << ", " << b << "\n"; }
};

void test_2()
{
    std::any var = std::make_any<thud>(10, 10);
    try
    {
        std::any_cast<thud&>(var).Print();
        std::any_cast<thud&>(var).a = 11; // read/write
        std::any_cast<thud&>(var).Print();
        std::any_cast<int>(var); // throw!
    }
    catch(const std::bad_any_cast& e) 
    {
        std::cout << e.what() << '\n';
    }

    int* p = std::any_cast<int>(&var);
    std::cout << (p ? "contains int... \n" : "doesn't contain an int...\n");

    thud* pt = std::any_cast<thud>(&var);
    if (pt)
    {
        pt->a = 12;
        std::any_cast<thud&>(var).Print();
    }
}

int main()
{  

    attach(func1, "func1");
    call("func1");

    attach(func2, "func2");
    call("func2");

    attach(Foo::bar, "bar");
    call("bar");
    
    Baz baz;
    __attach(&Baz::qux, baz, "baz");
    call("baz");
    
    test_1();

    test_2();

}
