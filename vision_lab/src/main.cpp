#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <Halide.h>
#include <halide_image_io.h>

#include <VX/vx.h>
#include <VX/vx_compatibility.h>
//#include <vx_ext_opencv.h> <--------------------------- SIGSEGV!!!

using namespace Halide;

cv::Mat src; cv::Mat dst;
int MAX_KERNEL_LENGTH = 31;


void foo_halide()
{
    // This program defines a single-stage imaging pipeline that
    // outputs a grayscale diagonal gradient.
    
    // A 'Func' object represents a pipeline stage. It's a pure
    // function that defines what value each pixel should have. You
    // can think of it as a computed image.
    Halide::Func gradient;
    
    // Var objects are names to use as variables in the definition of
    // a Func. They have no meaning by themselves.
    Halide::Var x, y;
    
    // We typically use Vars named 'x' and 'y' to correspond to the x
    // and y axes of an image, and we write them in that order. If
    // you're used to thinking of images as having rows and columns,
    // then x is the column index, and y is the row index.
    
    // Funcs are defined at any integer coordinate of its variables as
    // an Expr in terms of those variables and other functions.
    // Here, we'll define an Expr which has the value x + y. Vars have
    // appropriate operator overloading so that expressions like
    // 'x + y' become 'Expr' objects.
    Halide::Expr e = x + y;
    
    // Now we'll add a definition for the Func object. At pixel x, y,
    // the image will have the value of the Expr e. On the left hand
    // side we have the Func we're defining and some Vars. On the right
    // hand side we have some Expr object that uses those same Vars.
    gradient(x, y) = e;
    
    // This is the same as writing:
    //
    //   gradient(x, y) = x + y;
    //
    // which is the more common form, but we are showing the
    // intermediate Expr here for completeness.
    
    // That line of code defined the Func, but it didn't actually
    // compute the output image yet. At this stage it's just Funcs,
    // Exprs, and Vars in memory, representing the structure of our
    // imaging pipeline. We're meta-programming. This C++ program is
    // constructing a Halide program in memory. Actually computing
    // pixel data comes next.
    
    // Now we 'realize' the Func, which JIT compiles some code that
    // implements the pipeline we've defined, and then runs it.  We
    // also need to tell Halide the domain over which to evaluate the
    // Func, which determines the range of x and y above, and the
    // resolution of the output image. Halide.h also provides a basic
    // templatized image type we can use. We'll make an 800 x 600
    // image.
    Halide::Buffer<int32_t> output = gradient.realize(800, 600);
    
    // Halide does type inference for you. Var objects represent
    // 32-bit integers, so the Expr object 'x + y' also represents a
    // 32-bit integer, and so 'gradient' defines a 32-bit image, and
    // so we got a 32-bit signed integer image out when we call
    // 'realize'. Halide types and type-casting rules are equivalent
    // to C.
    
    // Let's check everything worked, and we got the output we were
    // expecting:
    for (int j = 0; j < output.height(); j++) {
            for (int i = 0; i < output.width(); i++) {
                    // We can access a pixel of an Buffer object using similar
                    // syntax to defining and using functions.
                    if (output(i, j) != i + j) {
                            printf("Something went wrong!\n"
                                   "Pixel %d, %d was supposed to be %d, but instead it's %d\n",
                                   i, j, i + j, output(i, j));
                            return;
                        }
                }
        }
    
    // Everything worked! We defined a Func, then called 'realize' on
    // it to generate and run machine code that produced an Buffer.
    printf("Success!\n");
    
    return;
}



