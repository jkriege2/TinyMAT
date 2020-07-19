/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/


#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include "tinymatwriter.h"
#include <cmath>

using namespace std;


int main( int argc, const char* argv[] ) {
    TinyMATWriterFile* mat=TinyMATWriter_open("basic_test.mat");
	if (mat) {
	    // a matrix in row-major form
		double mat1[6]={
			1,2,
			3,4,
			5,6
		};
		int32_t mat1_size[2] = {2,3};
		// a matrix in column-major form (the same matrix as mat1)!!!
		double mat1cm[6]={
			1,3,5,
			2,4,6
		};
		// a data vector
		double vec1[8]={1,2,3,4,5,6,7,8};
		// a 3D matrix in row-major
		double mat3[3*3*3]= {
		    1,2,3,
			4,5,6,
			7,8,9,
			
			10,20,30,
			40,50,60,
			70,80,90,
			
			100,200,300,
			400,500,600,
			700,800,900
		};
		int32_t mat3_size[3] = {3,3,3};
		// another 3D matrix in row-major
		double mat432[4*3*2]= {
		    1,2,3,
			4,5,6,
			
			10,20,30,
			40,50,60,
			
			100,200,300,
			400,500,600,
			
			1000,2000,3000,
			4000,5000,6000,
		};
		int32_t mat432_size[3] = {3,2,4}; // columns, rows, matrices,...
		int16_t mat432i16[4*3*2]= {
		    1,2,3,
			4,5,6,
			
			10,20,30,
			40,50,60,
			
			100,200,300,
			400,500,600,
			
			1000,-2000,3000,
			-4000,5000,-6000,
		};
		int32_t mat432i16_size[3] = {3,2,4}; // columns, rows, matrices,...
		
		// a boolean matrix
		bool matb[4*3*2] = {
			true,false,true,
			false,true,false,
			
			true,true,true,
			false,false,false,
			
			true,false,true,
			true,false,true,
			
			true,true,false,
			false,true,true
		};
		int32_t matb_size[3] = {3,2,4}; // columns, rows, matrices,...
		
		// a struct as a map of doubles
		std::map<std::string, double> mp1;
		mp1["x"]=100;
		mp1["y"]=200;
		mp1["z"]=300;
		mp1["longname"]=10000*M_PI;
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector1", vec1, 1,8);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector2", vec1, 8,1);
		TinyMATWriter_writeStruct(mat, "struct1", mp1);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "matrix1", mat1, 2,3);
		TinyMATWriter_writeMatrix2D_colmajor(mat, "matrix1_fromcolmajor", mat1cm, 2,3);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "matrix1_ver2", mat1, mat1_size, 2);
		
		TinyMATWriter_writeMatrixND_colmajor(mat, "matrix3d", mat3, mat3_size, 3);
		TinyMATWriter_writeMatrixND_colmajor(mat, "matrix432d", mat432, mat432_size, 3);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "matrix3d_rowmajor", mat3, mat3_size, 3);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "matrix432d_rowmajor", mat432, mat432_size, 3);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "boolmatrix", matb, matb_size, 3);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "mat432i16", mat432i16, mat432i16_size, 3);

		TinyMATWriter_close(mat);
	}
    return 0;
}
