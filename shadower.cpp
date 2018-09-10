//
// shadower
//
// Add shadows to a png image by marching a shadow mask through the image
//
// (c)2018 Mark J Stock
//

#include "lodepng.h"
#include <iostream>
#include <cmath>
#include <cstring>

//
// basic usage
//
static void usage() {
  std::cerr << "Usage: shadower [-sc=<shade coeff>] [-sw=<shadow width>] [-ss=<shadow shift>] [-nb=<bins>] [-o=<outfile>] in.png" << std::endl;
  exit(1);
}

//
// entry and exit
//
int main(int argc, char *argv[])
{
  char in_file[255] = "in.png";
  char out_file[255] = "out.png";

  // how dark should the shadow be? 1.0 = perfect dark, 0.0 = not there
  float shade_coeff = 0.8;

  // factors on r,g,b to determine height
  float scale_r = -0.3;
  float scale_g = -0.6;
  float scale_b = -0.1;

  // shadow cone growth from top to bottom, normalized by image width
  float shadow_width = 1.0;

  // shadow cone downward shift from top to bottom, normalized by image height
  float shadow_shift = 0.03;

  // number of discrete layers in input image
  unsigned int hgt_bins = 16;

  // parse the command-line arguments
  for (int iarg=1; iarg<argc; ++iarg) {
    if (strncmp(argv[iarg], "-sc=", 4) == 0) {
      float new_val = atof(argv[iarg] + 4);
      shade_coeff = new_val;
    } else if (strncmp(argv[iarg], "-sw=", 4) == 0) {
      float new_val = atof(argv[iarg] + 4);
      if (new_val <= 0.f) usage();
      shadow_width = new_val;
    } else if (strncmp(argv[iarg], "-ss=", 4) == 0) {
      float new_val = atof(argv[iarg] + 4);
      if (new_val <= 0.f) usage();
      shadow_shift = new_val;
    } else if (strncmp(argv[iarg], "-nb=", 4) == 0) {
      int new_val = atoi(argv[iarg] + 4);
      hgt_bins = new_val;
    } else if (strncmp(argv[iarg], "-o=", 3) == 0) {
      std::strcpy (out_file, argv[iarg]+3);
    } else if (strncmp(argv[iarg], "-h", 2) == 0) {
      usage();
    } else if (strncmp(argv[iarg], "--h", 3) == 0) {
      usage();
    } else {
      // assume it's the file name
      std::strcpy (in_file, argv[iarg]);
    }
  }

  // load in the test file
  std::vector<unsigned char> in_image; // the raw pixels
  unsigned int in_width, in_height;
  unsigned int error = lodepng::decode(in_image, in_width, in_height, in_file);
  //if there's an error, display it
  if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

  // decompose it to hsv
/*
  std::vector<unsigned char> in_hsv;
  in_hsv.resize(in_width * in_height * 4);
  for (unsigned int i = 0; i < in_width*in_height; i++) {
    // assign r,g,b to temporaries
    unsigned char r = in_image[4*i+0];
    unsigned char g = in_image[4*i+1];
    unsigned char b = in_image[4*i+2];
    // copy over the alpha channel
    in_hsv[4*i+3] = in_image[4*i+3];
    // and make some references to the hsv array?

    // now convert
    // using code from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    unsigned char rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    in_hsv[4*i+2] = rgbMax;
    if (in_hsv[4*i+2] == 0)
    {
        in_hsv[4*i+0] = 0;
        in_hsv[4*i+1] = 0;
        continue;
    }

    in_hsv[4*i+1] = 255 * long(rgbMax - rgbMin) / in_hsv[4*i+2];
    if (in_hsv[4*i+1] == 0)
    {
        in_hsv[4*i+0] = 0;
        continue;
    }

    if (rgbMax == r)
        in_hsv[4*i+0] = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        in_hsv[4*i+0] = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        in_hsv[4*i+0] = 171 + 43 * (r - g) / (rgbMax - rgbMin);
  }
*/

  //float hue_displace = 0.05 * (float)std::max(in_width, in_height);
  //unsigned int band = (int)hue_displace + 2;


  // create new image data arrays
  unsigned int band = 0;
  unsigned int out_width = in_width + 2*band;
  unsigned int out_height = in_height + 2*band;
  std::cout << "output image will be " << out_width << "x"<< out_height << std::endl;
  std::vector<unsigned char> out_image;
  out_image.resize(out_width * out_height * 4);
  // fill with solid white
  for (unsigned int i = 0; i < out_width*out_height; i++) {
    out_image[4*i+0] = 255;
    out_image[4*i+1] = 255;
    out_image[4*i+2] = 155;
    out_image[4*i+3] = 255;
    //out_image[4*i+3] = 0;
  }
  // copy original image into the middle
  for (unsigned int i = 0; i < in_width*in_height; i++) {
    unsigned int oi = in_width * (band+(i/in_width)) + band + i%in_width;
    out_image[4*oi+0] = in_image[4*i+0];
    out_image[4*oi+1] = in_image[4*i+1];
    out_image[4*oi+2] = in_image[4*i+2];
    out_image[4*oi+3] = in_image[4*i+3];
  }

  // generate the shadow map (here 0.0 = no shadow, 1.0 = full shadow)
  std::vector<float> shadow;
  shadow.resize(out_width * out_height);
  for (unsigned int i = 0; i < out_width*out_height; i++) {
    shadow[i] = 0.0;
  }
  // make a copy so that we can diffuse it
  std::vector<float> shadow_copy;
  shadow_copy.resize(out_width * out_height);

  // idea: blur the input color map before computing heights!

  // calculate height map (0..1)
  float min_hgt = 9.9e+9;
  float max_hgt = -9.9e+9;
  std::vector<float> hgt;
  hgt.resize(out_width * out_height);

  // use rgb weights
  for (unsigned int i = 0; i < out_width*out_height; i++) {
    // copy over the alpha channel
    hgt[i] = scale_r * (float)in_image[4*i+0]/255.0
           + scale_g * (float)in_image[4*i+1]/255.0
           + scale_b * (float)in_image[4*i+2]/255.0;
    if (hgt[i] < min_hgt) min_hgt = hgt[i];
    if (hgt[i] > max_hgt) max_hgt = hgt[i];
  }

  // use hsv heights

  // normalize heights
  std::cout << "min/max height " << min_hgt << "/"<< max_hgt << std::endl;
  // scale to 0..1
  for (unsigned int i = 0; i < out_width*out_height; i++) {
    hgt[i] = (hgt[i] - min_hgt) / (max_hgt - min_hgt);
  }


  // march through heights from top to bottom
  float last_hgt = 1.0 + 0.5/(float)hgt_bins;
  for (unsigned int ilayer = 0; ilayer < hgt_bins+1; ++ilayer) {

    std::cout << "  computing layer " << ilayer;
    float this_hgt = last_hgt - 1.0/(float)hgt_bins;

    // loop over all pixels and trigger any that are now above the active height
    unsigned int num_pix = 0;
    #pragma omp parallel for reduction(+:num_pix)
    for (unsigned int i = 0; i < out_width*out_height; i++) {
      if (hgt[i] > this_hgt && hgt[i] < last_hgt) {
        // apply current shadow to the pixel
        const float factor = 1.0 - shade_coeff*shadow[i];
        out_image[4*i+0] = (unsigned char)(factor*(float)out_image[4*i+0]);
        out_image[4*i+1] = (unsigned char)(factor*(float)out_image[4*i+1]);
        out_image[4*i+2] = (unsigned char)(factor*(float)out_image[4*i+2]);

        // and update the shadow map
        // should we set to 1, or add 1?
        shadow[i] = 1.0;

        // increment our counter
        num_pix++;
      }
    }
    std::cout << ", found " << (100.0*num_pix)/((float)out_width*out_height) << "% pixels";

    // update the bands
    last_hgt = this_hgt;

    // are we done?
    if (this_hgt < 0.0) {
      std::cout << std::endl;
      break;
    }

    // diffuse the shadow map

    // might need to do this several times
    const float total_coeff = shadow_width * (float)out_width / (float)hgt_bins;
    const unsigned int num_iters = 1 + (int)total_coeff;
    const float coeff_per = total_coeff / (float)num_iters;
    std::cout << ", with " << num_iters << " diffusion iters";

    for (unsigned int it = 0; it < num_iters; ++it) {
      // first, make a copy
      shadow_copy = shadow;

      #pragma omp parallel for
      for (unsigned int y = 1; y < out_height-1; ++y) {
        for (unsigned int x = 1; x < out_width-1; ++x) {
          const size_t addr = out_width*y + x;
          shadow[addr] += coeff_per*0.125*(shadow_copy[addr-1] +
                                           shadow_copy[addr+1] +
                                           shadow_copy[addr-out_width] +
                                           shadow_copy[addr+out_width] -
                                           4.0*shadow_copy[addr]);
        }
      }
    }

    // shift the shadow map
    // first, make a copy
    shadow_copy = shadow;
    // how far to look
    const float dy = (float)out_height * shadow_shift / (float)hgt_bins;
    std::cout << ", shifted " << dy << " pix down" << std::endl;
    const unsigned int iy = dy;
    const unsigned int iyp1 = iy+1;
    const float yfrac = dy - (float)iy;

    #pragma omp parallel for
    for (unsigned int y = iyp1; y < out_height; ++y) {
      for (unsigned int x = 0; x < out_width; ++x) {
        const size_t addr = out_width*y + x;
        shadow[addr] = yfrac*shadow_copy[addr-iyp1*out_width] + (1.0-yfrac)*shadow_copy[addr-iy*out_width];
      }
    }

  }


  // output to a new png
  error = lodepng::encode(out_file, out_image, out_width, out_height);
  //if there's an error, display it
  if (error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

