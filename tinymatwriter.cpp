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
#include <math.h>
#include <float.h>
#include <stdio.h>
#include "tinymatwriter.h"
#include <iostream>
#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif
#ifdef TINYMAT_USES_QVARIANT
#  include <QDebug>
#  include <QPoint>
#  include <QSize>
#endif

#define TINYMAT_ORDER_UNKNOWN 0
#define TINYMAT_ORDER_BIGENDIAN 1
#define TINYMAT_ORDER_LITTLEENDIAN 2



/*! \brief this struct represents a mat file
    \ingroup TinyMATwriter
    \internal
 */
struct TinyMATWriterFile {
    /* \brief the libc file handle */
    FILE* file;

    /* \brief specifies the byte order of the system (and the written file!) */
    uint8_t byteorder;
};


int TinyMATWriter_fOK(const TinyMATWriterFile* mat) {
    return (mat && (mat->file!=NULL));
}


/*! \brief determines the byte order of the system
    \ingroup tinymatwriter
    \internal

    \return TINYMAT_ORDER_BIGENDIAN or TINYMAT_ORDER_LITTLEENDIAN, or TINYMAT_ORDER_UNKNOWN if the byte order cannot be determined
 */
inline int TinyMAT_get_byteorder()
{
        union {
                long l;
                char c[4];
        } test;
        test.l = 1;
        if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
                return TINYMAT_ORDER_BIGENDIAN;

        if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
                return TINYMAT_ORDER_LITTLEENDIAN;

        return TINYMAT_ORDER_UNKNOWN;
}



/*! \brief write a 4-byte word \a data directly into a file \a fileno
    \ingroup TinyMATwriter
    \internal
 */
#define WRITE32DIRECT(filen, data)  { \
    fwrite((&(data)), 4, 1, filen); \
}

/*! \brief write a data word \a data , which is first cast into a 4-byte word directly into a file \a fileno
    \ingroup TinyMATwriter
    \internal
 */
#define WRITE32DIRECT_CAST(filen, data)  { \
    uint32_t d=data; \
    WRITE32DIRECT((filen), d); \
}




/*! \brief write a 2-byte word \a data directly into a file \a fileno
    \ingroup TinyMATwriter
    \internal
 */
#define WRITE16DIRECT(filen, data)    { \
    fwrite((&(data)), 2, 1, filen); \
}

/*! \brief write a data word \a data , which is first cast into a 2-byte word directly into a file \a fileno
    \ingroup TinyMATwriter
    \internal
 */
#define WRITE16DIRECT_CAST(filen, data)    { \
    uint16_t d=data; \
    WRITE16DIRECT((filen), d); \
}




/*! \brief write a data word \a data , which is first cast into a 1-byte word directly into a file \a fileno
    \ingroup TinyMATwriter
    \internal
 */
#define WRITE8DIRECT(filen, data) {\
    uint8_t ch=data; \
    fwrite(&ch, 1, 1, filen);\
}

inline void TinyMAT_write8(FILE* filen, uint8_t data) {
    if (!filen) return;
    fwrite(&data, 1, 1, filen);
}
inline void TinyMAT_write8(FILE* filen, int8_t data) {
    if (!filen) return;
    fwrite(&data, 1, 1, filen);
}
inline void TinyMAT_write16(FILE* filen, uint16_t data) {
    if (!filen) return;
    fwrite(&data, 2, 1, filen);
}
inline void TinyMAT_write16(FILE* filen, int16_t data) {
    if (!filen) return;
    fwrite(&data, 2, 1, filen);
}
inline void TinyMAT_write32(FILE* filen, uint32_t data) {
    if (!filen) return;
    long spos=ftell(filen);
    int ret=fwrite(&data, 4,1, filen);
    if ((ftell(filen)-spos)!=4) std::cout<<"!!!TinyMAT_write32u: "<<spos<<" "<<ftell(filen)<<" "<<(ftell(filen)-spos)<<" "<<ret<<"\n";
}
inline void TinyMAT_write32(FILE* filen, int32_t data) {
    if (!filen) return;
    long spos=ftell(filen);
    int ret=fwrite(&data, 4,1, filen);
    if ((ftell(filen)-spos)!=4) std::cout<<"!!!TinyMAT_write32s: "<<spos<<" "<<ftell(filen)<<" "<<(ftell(filen)-spos)<<" "<<ret<<"\n";
}
inline void TinyMAT_write64(FILE* filen, uint64_t data) {
    if (!filen) return;
    fwrite(&data, 8, 1, filen);
}
inline void TinyMAT_write64(FILE* filen, int64_t data) {
    if (!filen) return;
    fwrite(&data, 8, 1, filen);
}
inline void TinyMAT_write64d(FILE* filen, double data) {
    if (!filen) return;
    fwrite(&data, 8, 1, filen);
}
inline void TinyMAT_write32f(FILE* filen, float data) {
    if (!filen) return;
    fwrite(&data, 4, 1, filen);
}


