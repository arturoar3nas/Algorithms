#include <stdio.h>
#include <iostream>
#include <memory>

using namespace std;

class C {
    public:
    C() : n(0) {};
    ~C(){cout << "~destructor" << endl;}
    int n;
};

void double_free_error(){
    C* a = new C();
    C* b = new C();
    
    a->n = 1;
    
    b = a;
    
    cout << a->n << endl;
    cout << a << endl;
    cout << b << endl;
    
    delete a;
    delete b;
}

void smart_pointer(){
    shared_ptr<C> a = make_shared<C>();
    shared_ptr<C> b = make_shared<C>();
    weak_ptr<C> w; //weak reference
    
    a->n = 1;
    
    //change ownership
    b = a; 
    
    // copy ref
    // w = a;
    
    // Has to be copied into a shared_ptr before usage
    auto sp = w.lock();
    auto sp_cpy = w.lock();
    
    cout << a->n << endl;
    cout << b->n << endl;
    
    if(sp)
        cout << sp->n << endl;
    
    if(sp_cpy)
        cout << sp_cpy->n << endl;
    
    cout << a << endl;
    cout << b << endl;
    if(sp)
        cout << sp << endl;
    
    if(sp_cpy)
        cout << sp_cpy << endl;
}

int main()
{
    smart_pointer();
    // double_free_error();

    return 0;
}