void gaussian_blur_halide()
{
    
    Var x("x"), y("y"), c("c");
    // Take a color 8-bit input
    Buffer<uint8_t> input = Halide::Tools::load_image("rsrc/rgb.png");
    
    int pixels = 10;
    
    // Upgrade it to 16-bit, so we can do math without it overflowing.
    Func input_16("input_16");
    input_16(x, y, c) = cast<uint16_t>(input(x, y, c));
    float sigma = 1.5f; 
    Func kernel;
    kernel(x) = exp(-x*x/(2*sigma*sigma)) / (sqrtf(2*M_PI)*sigma);
    
    // Blur it horizontally:
    Func blur_x("blur_x");
    blur_x(x, y, c) = (input_16(x-pixels, y, c) +
                       2 * input_16(x, y, c) +
                       input_16(x+pixels, y, c)) / 4;
    // Blur it vertically:
    Func blur_y("blur_y");
    blur_y(x, y, c) = (blur_x(x, y-pixels, c) +
                       2 * blur_x(x, y, c) +
                       blur_x(x, y+pixels, c)) / 4;
    
    // Convert back to 8-bit.
    Func output("output");
    output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));
    
    // Each Func in this pipeline calls a previous one using
    // familiar function call syntax (we've overloaded operator()
    // on Func objects). A Func may call any other Func that has
    // been given a definition. This restriction prevents
    // pipelines with loops in them. Halide pipelines are always
    // feed-forward graphs of Funcs.
    // Now let's realize it...
    
    //Buffer<uint8_t> result = output.realize(input.width(), input.height(), 3);
    
    // Except that the line above is not going to work. Uncomment
    // it to see what happens.
    // Realizing this pipeline over the same domain as the input
    // image requires reading pixels out of bounds in the input,
    // because the blur_x stage reaches outwards horizontally, and
    // the blur_y stage reaches outwards vertically. Halide
    // detects this by injecting a piece of code at the top of the
    // pipeline that computes the region over which the input will
    // be read. When it starts to run the pipeline it first runs
    // this code, determines that the input will be read out of
    // bounds, and refuses to continue. No actual bounds checks
    // occur in the inner loop; that would be slow.
    //
    // So what do we do? There are a few options. If we realize
    // over a domain shifted inwards by one pixel, we won't be
    // asking the Halide routine to read out of bounds. We saw how
    // to do this in the previous lesson:
    Halide::Buffer<uint8_t> result(input.width()-pixels*2, input.height()-pixels*2, 3);
    result.set_min(pixels, pixels);
    output.realize(result);
    
    //blur_x.realize(result);   
    
    //blur_y.realize(result);   
    
    // Save the result. It should look like a slightly blurry
    // parrot, and it should be two pixels narrower and two pixels
    // shorter than the input image.
    Halide::Tools::save_image(result, "blurry_parrot_1.png");
    
    // This is usually the fastest way to deal with boundaries:
    // don't write code that reads out of bounds :) The more
    // general solution is our next example.
    
    
    std::cout << "Done Halide blur\n";
}


void gaussian_blur_opencv()
{
    src = cv::imread( "rsrc/lena.jpg", cv::IMREAD_COLOR );
    if (src.empty()) {
            std::cout <<" Error opening image\n";
            return;
        }
    
    cv::imwrite("original_image.png", src);
    
    dst = src.clone();
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 ) {
            cv::blur( src, dst, cv::Size( i, i ), cv::Point(-1,-1) );
        }
    
    cv::imwrite("blur_image.png", dst);
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 ) {
            cv::GaussianBlur( src, dst, cv::Size( i, i ), 0, 0 );
        }
    
    cv::imwrite("gaussian_blur_image.png", dst);
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
        {
            cv::medianBlur ( src, dst, i );
        }
    
    cv::imwrite("median_blur_image.png", dst);
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
        {
            cv::bilateralFilter ( src, dst, i, i*2, i/2 );
        }
    
    cv::imwrite("bilateral_filter_image.png", dst);
    
    std::cout <<  "Done!\n";
    
    return;
}


int main( int argc, const char * argv[] )
{
    //if(cv::haveOpenVX())
    //	std::cout << "have openvx!\n";	  
    std::cout << "Vision Lab" << std::endl;
    //gaussian_blur_opencv();
    //gaussian_blur_halide();
    
    int width = 480, height = 360;
    vx_context context = vxCreateContext();
    vxReleaseContext( &context );	
    return 0;
}