#define TINYMAT_miINT8 1
#define TINYMAT_miUINT8 2
#define TINYMAT_miINT16 3
#define TINYMAT_miUINT16 4
#define TINYMAT_miINT32 5
#define TINYMAT_miUINT32 6
#define TINYMAT_miSINGLE 7
#define TINYMAT_miDOUBLE 9
#define TINYMAT_miINT64 12
#define TINYMAT_miUINT64 13
#define TINYMAT_miMATRIX 14
#define TINYMAT_miCOMPRESSED 15
#define TINYMAT_miUTF8 16
#define TINYMAT_miUTF16 17
#define TINYMAT_miUTF32 18





inline void TinyMAT_writeDatElement_u64(FILE* mat, uint64_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miUINT64);
	TinyMAT_write32(mat, (uint32_t)8);
	TinyMAT_write64(mat, data);
}

inline void TinyMAT_writeDatElement_i64(FILE* mat, int64_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miINT64);
	TinyMAT_write32(mat, (uint32_t)8);
	TinyMAT_write64(mat, data);
}

inline void TinyMAT_writeDatElement_u32(FILE* mat, uint32_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miUINT32);
	TinyMAT_write32(mat, (uint32_t)4);
	TinyMAT_write32(mat, data);
	TinyMAT_write32(mat, (uint32_t)0); // write padding
}

inline void TinyMAT_writeDatElement_i32(FILE* mat, int32_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miINT32);
	TinyMAT_write32(mat, (uint32_t)4);
	TinyMAT_write32(mat, data);
	TinyMAT_write32(mat, (uint32_t)0); // write padding
}

inline void TinyMAT_writeDatElement_u16(FILE* mat, uint16_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miUINT16);
	TinyMAT_write32(mat, (uint32_t)2);
	TinyMAT_write16(mat, data);
	TinyMAT_write16(mat, (uint16_t)0); // write padding
	TinyMAT_write32(mat, (uint32_t)0);
}

inline void TinyMAT_writeDatElement_i16(FILE* mat, int16_t data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miINT16);
	TinyMAT_write32(mat, (uint32_t)2);
	TinyMAT_write16(mat, data);
	TinyMAT_write16(mat, (uint16_t)0); // write padding
	TinyMAT_write32(mat, (uint32_t)0);
}

inline void TinyMAT_writeDatElement_dbl(FILE* mat, double data) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miDOUBLE);
	TinyMAT_write32(mat, (uint32_t)8);
	TinyMAT_write64d(mat, data);
	// no padding required
}
inline void TinyMAT_writeDatElement_dbla(FILE* mat, const double* data, size_t items) {
    //fpos_t p;
    //fgetpos(mat, &p);
    //std::cout<<"\n     TinyMAT_writeDatElement_dbla() "<<ftell(mat);
    long spos=ftell(mat);
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miDOUBLE);
    if (!data) items=0;
	TinyMAT_write32(mat, (uint32_t)(items*8));
//    if (data && items>0) {
//        std::cout<<"     TinyMAT_writeDatElement_dbla("<<data[0]<<items<<")"<<ftell(mat);
//    } else {
//        std::cout<<"     TinyMAT_writeDatElement_dbla("<<NULL<<items<<items*8<<")"<<ftell(mat);
//    }
    if (items>0 && data){
        fwrite(data, 8, items, mat);
    }
    //fgetpos(mat, &p);
    //std::cout<<"     TinyMAT_writeDatElement_dbla() "<<ftell(mat);

    // check for correct pos in file!
    if (ftell(mat)!=(int64_t)(spos+8+items*8)) {
        fseek(mat, spos+(int64_t)(8+items*8),SEEK_SET);
    }
	// no padding required
}
void TinyMAT_writeDatElement_dbla_rowmajor(FILE* mat, const double* data, size_t rows, size_t cols) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miDOUBLE);
	TinyMAT_write32(mat, (uint32_t)(rows*cols*8));
    if (rows*cols>0) {
        for (size_t c=0; c<cols; c++) {
            for (size_t r=0; r<rows; r++) {
                fwrite(&(data[r*cols+c]), 8, 1, mat);
            }
        }
    }
	// no padding required
}
void TinyMAT_writeDatElement_u32a(FILE* mat, const uint32_t* data, size_t items) {
	TinyMAT_write32(mat, (uint32_t)TINYMAT_miUINT32);
	TinyMAT_write32(mat, (uint32_t)(items*4));
    if (items>0) {
        fwrite(data, 4, items, mat);
        // write padding
        if (items%2==1) TinyMAT_write32(mat, (uint32_t)0);
    }
}

