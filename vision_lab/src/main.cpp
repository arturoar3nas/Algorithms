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

////////
// Useful macros for OpenVX error checking:
//   ERROR_CHECK_STATUS     - check status is VX_SUCCESS
//   ERROR_CHECK_OBJECT     - check if the object creation is successful
#define ERROR_CHECK_STATUS( status ) { \
        vx_status status_ = (status); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            exit(1); \
        } \
    }

#define ERROR_CHECK_OBJECT( obj ) { \
        vx_status status_ = vxGetStatus((vx_reference)(obj)); \
        if(status_ != VX_SUCCESS) { \
            printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
            exit(1); \
        } \
    }

#ifndef VX_VERSION_1_1
const vx_enum VX_IMAGE_FORMAT = VX_IMAGE_ATTRIBUTE_FORMAT;
const vx_enum VX_IMAGE_WIDTH  = VX_IMAGE_ATTRIBUTE_WIDTH;
const vx_enum VX_IMAGE_HEIGHT = VX_IMAGE_ATTRIBUTE_HEIGHT;
const vx_enum VX_MEMORY_TYPE_HOST = VX_IMPORT_TYPE_HOST;
const vx_enum VX_MEMORY_TYPE_NONE = VX_IMPORT_TYPE_NONE;
const vx_enum VX_THRESHOLD_THRESHOLD_VALUE = VX_THRESHOLD_ATTRIBUTE_THRESHOLD_VALUE;
const vx_enum VX_THRESHOLD_THRESHOLD_LOWER = VX_THRESHOLD_ATTRIBUTE_THRESHOLD_LOWER;
const vx_enum VX_THRESHOLD_THRESHOLD_UPPER = VX_THRESHOLD_ATTRIBUTE_THRESHOLD_UPPER;
typedef uintptr_t vx_map_id;
#endif

enum user_library_e
{
    USER_LIBRARY_EXAMPLE        = 1,
};

enum user_kernel_e
{
    USER_KERNEL_MEDIAN_BLUR     = VX_KERNEL_BASE( VX_ID_DEFAULT, USER_LIBRARY_EXAMPLE ) + 0x001,
};

////////
// log_callback function implements a mechanism to print log messages
// from the OpenVX framework onto console.
void VX_CALLBACK log_callback( vx_context    context,
                   vx_reference  ref,
                   vx_status     status,
                   const vx_char string[] )
{
    printf( "LOG: [ status = %d ] %s\n", status, string );
    fflush( stdout );
}


