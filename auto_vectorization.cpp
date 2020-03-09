
#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline") //Optimization flags
#pragma GCC option("arch=skylake-avx512","tune=skylake-avx512","no-zero-upper") //Enable AVX
#pragma GCC target("avx2")  //Enable AVX
#include <x86intrin.h> //AVX/SSE Extensions
#include <bits/stdc++.h> //All main STD libraries
// #include <tbb/tbb.h>
#include <tbb/parallel_for.h>

using namespace std;
using namespace std::chrono;

// -fopt-info-vec-optimized -fopt-info-vec-missed -fstack-usage -fsave-optimization-record 
// 'foo', 'bar', 'baz', 'qux', 'quux', 'corge', 'grault', 'garply', 'waldo', 'fred', 'plugh', 'xyzzy', 'thud'

// int ga[256], gb[256], gc[256];
// void foo () {
//   int i;
//   for (i=0; i<256; i++){
//     ga[i] = gb[i] + gc[i];
//   }
// }

// void bar (int n, int x) {
//    int i;

//    /* feature: support for unknown loop bound  */
//    /* feature: support for loop invariants  */
//    for (i=0; i<n; i++) {
//       gb[i] = x;
//    }

//    /* feature: general loop exit condition  */
//    /* feature: support for bitwise operations  */
//    while (n--){
//       ga[i] = gb[i]&gc[i]; i++;
//    }
// }

// typedef int aint __attribute__ ((__aligned__(16)));
// void baz (int n, aint * __restrict__ p, aint * __restrict q) {

//    /* feature: support for (aligned) pointer accesses.  */
//    while (n--){
//       *p++ = *q++ +5;
//    }
// }

// typedef int aint __attribute__ ((__aligned__(16)));
// int a[256], b[256], c[256];
// void qux (int n, aint * __restrict__ p, aint * __restrict__ q) {
//    int i;
//    int j;
//    int MAX = 10;

//    /* feature: support for (aligned) pointer accesses  */
//    /* feature: support for constants  */
//    while (n--){
//       *p++ = *q++ + 5;
//    }

//    /* feature: support for read accesses with a compile time known misalignment  */
//    for (i=0; i<n; i++){
//       a[i] = b[i+1] + c[i+3];
//    }

//    /* feature: support for if-conversion  */
//    for (i=0; i<n; i++){
//       j = a[i];
//       b[i] = (j > MAX ? MAX : 0);
//    }
// }

// void quux ()
// {
//     const float n_elements {1000000};
//     std::vector<float> v(n_elements);
//     for (std::vector<float>::iterator it = v.begin(); it != v.end(); it++)
//     {
//         (*it) += 23;
//     }   
// }

// void corge () noexcept
// {
//     const float n_elements {1000000};
//     const float n_iterations {1000};

//     std::vector<float> v(n_elements);
//     std::vector<float> elem_1(n_elements);
//     std::vector<float> elem_2(n_elements);

//     for(size_t i = 0; i < n_elements; ++i)
//     {
//         v[i] = elem_1[i] + elem_2[i]; 
//     }
// }