void TinyMAT_writeDatElement_string(FILE* mat, const char* data, size_t slen) {
    long spos=ftell(mat);
    size_t pad=slen%8;
    TinyMAT_write32(mat, (uint32_t)TINYMAT_miINT8);
    TinyMAT_write32(mat, (uint32_t)slen);
    if (slen>0) std::cout<<"TinyMAT_writeDatElement_string() slen="<<slen<<"  pad="<<pad<<"  spos="<<spos<<"\n";
    if (slen>0) {
        fwrite(data, 1, slen, mat);
        // write padding
        size_t pc=0;
        if (pad>0) {
            for (size_t i=pad; i<8; i++) {
                TinyMAT_write8(mat, (uint8_t)0);
                pc++;
                std::cout<<"  writePAD "<<i<<"/"<<pad<<" "<<ftell(mat)<<"\n";
            }

        }
        if (ftell(mat)!=long(spos+8+slen+pc)) {
            fseek(mat, spos+8+slen+pc,SEEK_SET);
            std::cout<<"  seek"<< spos+8+slen+pc<<" "<<slen<<" "<<pc<<"\n";
        }

    } else {
        if (ftell(mat)!=spos+8) {
            fseek(mat, spos+8,SEEK_SET);
            std::cout<<"  seek"<< spos+8<<"\n";
        }

    }

}

inline void TinyMAT_writeDatElement_string(FILE* mat, const char* data) {
    TinyMAT_writeDatElement_string(mat, data, strlen(data));
}

inline size_t TinyMAT_DatElement_realstringlen(const char* data) {
    if (!data) return 0;
	size_t slen=strlen(data);
    if (slen==0) return 0;
	size_t pad=slen%8;
	if (pad>0) {
		slen=slen+(8-pad);
	}
	return slen;
}







TinyMATWriterFile* TinyMATWriter_open(const char* filename, const char* description) {
	TinyMATWriterFile* mat=(TinyMATWriterFile*)malloc(sizeof(TinyMATWriterFile));

    mat->file=fopen(filename, "w");
    mat->byteorder=TinyMAT_get_byteorder();

    if (TinyMATWriter_fOK(mat)) {
		// setup and write Description field (116 bytes)
		const char stdmsg[]="MATLAB 5.0 MAT-file";
        char desc[116];
		size_t slstd=strlen(stdmsg);
		size_t sl=0;
		if (description) sl=strlen(description);
		if (slstd>0) {
			for (size_t i=0; i<slstd; i++) {
				desc[i]=stdmsg[i];
			}
		}
		size_t maxv=slstd;
		if (sl>0) {
		    maxv=maxv+2+sl;
		}
		if (maxv>116) maxv=116;
		if (sl>0) {
			desc[slstd]=':';
			desc[slstd+1]=' ';
			for (size_t i=slstd+2; i<sl; i++) {
				desc[i]=description[i];
			}
		}
		if (maxv<116) {
		    desc[maxv]='\0';
			for (size_t i=maxv+1; i<116; i++) {
				desc[i]=' ';
			}
		} else {
			desc[115]='\0';
		}
        fwrite(desc, 1, 116,mat->file);
		
		// write "Header Subsystem Data Offset Field" (not used, i.e. write 00000000)
		TinyMAT_write32(mat->file, (uint32_t)0x00000000);
		TinyMAT_write32(mat->file, (uint32_t)0x00000000);
		
		// write "Header Flag Fields"
		TinyMAT_write16(mat->file, (uint16_t)0x0100); // version
		TinyMAT_write8(mat->file, (int8_t)'I'); // endian indicator
		TinyMAT_write8(mat->file, (int8_t)'M');
		return mat;
    } else {
        free(mat);
        return NULL;
    }
}

#define TINYMAT_mxDOUBLE_CLASS_arrayflags 0x00000006
#define TINYMAT_mxCHAR_CLASS_CLASS_arrayflags 0x00000004
#define TINYMAT_mxCELL_CLASS_arrayflags 0x00000001

