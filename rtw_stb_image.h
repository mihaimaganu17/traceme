#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "stb/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image {
    public:
        rtw_image() {}

        // Loads image data from the specified `image_filename`.
        rtw_image(const char* image_filename) {
            // If the `RTW_IMAGES` environment variable is defined, looks only in that directory
            // for the image file.
            auto imagedir = getenv("RTW_IMAGES");

            if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;

            auto filename = std::string(image_filename);

            // If the image was not loaded, we search for the image in the current dirrectory, then
            // in the current's path `image` folder and if not found in the parent's path image
            // folder.
            if (load(filename)) return;
            if (load("images/" + filename)) return;
            if (load("../images/" + filename)) return;

            std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
        }

        // Make sure we free up resources
        ~rtw_image() {
            delete[] bdata;
            STBI_FREE(fdata);
        }

        // Loads the linear (gamma=1) image data from the given file name. Returns true if the load
        // succeeded.
        // The resulting data buffer contains the three (red, green, blue) floating-point values
        // each between [0.0, 1.0] for the first pixel.
        // Pixels are contiguous, going left to right for the width of the images, followed by
        // the next row below, for the full height of the image.
        bool load(const std::string& filename) {
            fdata = stbi_loadf(filename.c_str(), &image_width, &image_height,
                    &channels_in_file, bytes_per_pixel);
            if (fdata == nullptr) return false;

            // Compute number of bytes needed for each scanline
            bytes_per_scanline = image_width * bytes_per_pixel;
            convert_to_bytes();
            return true;
        }

        int width() const { return (fdata == nullptr) ? 0 : image_width; }
        int height() const { return (fdata == nullptr) ? 0 : image_height; }

        // Return the byte offset (from bdata) of the pixel at x,y coordinates. If there is no
        // image data ,returns magenta.
        const unsigned char* pixel_data(int x, int y) const {
            static unsigned char magenta[] = { 255, 0, 255 };

            // If there is no data, we return magenta by default.
            if (bdata == nullptr) return magenta;

            // Make sure we do not return outside of bounds
            x = clamp(x, 0, image_width);
            y = clamp(y, 0, image_height);

            return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
        }

    private:
        const int bytes_per_pixel = 3;
        int channels_in_file = 3;
        // Linear floating point pixel data which we read from an image
        float *fdata = nullptr;
        // Linear 8-bit pixel data
        unsigned char *bdata = nullptr;
        int image_width = 0;
        int image_height = 0;
        int bytes_per_scanline = 0;

        // Convert a floating-point `value` between [0.0, 1.0] into an int between [0, 255]
        static unsigned char float_to_byte(float value) {
            if (value <= 0.0)
                return 0;
            if (1.0 <= value)
                return 255;
            return static_cast<unsigned char>(256.0 * value);
        }

        static int clamp(int value, int min, int max) {
            if (value < min) {
                return min;
            } else if (value > max) {
                return max - 1;
            } else {
                return value;
            }
        }

        // Convert the linear floating point pixel data to bytes, storing the resulting byte data
        // in the `bdata` member;
        void convert_to_bytes() {
            // Compute the total number of bytes
            int total_bytes = image_width * image_height * bytes_per_pixel;
            // Allocate a new array with that many elements
            bdata = new unsigned char[total_bytes];

            // Iterate through all pixel components, converting from [0.0, 1.0] float values to
            // unsigned [0, 255] byte values.
            auto *bptr = bdata;
            auto *fptr = fdata;

            for (auto i=0; i < total_bytes; i++, fptr++, bptr++)
                *bptr = float_to_byte(*fptr);
        }
};

#endif