void garply () noexcept
{
    auto __attribute__((aligned(32))) vec_a = std::vector<double>(10000) ;
    auto __attribute__((aligned(32))) vec_b = std::vector<double>(10000);
    auto __attribute__((aligned(32))) vec_c = std::vector<double>(10000);
    auto __attribute__((aligned(32))) vec_d = std::vector<double>(10000);
    auto __attribute__((aligned(32))) vec_f = std::vector<double>(10000);
    auto __attribute__((aligned(32))) vec_g = std::vector<double>(10000);
    auto __attribute__((aligned(32))) vec_h = std::vector<double>(10000);

    tbb::parallel_for( tbb::blocked_range<int>(0,vec_a.size()),
                    [&](tbb::blocked_range<int> r)
    {
        
        auto __attribute__((aligned(32))) vec_a_it = vec_a.begin();
        auto __attribute__((aligned(32))) vec_b_it = vec_b.begin();
        auto __attribute__((aligned(32))) vec_c_it = vec_c.begin();
        auto __attribute__((aligned(32))) it = vec_d.begin();
        auto __attribute__((aligned(32))) vec_f_it = vec_f.begin();
        auto __attribute__((aligned(32))) vec_g_it = vec_g.begin();
        auto __attribute__((aligned(32))) vec_h_it = vec_h.begin();

        for (int i=r.begin(); i<r.end(); ++i, vec_a_it++,
                                                vec_b_it++,
                                                vec_c_it++,
                                                it++,
                                                vec_f_it++,
                                                vec_g_it++,
                                                vec_h_it++)
        {
            

            (*vec_f_it) = (*vec_a_it)-(*vec_b_it);
            if (((*vec_f_it) > 0) && ((*vec_a_it) > double{1.0d})){
                (*vec_h_it) = (((*vec_f_it) / (*vec_c_it)) > double{1.0d})? ( ((*vec_g_it)>0)? static_cast<double>(0xFF) : double{0.0d}) : double{0.0d};
                // (*it) = ((*it) < 0.0d) ? 0.0d : (*it);
            }
            // else {
                // (*it) = 0;
                // (*vec_h_it) = 0;
            // }
        }
    });
}

// void grault() noexcept
// {
//     const float n_elements {1000000};
//     const float n_iterations {1000};

//     std::vector<float> v(n_elements);
//     std::vector<float> elem_1(n_elements);
//     std::vector<float> elem_2(n_elements);

//     //for(auto i = 0; i < n_iterations; ++i) {
//         auto it_el_1 = elem_1.begin();
//         auto it_el_2 = elem_2.begin();
//         for(auto it = v.begin(); it != v.end(); ++it)
//         {
//             (*it) = (*it_el_1) + (*it_el_2);
//             (*it) = (*it_el_1) - (*it_el_2); 
//             (*it) = (*it_el_1) * (*it_el_2); 
//             (*it) = (*it_el_1) / (*it_el_2);  
//         }
//     //}
// }


int main()
{

    // const float n_elements {1000000};
    // const float n_iterations {1000};


    
    // {
    //     std::vector<float> v(n_elements);
    
    //     for(size_t i = 0; i < n_iterations; ++i)
    //     {
    //         for(size_t j = 0; j < n_elements; ++j)
    //         {
    //             v[j] = j;
    //         }
    //     }
    // }
    // {
    //     std::vector<float> v(n_elements);
    //     std::vector<float> elem_1(n_elements);
    //     std::vector<float> elem_2(n_elements);
    
    //     for(size_t i = 0; i < n_iterations; ++i)
    //     {
    //         for(size_t j = 0; j < n_elements; ++j)
    //         {
    //             v[j] = elem_1[j] / elem_2[j];
    //         }
    //     }
    // }

    // {
    //     std::vector<float> v(n_elements);
    //     for (std::vector<float>::iterator it = v.begin(); it != v.end(); it++)
    //     {
    //         (*it) += 23;
    //     }
    // }

	// const int N = 200000; //Array Size
	// const int noTests = 10000; //Number of tests
	// float a[N],b[N],c[N],result[N];
    // auto now = high_resolution_clock::now();
    // for (int i = 0; i < N; ++i)  //Data initialization	
	// { 
    //   a[i] =       ((float)i)+ 0.1335f; 
	//   b[i] = 1.50f*((float)i)+ 0.9383f; 
	//   c[i] = 0.33f*((float)i)+ 0.1172f; 
    // }
     
//    for (int i = 0; i < noTests; ++i) 
// 	 for (int j = 0; j < N; ++j)
// 	 {
// 	  result[j] = a[j]+b[j]-c[j]+3*(float)i;
// 	 }
	//auto end_time = duration_cast<duration<double>>(high_resolution_clock::now() - now).count();
	//assert( result[2] == ( 2.0f + 0.1335f)+( 1.50f*2.0f + 0.9383f)-(0.33f*2.0f+0.1172f)+3*(float)(noTests-1));
    //cout << "CG> message -channel \"exercise results\" Time used: "<< end_time << "s, N * noTests="<<(N*noTests)<< endl;
    return 0;
}