void TinyMATWriter_writeMatrix2D_colmajor(TinyMATWriterFile* mat, const char* name, const double* data_real, int32_t cols, int32_t rows) {
	uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxDOUBLE_CLASS_arrayflags, 0};
	
	
	size_bytes+=16; // array flags
	size_bytes+=16; // dimensions flags
	size_bytes+=8+TinyMAT_DatElement_realstringlen(name); // array name
    size_bytes+=8+8*cols*rows; // actual data
	
	// write tag header
	TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miMATRIX);
	TinyMAT_write32(mat->file, size_bytes);
	
	// write arrayflags
	TinyMAT_writeDatElement_u32a(mat->file, arrayflags, 2);

	// write field dimensions
	TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miINT32);
	TinyMAT_write32(mat->file, (uint32_t)8);
	TinyMAT_write32(mat->file, cols);
	TinyMAT_write32(mat->file, rows);
	
	// write field name
    TinyMAT_writeDatElement_string(mat->file, name);

	
	// write data type
	TinyMAT_writeDatElement_dbla(mat->file, data_real, cols*rows);
	
}



void TinyMATWriter_writeString(TinyMATWriterFile *mat, const char *name, const char *data)
{
    TinyMATWriter_writeString(mat, name, data, strlen(data));
}

void TinyMATWriter_writeString(TinyMATWriterFile *mat, const char *name, const char *data, size_t slen)
{
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxCHAR_CLASS_CLASS_arrayflags, 0};



    size_bytes+=16; // array flags
    size_bytes+=16; // dimensions flags
    size_bytes+=8+TinyMAT_DatElement_realstringlen(name); // array name
    size_bytes+=8+TinyMAT_DatElement_realstringlen(data); // actual data

    // write tag header
    TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miMATRIX);
    TinyMAT_write32(mat->file, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat->file, arrayflags, 2);

    // write field dimensions
    TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miINT32);
    TinyMAT_write32(mat->file, (uint32_t)8);
    TinyMAT_write32(mat->file, 1);
    TinyMAT_write32(mat->file, (uint32_t)slen);

    // write field name
    TinyMAT_writeDatElement_string(mat->file, name);


    // write data type
    TinyMAT_writeDatElement_string(mat->file, data, slen);
}


void TinyMATWriter_writeMatrix2D_rowmajor(TinyMATWriterFile* mat, const char* name, const double* data_real, int32_t cols, int32_t rows) {
	uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxDOUBLE_CLASS_arrayflags, 0};

	
	
	size_bytes+=16; // array flags
	size_bytes+=16; // dimensions flags
	size_bytes+=8+TinyMAT_DatElement_realstringlen(name); // array name
	size_bytes+=8+8*cols*rows; // actual data
	
	// write tag header
	TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miMATRIX);
	TinyMAT_write32(mat->file, size_bytes);
	
	// write arrayflags
	TinyMAT_writeDatElement_u32a(mat->file, arrayflags, 2);

	// write field dimensions
	TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miINT32);
	TinyMAT_write32(mat->file, (uint32_t)8);
	TinyMAT_write32(mat->file, cols);
	TinyMAT_write32(mat->file, rows);
	
	// write field name
    TinyMAT_writeDatElement_string(mat->file, name);

	// write data type
	TinyMAT_writeDatElement_dbla_rowmajor(mat->file, data_real, rows, cols);
	
}



void TinyMATWriter_close(TinyMATWriterFile* mat) {
	if (mat) {
		if (mat->file) fclose(mat->file);
		free(mat);
	}
}


