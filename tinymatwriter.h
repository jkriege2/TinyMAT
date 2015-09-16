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


#ifndef TINYMATWRITER_H
#define TINYMATWRITER_H

#ifndef TINYMATWRITER_LIB_EXPORT
#    define TINYMATWRITER_LIB_EXPORT
#endif

#include <inttypes.h>
#include <list>
#include <vector>
#include <string>
#include <map>

#ifdef TINYMAT_USES_QVARIANT
#  include <QVariant>
#  include <QList>
#  include <QStringList>
#  include <QMap>
#  include <QVector>
#  include <QString>
#endif


/*! \defgroup tinymatwriter Tiny Matlab(r) MAT writer library
    \ingroup tools

\code
    TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
	if (mat) {
		double mat1[4]={1,2,3,4};
		double vec1[8]={1,2,3,4,5,6,7,8};
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector1", vec1, 1,8);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "matrix1", mat1, 2,2);
		TinyMATWriter_writeMatrix2D_rowmajor(mat, "vector2", vec1, 8,1);
		TinyMATWriter_close(mat);
	}
\endcode
    Here is Octave/Matlab code to test the output and create a comparable MAT-file:
\code
	mat1=[1,2;3,4];
	vec1=[1,2,3,4,5,6,7,8];
	vec2=vec1';

	save("-v6", "./test_octave.mat", "vec1", "mat1", "vec2")

	load("test.mat", "-v6")

	if (vec1==vector1) 
		disp('vec1 OK');
	end

	if (mat1==matrix1) 
		disp('mat1 OK');
	end

	if (vec2==vector2) 
		disp('vec2 OK');
	end
\endcode
   
 */



/** \brief struct used to describe a TIFF file
  * \ingroup tinymatwriter
  */
struct TinyMATWriterFile; // forward

#ifndef TRUE
#  define TRUE (0==0)
#endif
#ifndef FALSE
#  define FALSE (0==1)
#endif

/*! \brief returns \c TRUE (non-zero) if the given TinyMATWriterFile has been opened successfully and is OK
    \ingroup tinymatwriter

    \param mat the MAT-file
    \return \c TRUE if the file is OK and can be written to or \c FALSE

  */
TINYMATWRITER_LIB_EXPORT int TinyMATWriter_fOK(const TinyMATWriterFile* mat);

/*! \brief create a new MAT file
    \ingroup tinymatwriter

    \param filename name of the new TIFF file
    \param description description of the file (max. 115 characters)
    \return a new TinyMATWriterFile pointer on success, or NULL on errors

  */
TINYMATWRITER_LIB_EXPORT TinyMATWriterFile* TinyMATWriter_open(const char* filename, const char* description=NULL);

/*! \brief write a string into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the string to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeString(TinyMATWriterFile* mat, const char* name, const char* data);
/*! \brief write a string into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the string to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeString(TinyMATWriterFile* mat, const char* name, const std::string& data);

/*! \brief write a string into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the string to write
    \param slen length of the string in bytes

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeString(TinyMATWriterFile* mat, const char* name, const char* data, uint32_t slen);


/*! \brief write an empty (double) matrix into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeEmptyMatrix(TinyMATWriterFile* mat, const char* name);





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// METHODS TO WRITE MATRICES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief write a N-dimensional double matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const double* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional float matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const float* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional uint64_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const uint64_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional int64_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const int64_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional uint32_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const uint32_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional int32_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const int32_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional uint16_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const uint16_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional int16_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const int16_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional uint8_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const uint8_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional int8_t matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const int8_t* data_real, const int32_t* sizes, uint32_t ndims) ;

/*! \brief write a N-dimensional bool matrix in column-major form into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param sizes number of entries in each dimension {rows, cols, matrices, ...}
    \param ndims number of dimensions

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile* mat, const char* name, const bool* data_real, const int32_t* sizes, uint32_t ndims) ;



/*! \brief write a N-dimensional double  matrix  into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in row-major order) {M1row1, M1row2, ..., M1rowC, M2row1, M2row2, ... }
    \param sizes number of entries in each dimension {cols, rows, matrices, ...}
    \param ndims number of dimensions

  */
template<typename T>
inline void TinyMATWriter_writeMatrixND_rowmajor(TinyMATWriterFile* mat, const char* name, const T* data_real, const int32_t* sizes, uint32_t ndims) {
    T* dat=NULL;
    int32_t* siz=NULL;
    if (data_real && sizes && ndims>1) {
        uint32_t nentries=1;
        uint32_t cols=1;
        uint32_t rows=1;
        uint32_t nmatrices=1;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            }
            nentries=nentries*sizes[i];

            if (i==0) cols=sizes[i];
            else if (i==1) rows=sizes[i];
            else {
                nmatrices=nmatrices*sizes[i];
            }
        }
        if (nentries>0) {
            dat=new T[nentries];//(T*)malloc(nentries*sizeof(T));
            siz=new int32_t[ndims];//=(int32_t*)malloc(ndims*sizeof(int32_t));
            for (uint32_t i=0; i<ndims; i++) siz[i]=sizes[i];
            if (ndims>1) {
                siz[0]=sizes[1];
                siz[1]=sizes[0];
            }
            for (uint32_t m=0; m<nmatrices; m++) {
                for(uint32_t r=0; r<rows; r++) {
                    for (uint32_t c=0; c<cols; c++) {
                        dat[m*cols*rows+c*rows+r]=data_real[m*cols*rows+r*cols+c];
                    }
                }
            }
        }

    }
    if (dat) {
        TinyMATWriter_writeMatrixND_colmajor(mat, name, dat, siz, ndims);
        //free(dat);
        //free(siz);
        delete[] dat;
        delete[] siz;
    } else {
        TinyMATWriter_writeMatrixND_colmajor(mat, name, data_real, sizes, ndims);
    }
}

