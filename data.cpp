//
//  data.cpp
//  ubimet
//
//  Created by Héctor Esteban Cabezos on 04/06/2020.
//  Copyright © 2020 Héctor Esteban Cabezos. All rights reserved.
//

#include "data.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <array>
#include <png.h>
#include "3rd_party/heatmap.h"
#include "3rd_party/lodepng.h"

/**
 * Constructor
 * @param in_filename Absolute path of the binary file to be read.
 * @param out_csv Name of the generated csv file.
 */
Data::Data(const char* in_filename,const char* out_csv): input_filename(in_filename), csv_filename(out_csv)
{
    readFile();
}

/**
 * Obtain Phi projection
 * @param row Row of the concrete pixel being converted.
 * @param col Column of the concrete pixel being converted.
 */
float Data::productPh(int row, int col){
    return (1.570796326795-2*atan(0.404026225835*pow((1/sin(atan(1/(5899179.2/\
    ((5676531.8*pow((tan((1.570796326795-(v1*0.017453292520))/2)/\
    0.373884679485),0.737361597616)*sin(0.012869387656*(v0-13.333333333))-\
    v3*v4)+col*v4)-((6380000.0*(0.924636243305-0.889738536848*pow((tan((\
    1.570796326795-(v1*0.017453292520))/2)/0.373884679485),0.737361597616)*\
    cos(0.012869387656*(v0-13.333333333)))+v2*v5)-row*v5)/((5676531.8*\
    pow((tan((1.570796326795-(v1*0.017453292520))/2)/0.373884679485),\
    0.737361597616)*sin(0.012869387656*(v0-13.333333333))-v3*v4)+col*v4)))))*\
    ((5676531.8*pow((tan((1.570796326795-(v1*0.017453292520))/2)/\
    0.373884679485),0.737361597616)*sin(0.012869387656*(v0-13.333333333))-\
    v3*v4)+col*v4)/6380000.0*0.737361597616/sin(0.767944870878),1/\
                                                     0.737361597616)));
}

/**
 * Obtain lambda projection.
 * @param row Row of the concrete pixel being converted.
 * @param col Column of the concrete pixel being converted.
 * @return lambda projection with respect the radar position
 */
float Data::productLA(int row, int col){
    return atan(1/(5899179.2/((5676531.8*pow((tan((1.570796326795-(v1*0.017453292520))\
    /2)/0.373884679485),0.737361597616)*sin(0.012869387656*(v0-13.333333333))\
    -v3*v4)+col*v4)-((6380000.0*(0.924636243305-0.889738536848*pow((tan((\
    1.570796326795-(v1*0.017453292520))/2)/0.373884679485),0.737361597616)*\
    cos(0.012869387656*(v0-13.333333333)))+v2*v5)-row*v5)/((5676531.8*pow((\
    tan((1.570796326795-(v1*0.017453292520))/2)/0.373884679485),\
    0.737361597616)*sin(0.012869387656*(v0-13.333333333))-v3*v4)+col*v4)))/\
    0.737361597616+0.232710566928;
}

/**
 * From pixels to (lat,lon)
 * @param row Row of the concrete pixel being converted.
 * @param col Column of the concrete pixel being converted.
 * @return Struct defining (lat,lon)
 */
Spherical Data::pixelToGeographical(int row, int col){
    Spherical conversion{};
    conversion.latitude = productPh(row,col)*180/M_PI;
    conversion.longitude = productLA(row,col)*180/M_PI;
    return conversion;
}

/**
 * Save the corresponding (lon,lat) from the image into CSV with their corresponding value, which has units
 * mm/h
 * @param sph Vector with the (lat,lon) for each position.
 * @param value The mm/h for each position in the max image
 */
void Data::createCsv(const std::vector<Spherical>& sph, const std::vector<short int>& value){
    std::ofstream myfile;
    myfile.open (csv_filename);
    myfile << "lat;lon;value\n";
    for (int i=0;i<sph.size();i++)
    {
        myfile << sph[i].latitude << ";" << sph[i].longitude << ";" << value[i] << "\n";
    }
    myfile.close();
}

/**
 * Create the maximum image from all the verical layers. The L layers are used and for each (x,y) pixel I 
 * get the maximum of (x,y) in all L layers. Final result has same dimension as a single layer picture.
 * @param sph Vector with the (lat,lon) for each position.
 */
void Data::maxColumn(const std::vector<Spherical> &sph){
    std::vector<short int> max_picture;
    for (int i=0;i<Pixels[0].size();i++)
    {
        short int max_col_i = 0;
 
        for (int j=0; j<L; j++){
            if (Pixels[j][i] > max_col_i)
            {
                max_col_i = Pixels[j][i];
            }
        }
        max_picture.push_back(max_col_i);
    }
    plotImage(Nx, Ny, max_picture,sph);
    createCsv(sph, max_picture);
}

/**
 * Obtain the maximum/minimum latitude and longitude from the data
 */
void Data::obtainLimitsMap(const std::vector<Spherical> &sph){
    auto check = [this](const Spherical& sphere) { 
        if (sphere.latitude > max_lat) {max_lat = sphere.latitude;}
        else if (sphere.latitude < min_lat) {min_lat = sphere.latitude;}
        if (sphere.longitude > max_lon) {max_lon = sphere.longitude;}
        else if (sphere.longitude < min_lon) {min_lon = sphere.longitude;}
    };
    std::for_each(sph.cbegin(), sph.cend(), check);
}

/**
 * Create the heatmap PNG file,  suing the package in https://github.com/lucasb-eyer/libheatmap
 * @param Nx Number of columns of data.
 * @param Ny Number of rows of data.
 * @param image Array with the data to be converted to PNG.
 * @param counter To be included in the filename to distinguish between pictures.
 */