#ifdef TINYMAT_USES_QVARIANT
    void TinyMATWriter_writeQVariantList(TinyMATWriterFile *mat, const char *name, const QVariantList &data)
    {
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};





        // write tag header
        TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=ftell(mat->file);
        TinyMAT_write32(mat->file, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat->file, arrayflags, 2);

        // write field dimensions
        TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miINT32);
        TinyMAT_write32(mat->file, (uint32_t)8);
        TinyMAT_write32(mat->file, (int32_t)1);
        TinyMAT_write32(mat->file, (int32_t)data.size());

        // write field name
        TinyMAT_writeDatElement_string(mat->file, name);

        // write data type
        for (int i=0; i<data.size(); i++) {
            if (data[i].type()==QVariant::String) {
                QByteArray a=data[i].toString().toLatin1();
                TinyMATWriter_writeString(mat, "", a.data(), a.size());
            } else if (data[i].canConvert(QVariant::Double)) {
                double a=data[i].toDouble();
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", &a, 1, 1);
            } else if (data[i].canConvert(QVariant::PointF)) {
                double a[2]={data[i].toPointF().x(), data[i].toPointF().y()};
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
            } else if (data[i].canConvert(QVariant::Point)) {
                double a[2]={(double)data[i].toPoint().x(), (double)data[i].toPoint().y()};
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
            } else if (data[i].canConvert(QVariant::SizeF)) {
                double a[2]={data[i].toSizeF().width(), data[i].toSizeF().height()};
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
            } else if (data[i].canConvert(QVariant::Size)) {
                double a[2]={(double)data[i].toSize().width(), (double)data[i].toSize().height()};
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
            } else if (data[i].canConvert(QVariant::String)) {
                QByteArray a=data[i].toString().toLatin1();
                TinyMATWriter_writeString(mat, "", a.data(), a.size());
            } else {
                TinyMATWriter_writeMatrix2D_colmajor(mat, "", NULL, 0, 0);
            }
        }

        long endpos=ftell(mat->file);
        fseek(mat->file, sizepos, SEEK_SET);
        size_bytes=endpos-sizepos-4;
        TinyMAT_write32(mat->file, size_bytes);
        fseek(mat->file, endpos, SEEK_SET);
    }


    void TinyMATWriter_writeQVariantMatrix(TinyMATWriterFile *mat, const char *name, const QList<QList<QVariant> > &data)
    {
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};


        int32_t maxrows=0;
        for (int i=0; i<data.size(); i++) {
            for (int j=0; j<data[i].size(); j++) {
                maxrows=qMax(maxrows, data[i].size());
            }
        }


        // write tag header
        TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miMATRIX);
        long sizepos;
        sizepos=ftell(mat->file);
        TinyMAT_write32(mat->file, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat->file, arrayflags, 2);

        // write field dimensions
        TinyMAT_write32(mat->file, (uint32_t)TINYMAT_miINT32);
        TinyMAT_write32(mat->file, (uint32_t)8);
        TinyMAT_write32(mat->file, (int32_t)maxrows);
        TinyMAT_write32(mat->file, (int32_t)data.size());

        // write field name
        TinyMAT_writeDatElement_string(mat->file, name);

        // write data type
        for (int i=0; i<data.size(); i++) {
            for (int j=0; j<maxrows; j++) {
                QVariant v;
                if (j<data[i].size()) {
                    v=data[i].operator[](j);
                }
                if (v.type()==QVariant::String) {
                    QByteArray a=v.toString().toLatin1();
                    std::cout<<i<<" "<<j<<" "<<QString(a).toStdString()<<"\n";
                    TinyMATWriter_writeString(mat, "", a.data(), a.size());
                } else if (v.canConvert(QVariant::Double)) {
                    double a=v.toDouble();
                    std::cout<<i<<" "<<j<<" "<<a<<"\n";
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", &a, 1, 1);
                } else if (v.canConvert(QVariant::PointF)) {
                    double a[2]={v.toPointF().x(), v.toPointF().y()};
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
                } else if (v.canConvert(QVariant::Point)) {
                    double a[2]={(double)v.toPoint().x(), (double)v.toPoint().y()};
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
                } else if (v.canConvert(QVariant::SizeF)) {
                    double a[2]={v.toSizeF().width(), v.toSizeF().height()};
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
                } else if (v.canConvert(QVariant::Size)) {
                    double a[2]={(double)v.toSize().width(), (double)v.toSize().height()};
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", a, 1, 2);
                } else if (v.canConvert(QVariant::String)) {
                    QByteArray a=v.toString().toLatin1();
                    TinyMATWriter_writeString(mat, "", a.data(), a.size());
                } else {
                    std::cout<<i<<" "<<j<<" "<<"EMPTY"<<"\n";
                    TinyMATWriter_writeMatrix2D_colmajor(mat, "", NULL, 0, 0);
                }
                std::cout<<"### "<<ftell(mat->file)<<" "<<(ftell(mat->file)%8)<<"\n";
            }
        }

        long endpos;
        endpos=ftell(mat->file);
        fseek(mat->file, sizepos, SEEK_SET);
        //fsetpos(mat->file, &sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_write32(mat->file, size_bytes);
        fseek(mat->file, endpos, SEEK_SET);
        //fsetpos(mat->file, &endpos);
        std::cout<<endpos<<" "<<ftell(mat->file)<<"\n";
    }

#endif


