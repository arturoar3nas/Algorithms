
#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline") //Optimization flags
#pragma GCC option("arch=native","tune=native","no-zero-upper") //Enable AVX
#pragma GCC target("avx2")  //Enable AVX
#include <x86intrin.h> //AVX/SSE Extensions
#include <bits/stdc++.h> //All main STD libraries

using namespace std;
using namespace std::chrono;

// -fopt-info-vec-optimized -fstack-usage

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



int main()
{

    const float n_elements {1000000};
    const float n_iterations {1000};
    
    {
        std::vector<float> v(n_elements);
    
        for(size_t i = 0; i < n_iterations; ++i)
        {
            for(size_t j = 0; j < n_elements; ++j)
            {
                v[j] = j;
            }
        }
    }
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