void Data::plotImage(int Nx, int Ny, const std::vector<short int>& image, const std::vector<Spherical> &sph, int counter){
    heatmap_t* hm = heatmap_new(Nx, Ny);
    short int value;
    int aprox_row;
    int aprox_column;
    obtainLimitsMap(sph); // gives the maximum/minimum latitude and longitude as max_lat, min_lat, max_long, min_long
    for(int i = 0 ; i < image.size() ; ++i) {
        value = image[i];
        aprox_row = Ny - (int)round(Ny*static_cast<double>(sph[i].latitude-min_lat)/(max_lat-min_lat)); // Added Ny begining as orifin is bottom left
        aprox_column = (int)round(Nx*static_cast<double>(sph[i].longitude-min_lon)/(max_lon-min_lon));
        heatmap_add_weighted_point(hm, aprox_column, aprox_row, value); // TODO: Should plot based on (lat,row) not (row,column)
    }
    unsigned char im[Nx*Ny*4];
    heatmap_render_default_to(hm, &im[0]); // Render map into picture
    heatmap_free(hm); // I do not need the map anymore
    writepng("heatmap"+std::to_string(counter)+".png", Nx, Ny, im);
}

/**
 * Convert from pixels in the image into (lat,lon)
 * @return Vector of size (NX*NY) of Spherical objects
 */
std::vector<Spherical> Data::convertCoordinates(){
    std::vector<Spherical> translation;
    int row;
    int col;
    for (int i=0; i<Pixels[0].size(); i++)
    {
        row = i/Ny;
        col = i%Ny;
        translation.push_back(pixelToGeographical(row, col));
    }
    return translation;
}

/**
 * Read the binary file. While reading the file it saves as well
 * pictures for each vertical layer
 */
void Data::readFile(){
    std::streampos fileSize;
    std::ifstream file(input_filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<unsigned char> fileData(fileSize);
    file.read((char*) &fileData[0], fileSize);
    id = fileData[0];
    for(int i=0; i<17;i++)
    {
        date[i] = fileData[1+i];
    }
    Ny = fileData[18] | (fileData[19] << 8);
    Nx = fileData[20] | (fileData[21] << 8);
    L = fileData[22];
    distance = fileData[23] | (fileData[24] << 8);
    N = fileData[25];
    short int counter = 0;
    quant_levels[N] = 0; // unknown
    for (int i=26; i<26+N*2; i+=2)
    {
        quant_levels[counter++] = fileData[i] | (fileData[i+1] << 8);
    }
    int init = 26+N*2;
    for (int i=0;i<L;i++)
    {
        std::vector<short int> aux;
        for (int j=init + i*Ny*Nx; j<init + (i+1)*Ny*Nx; j++)
        {
            aux.push_back(quant_levels[fileData[j]]);
        }
        Pixels.push_back(aux);
        //plotImage(Nx, Ny, aux, i);
    }
}

/**
 * Will call the convert cordinates function to obtain (lat,lon)
 * and then will create PNG image and CSV file
 */
void Data::obtain_results()
{
    std::vector<Spherical> conversion = convertCoordinates();
    maxColumn(conversion);
}

/**
 * Write from array into PNG.
 * Extracted from https://github.com/lucasb-eyer/libheatmap/blob/master/examples/simplest_libpng.cpp
 * @param filename Name of the saved PNG file.
 * @param Nx Number of columns.
 * @param Ny Number of rows.
 * @param data Array with the data to be converted to PNG.
 * @return Confirm operation was successful
 */
unsigned int Data::writepng(const std::string& filename, int Nx, int Ny, const unsigned char* data)
{
    // NULLS are user error/warning functions.
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png_ptr) {
        std::cerr << "Error initializing libpng write struct." << std::endl;
        return 2;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
       std::cerr << "Error initializing libpng info struct." << std::endl;
       png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
       return 3;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        std::cerr << "Error in setjmp!?" << std::endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 4;
    }

    FILE *fp = fopen(filename.c_str(), "wb");
    if(!fp) {
        std::cerr << "Error writing " << filename << ": " << strerror(errno) << std::endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 5;
    }
    png_init_io(png_ptr, fp);

    // Could be used for progress report of some sort.
    //png_set_write_status_fn(png_ptr, write_row_callback);

    // turn on or off filtering, and/or choose specific filters.
    // You can use either a single PNG_FILTER_VALUE_NAME or the logical OR
    // of one or more PNG_FILTER_NAME masks.
    png_set_filter(png_ptr, 0,
         PNG_FILTER_NONE  | PNG_FILTER_VALUE_NONE
       // | PNG_FILTER_SUB   | PNG_FILTER_VALUE_SUB
       // | PNG_FILTER_UP    | PNG_FILTER_VALUE_UP
       // | PNG_FILTER_AVE   | PNG_FILTER_VALUE_AVE
       // | PNG_FILTER_PAETH | PNG_FILTER_VALUE_PAETH
       // | PNG_ALL_FILTERS
    );

    // set the zlib compression level.
    // 1 = fast but not much compression, 9 = slow but much compression.
    png_set_compression_level(png_ptr, 1);

    static const int bit_depth = 8;
    static const int color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    static const int interlace_type = PNG_INTERLACE_ADAM7; // or PNG_INTERLACE_NONE
    png_set_IHDR(png_ptr, info_ptr, Nx, Ny, bit_depth, color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytep* row_pointers = (png_byte**)png_malloc(png_ptr, Ny*sizeof(png_bytep));
    for(size_t y = 0 ; y < Ny ; ++y) {
        row_pointers[y] = const_cast<png_bytep>(data + y*Nx*4);
    }
    png_set_rows(png_ptr, info_ptr, row_pointers);

    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

    // Cleanup
    png_free(png_ptr, row_pointers);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}
