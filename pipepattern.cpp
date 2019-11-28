#include <iostream>
#include <functional>

using namespace std;

template <class F>
struct pipeable
{
private:
    F f;
public:
    pipeable(F&& f) : f(std::forward<F>(f)) {}
     
    template<class... Xs>
    auto operator()(Xs&&... xs) -> decltype(std::bind(f, std::placeholders::_1, std::forward<Xs>(xs)...)) const {
        return std::bind(f, std::placeholders::_1, std::forward<Xs>(xs)...);
    }
   
};
    
template <class F>
pipeable<F> piped(F&& f) { return pipeable<F>{std::forward<F>(f)}; }
    
template<class T, class F>
auto operator|(T&& x, const F& f) -> decltype(f(std::forward<T>(x)))
{
    return f(std::forward<T>(x));
}

int main()
{
    auto add = piped([](int x, int y){ cout << "x: " << to_string(x) << " y: " << to_string(y) << endl; return x + y; });
    auto mul = piped([](int x, int y){ return x * y; });
    int y = 5 | add(2) | mul(5) | add(1); // Output: 36
    cout<<"output: " << to_string(y);

    return 0;
}
