#include "tinymatwriter.h"


using namespace std;


int main() {
    TinyMATWriterFile* mat=TinyMATWriter_open("extcmake_tinymat_test.mat");
	if (mat) {
	    // a matrix in row-major form
		double mat1[6]={
			1,2,
			3,4,
			5,6
		};

		TinyMATWriter_writeMatrix2D_rowmajor(mat, "matrix1", mat1, 2,3);

		TinyMATWriter_close(mat);
	}
    return 0;
}
