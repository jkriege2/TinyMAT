/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-07-07 12:07:58 +0200 (Di, 07 Jul 2015) $  (revision $Rev: 4005 $)

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


#include <iostream>
#include <stdio.h>
#include "../tinymatwriter.h"


using namespace std;


int main( int argc, const char* argv[] ) {
	TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
	if (mat) {
		double mat1[4]={1,2,3,4};
		double vec1[8]={1,2,3,4,5,6,7,8};
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector1", vec1, 1,8);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "matrix1", mat1, 2,2);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector2", vec1, 8,1);
		TinyMATWriter_close(mat);
	}
    return 0;
}