cv::Mat copyVxImageToCvMat(vx_image ovxImage)
{
    vx_status status;
    vx_df_image df_image = 0;
    vx_uint32 width, height;
    status = vxQueryImage(ovxImage, VX_IMAGE_FORMAT, &df_image, sizeof(vx_df_image));
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to query image");
    status = vxQueryImage(ovxImage, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to query image");
    status = vxQueryImage(ovxImage, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to query image");

    if (!(width > 0 && height > 0)) throw std::runtime_error("Invalid format");

    int depth;
    switch (df_image)
    {
    case VX_DF_IMAGE_U8:
        depth = CV_8U;
        break;
    case VX_DF_IMAGE_U16:
        depth = CV_16U;
        break;
    case VX_DF_IMAGE_S16:
        depth = CV_16S;
        break;
    case VX_DF_IMAGE_S32:
        depth = CV_32S;
        break;
    default:
        throw std::runtime_error("Invalid format");
        break;
    }

    cv::Mat image(height, width, CV_MAKE_TYPE(depth, 1));

    vx_rectangle_t rect;
    rect.start_x = rect.start_y = 0;
    rect.end_x = width; rect.end_y = height;

    vx_imagepatch_addressing_t addr;
    addr.dim_x = width;
    addr.dim_y = height;
    addr.stride_x = (vx_uint32)image.elemSize();
    addr.stride_y = (vx_uint32)image.step.p[0];
    vx_uint8* matData = image.data;

#ifdef VX_VERSION_1_1
    status = vxCopyImagePatch(ovxImage, &rect, 0, &addr, matData, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to copy image patch");
#else
    status = vxAccessImagePatch(ovxImage, &rect, 0, &addr, (void**)&matData, VX_READ_ONLY);
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to access image patch");
    status = vxCommitImagePatch(ovxImage, &rect, 0, &addr, matData);
    if (status != VX_SUCCESS)
        throw std::runtime_error("Failed to commit image patch");
#endif

    return image;
}

vx_image convertCvMatToVxImage(vx_context context, cv::Mat image, bool toCopy)
{
    if (!(!image.empty() && image.dims <= 2 && image.channels() == 1))
        throw std::runtime_error("Invalid format");

    vx_uint32 width  = image.cols;
    vx_uint32 height = image.rows;

    vx_df_image color;
    switch (image.depth())
    {
    case CV_8U:
        color = VX_DF_IMAGE_U8;
        break;
    case CV_16U:
        color = VX_DF_IMAGE_U16;
        break;
    case CV_16S:
        color = VX_DF_IMAGE_S16;
        break;
    case CV_32S:
        color = VX_DF_IMAGE_S32;
        break;
    default:
        throw std::runtime_error("Invalid format");
        break;
    }

    vx_imagepatch_addressing_t addr;
    addr.dim_x = width;
    addr.dim_y = height;
    addr.stride_x = (vx_uint32)image.elemSize();
    addr.stride_y = (vx_uint32)image.step.p[0];
    vx_uint8* ovxData = image.data;

    vx_image ovxImage;
    if (toCopy)
    {
        ovxImage = vxCreateImage(context, width, height, color);
        if (vxGetStatus((vx_reference)ovxImage) != VX_SUCCESS)
            throw std::runtime_error("Failed to create image");
        vx_rectangle_t rect;

        vx_status status = vxGetValidRegionImage(ovxImage, &rect);
        if (status != VX_SUCCESS)
            throw std::runtime_error("Failed to get valid region");

#ifdef VX_VERSION_1_1
        status = vxCopyImagePatch(ovxImage, &rect, 0, &addr, ovxData, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        if (status != VX_SUCCESS)
            throw std::runtime_error("Failed to copy image patch");
#else
        status = vxAccessImagePatch(ovxImage, &rect, 0, &addr, (void**)&ovxData, VX_WRITE_ONLY);
        if (status != VX_SUCCESS)
            throw std::runtime_error("Failed to access image patch");
        status = vxCommitImagePatch(ovxImage, &rect, 0, &addr, ovxData);
        if (status != VX_SUCCESS)
            throw std::runtime_error("Failed to commit image patch");
#endif
    }
    else
    {
        ovxImage = vxCreateImageFromHandle(context, color, &addr, (void**)&ovxData, VX_MEMORY_TYPE_HOST);
        if (vxGetStatus((vx_reference)ovxImage) != VX_SUCCESS)
            throw std::runtime_error("Failed to create image from handle");
    }

    return ovxImage;
}


vx_status VX_CALLBACK median_blur_validator( vx_node node,
                                             const vx_reference parameters[], vx_uint32 num,
                                             vx_meta_format metas[] )
{
    // parameter #0 -- input image of format VX_DF_IMAGE_U8
    vx_df_image format = VX_DF_IMAGE_VIRT;
    ERROR_CHECK_STATUS( vxQueryImage( ( vx_image )parameters[0], VX_IMAGE_FORMAT, &format, sizeof( format ) ) );
    if( format != VX_DF_IMAGE_U8 )
    {
        return VX_ERROR_INVALID_FORMAT;
    }

    // parameter #2 -- scalar of type VX_TYPE_INT32
    vx_enum type = VX_TYPE_INVALID;
    ERROR_CHECK_STATUS( vxQueryScalar( ( vx_scalar )parameters[2], VX_SCALAR_TYPE, &type, sizeof( type ) ) );
    if( type != VX_TYPE_INT32 )
    {
        return VX_ERROR_INVALID_TYPE;
    }

    // parameter #1 -- output image should be VX_DF_FORMAT_U8 and should have same dimensions as input image
    vx_uint32 width = 0, height = 0;
    ERROR_CHECK_STATUS( vxQueryImage( ( vx_image )parameters[0], VX_IMAGE_WIDTH,  &width,  sizeof( width ) ) );
    ERROR_CHECK_STATUS( vxQueryImage( ( vx_image )parameters[0], VX_IMAGE_HEIGHT, &height, sizeof( height ) ) );
    format = VX_DF_IMAGE_U8;
    ERROR_CHECK_STATUS( vxSetMetaFormatAttribute( metas[1], VX_IMAGE_FORMAT, &format, sizeof( format ) ) );
    ERROR_CHECK_STATUS( vxSetMetaFormatAttribute( metas[1], VX_IMAGE_WIDTH,  &width,  sizeof( width ) ) );
    ERROR_CHECK_STATUS( vxSetMetaFormatAttribute( metas[1], VX_IMAGE_HEIGHT, &height, sizeof( height ) ) );

    return VX_SUCCESS;
}

vx_status VX_CALLBACK median_blur_host_side_function( vx_node node, const vx_reference * refs, vx_uint32 num )
{
    vx_image  input   = ( vx_image ) refs[0];
    vx_image  output  = ( vx_image ) refs[1];
    vx_scalar s_ksize = ( vx_scalar )refs[2];

    vx_int32  ksize   = 0;
    ERROR_CHECK_STATUS( vxCopyScalar( s_ksize, &ksize, VX_READ_ONLY, VX_MEMORY_TYPE_HOST ) );

    vx_uint32 width = 0, height = 0;
    ERROR_CHECK_STATUS( vxQueryImage( input, VX_IMAGE_WIDTH,  &width,  sizeof( width ) ) );
    ERROR_CHECK_STATUS( vxQueryImage( input, VX_IMAGE_HEIGHT, &height, sizeof( height ) ) );

    vx_rectangle_t rect = { 0, 0, width, height };
    vx_map_id map_input, map_output;
    vx_imagepatch_addressing_t addr_input, addr_output;
    void * ptr_input, * ptr_output;
    ERROR_CHECK_STATUS( vxMapImagePatch( input,  &rect, 0, &map_input, &addr_input,  &ptr_input,  VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X ) );
    ERROR_CHECK_STATUS( vxMapImagePatch( output, &rect, 0, &map_output, &addr_output, &ptr_output, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X ) );

    cv::Mat mat_input(  height, width, CV_8U, ptr_input,  addr_input .stride_y );
    cv::Mat mat_output( height, width, CV_8U, ptr_output, addr_output.stride_y );
    cv::medianBlur( mat_input, mat_output, ksize );

    ERROR_CHECK_STATUS( vxUnmapImagePatch( input,  map_input ) );
    ERROR_CHECK_STATUS( vxUnmapImagePatch( output, map_output ) );

    return VX_SUCCESS;
}

vx_status registerUserKernel( vx_context context )
{
    vx_kernel kernel = vxAddUserKernel( context,
                                    "app.userkernels.median_blur",
                                    USER_KERNEL_MEDIAN_BLUR,
                                    median_blur_host_side_function,
                                    3,   // numParams
                                    median_blur_validator,
                                    NULL,
                                    NULL );
    ERROR_CHECK_OBJECT( kernel );

    ERROR_CHECK_STATUS( vxAddParameterToKernel( kernel, 0, VX_INPUT,  VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED ) ); // input
    ERROR_CHECK_STATUS( vxAddParameterToKernel( kernel, 1, VX_OUTPUT, VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED ) ); // output
    ERROR_CHECK_STATUS( vxAddParameterToKernel( kernel, 2, VX_INPUT,  VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED ) ); // ksize
    ERROR_CHECK_STATUS( vxFinalizeKernel( kernel ) );
    ERROR_CHECK_STATUS( vxReleaseKernel( &kernel ) );

    vxAddLogEntry( ( vx_reference ) context, VX_SUCCESS, "OK: registered user kernel app.userkernels.median_blur\n" );
    return VX_SUCCESS;
}

vx_node userMedianBlurNode( vx_graph graph,
                            vx_image input,
                            vx_image output,
                            vx_int32 ksize )
{
    vx_context context = vxGetContext( ( vx_reference ) graph );
    vx_kernel kernel   = vxGetKernelByEnum( context, USER_KERNEL_MEDIAN_BLUR );
    ERROR_CHECK_OBJECT( kernel );
    vx_node node       = vxCreateGenericNode( graph, kernel );
    ERROR_CHECK_OBJECT( node );

    vx_scalar s_ksize  = vxCreateScalar( context, VX_TYPE_INT32, &ksize );
    ERROR_CHECK_OBJECT( s_ksize );

    ERROR_CHECK_STATUS( vxSetParameterByIndex( node, 0, ( vx_reference ) input ) );
    ERROR_CHECK_STATUS( vxSetParameterByIndex( node, 1, ( vx_reference ) output ) );
    ERROR_CHECK_STATUS( vxSetParameterByIndex( node, 2, ( vx_reference ) s_ksize ) );

    ERROR_CHECK_STATUS( vxReleaseScalar( &s_ksize ) );
    ERROR_CHECK_STATUS( vxReleaseKernel( &kernel ) );

    return node;
}

void gaussian_blur_openvx()
{

    src = cv::imread( "rsrc/rgb.png",  cv::IMREAD_COLOR  );
    if (src.empty()) {
	std::cout <<" Error opening image\n";
        return;
    }

    vx_uint32 width = src.cols, height = src.rows;
    vx_int32   ksize     = 7; // - median filter kernel size
    vx_context context = vxCreateContext();

    vx_image input_rgb_image  = vxCreateImage( context, width, height, VX_DF_IMAGE_RGB );
    ERROR_CHECK_OBJECT( input_rgb_image );
    
    //input_rgb_image = convertCvMatToVxImage(context, src, true);

    // for receive message from openvx
    vxRegisterLogCallback( context, log_callback, vx_false_e );

    // Register user kernel with context by calling your implementation of "registerUserKernel()".
    ERROR_CHECK_STATUS( registerUserKernel( context ) );

    vx_image output_filtered_image = vxCreateImage( context, width, height, VX_DF_IMAGE_U8 );
	
    // Create graph object and intermediate image objects.
    // Given that input is an RGB image, it is best to extract a gray image
    // from RGB image, which requires two steps:
    //   - perform RGB to IYUV color conversion
    //   - extract Y channel from IYUV image
    // This requires two intermediate OpenVX image objects. Since you don't
    // need to access these objects from the application, they can be virtual
    // objects that can be created using the vxCreateVirtualImage API.
    
	    
    vx_graph graph      = vxCreateGraph( context );

    vx_image yuv_image  = vxCreateVirtualImage( graph, width, height, VX_DF_IMAGE_IYUV );
    vx_image luma_image = vxCreateVirtualImage( graph, width, height, VX_DF_IMAGE_U8 );

    ERROR_CHECK_OBJECT( yuv_image );
    ERROR_CHECK_OBJECT( luma_image );

    ERROR_CHECK_OBJECT( output_filtered_image );

    vx_node nodes[] =
    {
        vxColorConvertNode(   graph, input_rgb_image, yuv_image ),
        vxChannelExtractNode( graph, yuv_image, VX_CHANNEL_Y, luma_image ),
        userMedianBlurNode(   graph, luma_image, output_filtered_image, ksize )
    };
    for( vx_size i = 0; i < sizeof( nodes ) / sizeof( nodes[0] ); i++ )
    {
        ERROR_CHECK_OBJECT( nodes[i] );
        //ERROR_CHECK_STATUS( vxReleaseNode( &nodes[i] ) );
    }

    // Copy input RGB frame from OpenCV to OpenVX. In order to do this,
    // you need to use vxCopyImagePatch API.
    // See "VX/vx_api.h" for the description of these APIs.
    
    vx_rectangle_t cv_rgb_image_region;
    cv_rgb_image_region.start_x    = 0;
    cv_rgb_image_region.start_y    = 0;
    cv_rgb_image_region.end_x      = width;
    cv_rgb_image_region.end_y      = height;
    vx_imagepatch_addressing_t cv_rgb_image_layout;
    cv_rgb_image_layout.stride_x   = 3;
    cv_rgb_image_layout.stride_y   = src.step.p[0];
    vx_uint8 * cv_rgb_image_buffer = src.data;
    ERROR_CHECK_STATUS( vxCopyImagePatch( input_rgb_image, &cv_rgb_image_region, 0,
                                          &cv_rgb_image_layout, cv_rgb_image_buffer,
                                          VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST ) );
   
    // Now that input RGB image is ready, just run the graph.
    ERROR_CHECK_STATUS( vxProcessGraph( graph ) );
        
    // Display the output filtered image.
    vx_rectangle_t rect = { 0, 0, width, height };
    vx_map_id map_id;
    vx_imagepatch_addressing_t addr;
    void * ptr;
    ERROR_CHECK_STATUS( vxMapImagePatch( output_filtered_image, &rect, 0, &map_id, &addr, &ptr,
                                             VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X ) );
    cv::Mat mat( height, width, CV_8U, ptr, addr.stride_y );

    cv::imwrite( "MedianBlur_OpenVX.png", mat );
    //cv::imwrite("Output_OpenVX.png", copyVxImageToCvMat(output_filtered_image));
    //cv::imwrite("Input_OpenVX.png", copyVxImageToCvMat(input_rgb_image));

    ERROR_CHECK_STATUS( vxUnmapImagePatch( output_filtered_image, map_id ) );

    vxReleaseImage( &luma_image ); 
    vxReleaseImage( &yuv_image );
    vxReleaseImage( &input_rgb_image );
    vxReleaseImage( &output_filtered_image );
    vxReleaseContext( &context );
    vxReleaseGraph( &graph );
}


int main( int argc, const char * argv[] )
{
    std::cout << "Vision Lab" << std::endl;
    //gaussian_blur_opencv();
    //gaussian_blur_halide();
    gaussian_blur_openvx();	
    std::cout << "Done!" << std::endl;
    return 0;    
}

