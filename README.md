# TinyMAT
C/C++ library to handle writing simple Matlab(r) MAT file

This library implements a very simple interface to write Matlab MAT file (level 5), as described in http://www.mathworks.de/help/pdf_doc/matlab/matfile_format.pdf

It is licensed under the terms of the GNU Lesser general Public license (LGPL) >=2.1

# Example
The following example code writes some arrays and matrices into a MAT-file:
```C++
	TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
	if (mat) {
		double mat1[4]={1,2,3,4};
		double vec1[8]={1,2,3,4,5,6,7,8};
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector1", vec1, 1,8);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "matrix1", mat1, 2,2);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector2", vec1, 8,1);
		TinyMATWriter_close(mat);
	}
```
