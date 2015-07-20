# TinyMAT
A (partly templated) C++ library to handle writing simple Matlab(r) MAT file in Version "MATLAB 5.0" or higher

This library implements a very simple interface to write Matlab MAT file (level 5), as described in http://www.mathworks.de/help/pdf_doc/matlab/matfile_format.pdf

It is licensed under the terms of the GNU Lesser general Public license (LGPL) >=2.1

# Example
The following example code writes some arrays and matrices into a MAT-file:
```C++
	TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
	if (mat) {
		// a 3D matrix in row-major
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

		// a struct as a map of doubles
		std::map<std::string, double> mp1;
		mp1["x"]=100;
		mp1["y"]=200;
		mp1["z"]=300;
		mp1["longname"]=10000*M_PI;

		TinyMATWriter_writeStruct(mat, "struct1", mp1);
		TinyMATWriter_writeMatrixND_rowmajor(mat, "matrix432", mat432, mat432_size, 3);
		
		TinyMATWriter_close(mat);
	}
```