/*! \brief write a 2-dimensional double matrix in column-major order into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in column-major order)
    \param cols number of columns
	\param rows number of rows

  */
template<typename T>
inline  void TinyMATWriter_writeMatrix2D_colmajor(TinyMATWriterFile* mat, const char* name, const T* data_real, int32_t cols, int32_t rows) {
    int32_t siz[2]={rows, cols};
    TinyMATWriter_writeMatrixND_colmajor(mat, name, data_real, siz, 2);
}

/*! \brief write a 2-dimensional double matrix in row-major order into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in row-major order)
    \param cols number of columns
    \param rows number of rows

  */
template<typename T>
inline  void TinyMATWriter_writeMatrix2D_rowmajor(TinyMATWriterFile* mat, const char* name, const T* data_real, int32_t cols, int32_t rows) {
    int32_t siz[2]={cols, rows};
    TinyMATWriter_writeMatrixND_rowmajor(mat, name, data_real, siz, 2);
}

/*! \brief write a 1-dimensional double vector as a row-vector into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in row-major order)
    \param cols number of columns
    \param rows number of rows

  */
template<typename T>
inline  void TinyMATWriter_writeVecorAsRow(TinyMATWriterFile* mat, const char* name, const T* data_real, int32_t rows) {
    int32_t siz[2]={1, rows};
    TinyMATWriter_writeMatrixND_rowmajor(mat, name, data_real, siz, 2);
}

/*! \brief write a 1-dimensional double vector as a column-vector into a MAT-file
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data_real the array to write (in row-major order)
    \param cols number of columns
    \param rows number of rows

  */
template<typename T>
inline  void TinyMATWriter_writeVecorAsColumn(TinyMATWriterFile* mat, const char* name, const T* data_real, int32_t rows) {
    int32_t siz[2]={rows, 1};
    TinyMATWriter_writeMatrixND_rowmajor(mat, name, data_real, siz, 2);
}





















/*! \brief write a 1-dimensional std::list<std::string> into a MAT-file as a cell array
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeStringList(TinyMATWriterFile* mat, const char* name, const std::list<std::string>& data);

/*! \brief write a 1-dimensional std::vector<std::string> into a MAT-file as a cell array
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeStringVector(TinyMATWriterFile* mat, const char* name, const std::vector<std::string>& data);

/*! \brief write a 1-dimensional std::list<double> into a MAT-file as a 1D matrix
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write
    \param columnVector if \C true, data is stored as a column vector ... otherwise as a row-vetor

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeDoubleList(TinyMATWriterFile* mat, const char* name, const std::list<double>& data, bool columnVector=false);

/*! \brief write a 1-dimensional std::vector<double> into a MAT-file as a 1D matrix
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write
    \param columnVector if \C true, data is stored as a column vector ... otherwise as a row-vetor

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeDoubleVector(TinyMATWriterFile* mat, const char* name, const std::vector<double>& data, bool columnVector=false);


/*! \brief write a a std::map<std::string,double> into a MAT-file as a struct
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write
    \param columnVector if \C true, data is stored as a column vector ... otherwise as a row-vetor

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeStruct(TinyMATWriterFile* mat, const char* name, const std::map<std::string, double>& data);


#ifdef TINYMAT_USES_QVARIANT
/*! \brief write a 1-dimensional QVariantList into a MAT-file as a cell array
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeQVariantList(TinyMATWriterFile* mat, const char* name, const QVariantList& data);
/*! \brief write a 1-dimensional QStringList into a MAT-file as a cell array
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeQStringList(TinyMATWriterFile* mat, const char* name, const QStringList& data);
/*! \brief write a 1-dimensional QVariantList (as a list of columns) into a MAT-file as a cell array
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the array to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeQVariantMatrix_listofcols(TinyMATWriterFile* mat, const char* name, const QList<QList<QVariant> >& data);
/*! \brief write a QVariantMap into a MAT-file as a struct
    \ingroup tinymatwriter

    \param mat the MAT-file to write into
    \param name variable name for the new array
    \param data the map to write

  */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeQVariantMap(TinyMATWriterFile* mat, const char* name, const QVariantMap& data);

#endif

/*! \brief close a given MAT file
    \ingroup tinymatwriter

    \param tiff TIFF file to close

    This function also releases memory allocated in TinyMATWriter_open() in \a tiff.
 */
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_close(TinyMATWriterFile* mat);

#endif // TINYMATWRITER_H
