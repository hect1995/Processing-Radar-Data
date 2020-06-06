//
//  data.hpp
//  ubimet
//
//  Created by Héctor Esteban Cabezos on 04/06/2020.
//  Copyright © 2020 Héctor Esteban Cabezos. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <map>


struct Spherical{
    float latitude;
    float longitude;
};

class Data{
    
private:
    short int id; // 1 byte
    char date[17];
    short int Ny;
    short int Nx;
    short int L;
    short int distance;
    short int N;
    std::map<short int, short int> quant_levels;
    std::vector<std::vector<short int>> Pixels;
    
    const char* input_filename;
    const char* csv_filename;
    static constexpr double v0 = 8.194; //(reference lon)
    static constexpr double v1 = 50.437; //(reference lat)
    static constexpr double v2 = 0;
    static constexpr double v3 = 0;
    static constexpr double v4 = 1018.18; // (meter / pixel)
    static constexpr double v5 = 1018.18; //(meter / pixel)
    
public:
    Data(const char* in_filename, const char* out_csv="output.csv");
    std::vector<Spherical> convertCoordinates();
    Spherical pixelToGeographical(int row, int col);
    void plotImage(int Nx, int Ny, const std::vector<short int>& image);
    static float productPh(int row, int col);
    static float productLA(int row, int col);
    void maxColumn(const std::vector<Spherical> &sph);
    void createCsv(const std::vector<Spherical>& sph, const std::vector<short int>& value);

    void readFile();
    unsigned int writepng(const std::string& filename, int Nx, int Ny, const unsigned char* data);
};

#endif /* data_hpp */
