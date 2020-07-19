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
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <list>
#include <algorithm>

//#include <iostream>

#include "tinymatwriter.h"

/** \brief if defined, files are beeing created in a memory buffer and are only written to disk at the end. 
*          if undefined, the files are written directly to disk, including move operations on disk, which can be a factor 2-3 slower. */
//#define TINYMAT_WRITE_VIA_MEMORY

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
//#  include <QDebug>
#  include <QPoint>
#  include <QSize>
#endif

#define TINYMAT_ORDER_UNKNOWN 0
#define TINYMAT_ORDER_BIGENDIAN 1
#define TINYMAT_ORDER_LITTLEENDIAN 2


#if defined(__GNUC__) || defined(__GNUG__)
#define TINYMAT_inlineattrib  inline
// use this for no-inline:
//#define TINYMAT_inlineattrib  __attribute__((noinline))
#else
#define TINYMAT_inlineattrib inline
#endif

#define TINYMAT_mxCHAR_CLASS_CLASS_arrayflags 0x00000004
#define TINYMAT_mxDOUBLE_CLASS_arrayflags 0x00000006
#define TINYMAT_mxSINGLE_CLASS_arrayflags 0x00000007
#define TINYMAT_mxINT8_CLASS_arrayflags 0x00000008
#define TINYMAT_mxUINT8_CLASS_arrayflags 0x00000009
#define TINYMAT_mxINT16_CLASS_arrayflags 0x0000000A
#define TINYMAT_mxUINT16_CLASS_arrayflags 0x0000000B
#define TINYMAT_mxINT32_CLASS_arrayflags 0x0000000C
#define TINYMAT_mxUINT32_CLASS_arrayflags 0x0000000D
#define TINYMAT_mxINT64_CLASS_arrayflags 0x0000000E
#define TINYMAT_mxUINT64_CLASS_arrayflags 0x0000000F
#define TINYMAT_mxUINT8_LOGICAL_CLASS_arrayflags (TINYMAT_mxUINT8_CLASS_arrayflags+(0x0002<<8))


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

struct TinyMATWriterStruct {
  inline TinyMATWriterStruct() :
    sizepos(-1),
    data_start(-1)
  {
  }
  /** \brief position of the size-data field */
  long sizepos;
  long data_start;
  std::vector<std::string> itemnames;
};

struct TinyMATWriterCell {
  inline TinyMATWriterCell() :
    sizepos(-1),
    data_start(-1)
  {
  }
  /** \brief position of the size-data field */
  long sizepos;
  long data_start;
};

enum class TinyMATWriterStackItem {
  Cell,
  Struct
};

/*! \brief this struct represents a mat file
    \ingroup TinyMATwriter
    \internal
 */
struct TinyMATWriterFile {
    TinyMATWriterFile() :
      file(NULL),
      filedata(NULL),
      filedata_size(0),
      filedata_current(0),
      filedata_count(0),
      byteorder(TINYMAT_ORDER_UNKNOWN)
    {
    }

    /** \brief the libc file handle */
    FILE* file;
    /** \brief Zwischenspeicher-Array beim Schreiben von Matlab-Daten */
    uint8_t* filedata;
    /** \brief Größe von filedata */
    size_t filedata_size;
    /** \brief aktuelle Position in filedata */
    size_t filedata_current;
    /** \brief tatsächliche Datenbytes in filedata */
    size_t filedata_count;

    /** \brief specifies the byte order of the system (and the written file!) */
    uint8_t byteorder;

    std::vector<TinyMATWriterStruct> structures;
    std::vector<TinyMATWriterCell> cells;
    std::vector<TinyMATWriterStackItem> stack;

    inline void startStruct() {
      structures.push_back(TinyMATWriterStruct());
      stack.push_back(TinyMATWriterStackItem::Struct);
    }

    void endStruct() {
      structures.pop_back();
      stack.pop_back();
    }

    inline TinyMATWriterStruct& lastStruct() {
      return structures[structures.size()-1];
    }

    inline void addStructItemName(const std::string& name) {
      if (structures.size()>0 && stack.size()>0 && stack[stack.size()-1]==TinyMATWriterStackItem::Struct) {
        lastStruct().itemnames.push_back(name);
      }
    }

    inline void startCell() {
      cells.push_back(TinyMATWriterCell());
      stack.push_back(TinyMATWriterStackItem::Cell);
    }

    void endCell() {
      cells.pop_back();
      stack.pop_back();
    }

    inline TinyMATWriterCell& lastCell() {
      return cells[cells.size()-1];
    }
    
};


int TinyMATWriter_fOK(const TinyMATWriterFile* mat)  {
    return (mat && (mat->file!=NULL));
}


/*! \brief determines the byte order of the system
    \ingroup tinymatwriter
    \internal

    \return TINYMAT_ORDER_BIGENDIAN or TINYMAT_ORDER_LITTLEENDIAN, or TINYMAT_ORDER_UNKNOWN if the byte order cannot be determined
 */
 TINYMAT_inlineattrib static int TinyMAT_get_byteorder()
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


 TINYMAT_inlineattrib static int TinyMAT_fclose(TinyMATWriterFile* file) {
     //std::cout<<"TinyMAT_fclose()\n";
     //std::cout.flush();
     if (!file) return 0;
     if (!file->file) {
       delete file;
       return 0;
     }
#ifdef TINYMAT_WRITE_VIA_MEMORY
     if (file->filedata_count>0 && file->filedata) {
       fseek(file->file, 0, SEEK_SET);
       fwrite(file->filedata, 1, file->filedata_count, file->file);
       file->filedata_size = 0;
       file->filedata_current = 0;
       file->filedata_count = 0;
       free(file->filedata);
     }
#endif
     int ret= fclose(file->file);
     delete file;
     return ret;
 }

 TINYMAT_inlineattrib static TinyMATWriterFile* TinyMAT_fopen(const char* filename, size_t bufSize=1024*100) {
     //std::cout<<"TinyMAT_fopen()\n";
     //std::cout.flush();
     TinyMATWriterFile* mat=new TinyMATWriterFile;

     if (fopen_s(&(mat->file), filename, "wb+") == 0) {
       if (mat->file) {
         if (bufSize > 0) {
           setvbuf(mat->file, NULL, _IOFBF, bufSize);
         }
         else {
           setvbuf(mat->file, NULL, _IOFBF, BUFSIZ);
         }
       }
       mat->byteorder = (uint8_t)TinyMAT_get_byteorder();
     }
#ifdef TINYMAT_WRITE_VIA_MEMORY
      mat->filedata_size = std::max<size_t>(bufSize, BUFSIZ);
      mat->filedata_current = 0;
      mat->filedata_count = 0;
      mat->filedata = (uint8_t*)malloc(mat->filedata_size);
      if (!mat->filedata) {
        delete mat;
        return NULL;
      }
#endif
     return mat;
 }

  TINYMAT_inlineattrib static long TinyMAT_ftell(TinyMATWriterFile* file) {
     //std::cout<<"TinyMAT_ftell()\n";
     //std::cout.flush();
     if (!file || !file->file) return 0;
#ifdef TINYMAT_WRITE_VIA_MEMORY
     return static_cast<long>(file->filedata_current);
#else
     return ftell(file->file);
#endif
 }
 TINYMAT_inlineattrib static int TinyMAT_fseek(TinyMATWriterFile* file, long offset) {
     //std::cout<<"TinyMAT_fseek()\n";
     //std::cout.flush();
     if (!file || !file->file) return 0;
#ifdef TINYMAT_WRITE_VIA_MEMORY
       long start = 0;
       int res = 0;
       if (start + offset < 0) {
         throw std::runtime_error("seek before start of file");
         res=-1;
       } else if (static_cast<size_t>(start + offset) > file->filedata_count) {
         throw std::runtime_error("seek after end of file");
         res=-1;
       } else {
         file->filedata_current = start + offset;
         res=0;
       }
       return res;
#else
       return fseek(file->file, offset, SEEK_SET);
#endif
 }

 /** \brief grows the internal memory array for file writing by \a size_increment bytes */
 TINYMAT_inlineattrib static void TinyMAT_growMem(uint32_t size_increment, TinyMATWriterFile* file) {
#ifdef TINYMAT_WRITE_VIA_MEMORY
   if (file->filedata_current + size_increment + 100 >= file->filedata_size) {
     size_t newsize = file->filedata_size;
     while (file->filedata_current + size_increment + 100 >= newsize) {
       if (newsize < 100 * 1024 * 1024) newsize = newsize * 2;
       else if (newsize < 1000 * 1024 * 1024) newsize = newsize * 3 / 2;
       else newsize = newsize * 6 / 5;
     }
     file->filedata = (uint8_t*)realloc(file->filedata, newsize);
     file->filedata_size = newsize;
   }
#endif
 }


TINYMAT_inlineattrib static int TinyMAT_fwrite(const void* data, uint32_t size, uint32_t count, TinyMATWriterFile* file)
{
     //std::cout<<"TinyMAT_fwrite()\n";
     if (!file || !file->file || !data || size*count<=0) return 0;
     int res = 0;
#ifdef TINYMAT_WRITE_VIA_MEMORY
       if (file->filedata_current + size*count + 100 >= file->filedata_size) {
         TinyMAT_growMem(size*count, file);
       }
       memcpy_s(&(file->filedata[file->filedata_current]), file->filedata_size- file->filedata_current, data, size*count);
       file->filedata_current = file->filedata_current + size*count;
       file->filedata_count = std::max(file->filedata_count, file->filedata_current);
       res=size*count;
#else
       res = (int)fwrite(data, 1, size*count, file->file);
#endif
     return res;
}

template<typename T>
TINYMAT_inlineattrib static int TinyMAT_fwritesmall(T data, TinyMATWriterFile* file)
{
     if (!file || !file->file) return 0;
     int res = 0;
#ifdef TINYMAT_WRITE_VIA_MEMORY
     if (file->filedata_current + sizeof(T) + 100 >= file->filedata_size) {
       TinyMAT_growMem(sizeof(T), file);
     }
     T* datap = reinterpret_cast<T*>(&(file->filedata[file->filedata_current]));
     *datap = data;
     file->filedata_current = file->filedata_current + sizeof(T);
     file->filedata_count = std::max(file->filedata_count, file->filedata_current);
     res=sizeof(T);
#else
     res = (int)fwrite(&data, 1, sizeof(T), file->file);
#endif
     return res;
}

TINYMAT_inlineattrib static int TinyMAT_fread(void* data, uint32_t size, uint32_t count, TinyMATWriterFile* file)
{
     //std::cout<<"TinyMAT_fwrite()\n";
     if (!file || !file->file || !data || size*count<=0) return 0;
     int res = 0;
#ifdef TINYMAT_WRITE_VIA_MEMORY
       int cnt = std::min<int>(size*count, static_cast<int>(file->filedata_size - file->filedata_current));
       if (static_cast<uint32_t>(cnt) != size*count) {
         throw std::runtime_error("read after end of file");
       }
       memcpy_s(data, size*count, &(file->filedata[file->filedata_current]), cnt);
       file->filedata_current = file->filedata_current + cnt;
       res = cnt;
#else
       res = (int)fread(data, 1, size*count, file->file);
#endif
     return res;
}



TINYMAT_inlineattrib static void TinyMAT_writeU8(TinyMATWriterFile* filen, uint8_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write8(TinyMATWriterFile* filen, int8_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_writeU16(TinyMATWriterFile* filen, uint16_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write16(TinyMATWriterFile* filen, int16_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_writeU32(TinyMATWriterFile* filen, uint32_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write32(TinyMATWriterFile* filen, int32_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_writeU64(TinyMATWriterFile* filen, uint64_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write64(TinyMATWriterFile* filen, int64_t data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write64d(TinyMATWriterFile* filen, double data) {
    TinyMAT_fwritesmall(data, filen);
}
TINYMAT_inlineattrib static void TinyMAT_write32f(TinyMATWriterFile* filen, float data) {
    TinyMAT_fwritesmall(data, filen);
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u64(TinyMATWriterFile* mat, uint64_t data) {
    TinyMAT_write32(mat, static_cast<uint32_t>(TINYMAT_miUINT64));
    TinyMAT_write32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write64(mat, data);
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i64(TinyMATWriterFile* mat, int64_t data) {
    TinyMAT_write32(mat, static_cast<uint32_t>(TINYMAT_miINT64));
    TinyMAT_write32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write64(mat, data);
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u32(TinyMATWriterFile* mat, uint32_t data) {
    TinyMAT_write32(mat, static_cast<uint32_t>(TINYMAT_miUINT32));
    TinyMAT_write32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write32(mat, data);
    TinyMAT_write32(mat, static_cast<uint32_t>(0)); // write padding
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i32(TinyMATWriterFile* mat, int32_t data) {
    TinyMAT_write32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_write32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write32(mat, data);
    TinyMAT_write32(mat, static_cast<uint32_t>(0)); // write padding
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElementS_i32(TinyMATWriterFile* mat, int32_t data) {
    TinyMAT_writeU16(mat, static_cast<uint16_t>(TINYMAT_miINT32));
    TinyMAT_writeU16(mat, static_cast<uint16_t>(sizeof(data)));
    TinyMAT_write32(mat, data);
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u16(TinyMATWriterFile* mat, uint16_t data) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miUINT16));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_writeU16(mat, data);
    TinyMAT_writeU16(mat, static_cast<uint16_t>(0)); // write padding
    TinyMAT_write32(mat, static_cast<uint32_t>(0));
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i16(TinyMATWriterFile* mat, int16_t data) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT16));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write16(mat, data);
    TinyMAT_writeU16(mat, static_cast<uint16_t>(0)); // write padding
    TinyMAT_write32(mat, static_cast<uint32_t>(0));
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_dbl(TinyMATWriterFile* mat, double data) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miDOUBLE));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(sizeof(data)));
    TinyMAT_write64d(mat, data);
    // no padding required
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_dbla(TinyMATWriterFile* mat, const double* data, uint32_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miDOUBLE));
    if (!data) items=0;
    TinyMAT_writeU32(mat, items*sizeof(*data));
    if (items>0 && data){
        TinyMAT_fwrite(data, sizeof(*data), items, mat);
    }
    // no padding required
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_flta(TinyMATWriterFile* mat, const float* data, uint32_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miSINGLE));
    if (!data) items=0;
    TinyMAT_writeU32(mat, items*sizeof(*data));
    if (items>0 && data){
        TinyMAT_fwrite(data, sizeof(*data), items, mat);
        // write padding
        if (items%2==1) TinyMAT_write32(mat, static_cast<uint32_t>(0));
    }
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u32a(TinyMATWriterFile* mat, const uint32_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miUINT32));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // write padding
        if (items%2==1) TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
    }
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i32a(TinyMATWriterFile* mat, const int32_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // write padding
        if (items%2==1) TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
    }
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u16a(TinyMATWriterFile* mat, const uint16_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miUINT16));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // write padding
        if (items%4==1) {
            TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
            TinyMAT_writeU16(mat, static_cast<uint16_t>(0));
        } else if (items%4==2) {
            TinyMAT_write32(mat, static_cast<uint32_t>(0));
        } else if (items%4==3) {
            TinyMAT_writeU16(mat, static_cast<uint16_t>(0));
        }

    }
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i16a(TinyMATWriterFile* mat, const int16_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT16));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // write padding
        if (items%4==1) {
            TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
            TinyMAT_writeU16(mat, static_cast<uint16_t>(0));
        } else if (items%4==2) {
            TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
        } else if (items%4==3) {
            TinyMAT_writeU16(mat, static_cast<uint16_t>(0));
        }
    }
}


TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u64a(TinyMATWriterFile* mat, const uint64_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miUINT64));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // no padding required
    }
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i64a(TinyMATWriterFile* mat, const int64_t* data, size_t items) {
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT64));
    TinyMAT_writeU32(mat, static_cast<uint32_t>(items*sizeof(*data)));
    if (items>0) {
        TinyMAT_fwrite(data, sizeof(*data), (uint32_t)items, mat);
        // no padding required
    }
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_i8a(TinyMATWriterFile* mat, const int8_t* data, uint32_t slen) {
    size_t pad=(slen)%8;
    uint32_t cla=TINYMAT_miINT8;
    TinyMAT_writeU32(mat, cla);
    TinyMAT_writeU32(mat, slen);
    if (slen>0 && data) {
        TinyMAT_fwrite(data, 1, slen, mat);
        if (pad>0) {
          static const uint8_t paddata[8] = { 0,0,0,0,0,0,0,0 };
          TinyMAT_fwrite(paddata, static_cast<uint32_t>(8 - pad), 1, mat);
        }
    }
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_u8a(TinyMATWriterFile* mat, const uint8_t* data, uint32_t slen) {
    size_t pad=(slen)%8;
    uint32_t cla=TINYMAT_miUINT8;
    TinyMAT_writeU32(mat, cla);
    TinyMAT_writeU32(mat, slen);
    if (slen>0 && data) {
        TinyMAT_fwrite(data, 1, slen, mat);
        if (pad>0) {
          static const uint8_t paddata[8] = { 0,0,0,0,0,0,0,0 };
          TinyMAT_fwrite(paddata, static_cast<uint32_t>(8 - pad), 1, mat);
        }
    }
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_stringas8bit(TinyMATWriterFile* mat, const char* data, uint32_t slen) {
    TinyMAT_writeDatElement_i8a(mat, (const int8_t*)(data), slen);
}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_string(TinyMATWriterFile* mat, const char* data, uint32_t slen) {
    size_t pad=(2*slen)%8;
    int16_t* tmp=NULL;
    if (slen>0 && data) {
        tmp=(int16_t*)malloc(slen*2);
        for (uint32_t i=0; i<slen; i++) {
            tmp[i]=data[i];
        }
    }
    uint32_t cla=TINYMAT_miUINT16;
    TinyMAT_writeU32(mat, cla);
    TinyMAT_writeU32(mat, slen*2);
    if (slen>0 && tmp) {
        TinyMAT_fwrite(tmp, 2, slen, mat);
        // write padding
        if (pad>0) {
          static const uint8_t paddata[8] = { 0,0,0,0,0,0,0,0 };
          TinyMAT_fwrite(paddata, static_cast<uint32_t>(8 - pad), 1, mat);
        }
    } else {

    }
    if (tmp) free(tmp);

}

TINYMAT_inlineattrib static void TinyMAT_writeDatElement_stringas8bit(TinyMATWriterFile* mat, const char* data) {
    TinyMAT_writeDatElement_stringas8bit(mat, data, (uint32_t)strlen(data));
}
TINYMAT_inlineattrib static void TinyMAT_writeDatElement_string(TinyMATWriterFile* mat, const char* data) {
    TinyMAT_writeDatElement_string(mat, data, (uint32_t)strlen(data));
}

TINYMAT_inlineattrib static size_t TinyMAT_DatElement_realstringlen8bit(const char* data) {
    if (!data) return 0;
    size_t slen=strlen(data);
    if (slen==0) return 0;
    size_t pad=slen%8;
    if (pad>0) {
        slen=slen+(8-pad);
    }
    return slen;
}
TINYMAT_inlineattrib static size_t TinyMAT_DatElement_realstringlen16bit(const char* data) {
    if (!data) return 0;
    size_t slen=strlen(data)*2;
    if (slen==0) return 0;
    size_t pad=slen%8;
    if (pad>0) {
        slen=slen+(8-pad);
    }
    return slen;
}











void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const double *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxDOUBLE_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_dbla(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const float *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxSINGLE_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_flta(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const uint64_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxUINT64_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_u64a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const int64_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxINT64_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_i64a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}




void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const uint32_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxUINT32_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_u32a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const int32_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxINT32_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_i32a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}



void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const uint16_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxUINT16_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_u16a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const int16_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxINT16_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_i16a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}




void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const uint8_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxUINT8_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_u8a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const int8_t *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxINT8_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        TinyMAT_writeDatElement_i8a(mat, data_real, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }
}

void TinyMATWriter_writeMatrixND_colmajor(TinyMATWriterFile *mat, const char *name, const bool *data_real, const int32_t *sizes, uint32_t ndims)
{
    if (!data_real || !sizes || ndims<=0) {
        TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
        mat->addStructItemName(name);
        uint32_t nentries=0;
        for (uint32_t i=0; i<ndims; i++) {
            if (i==0) {
                nentries=sizes[0];
            } else {
                nentries=nentries*sizes[i];
            }
        }

        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxUINT8_LOGICAL_CLASS_arrayflags, 0};


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        int8_t* dat=NULL;
        if (nentries>0) {
            dat=(int8_t*)malloc(nentries*sizeof(int8_t*));
            for (uint32_t i=0; i<nentries; i++) {
                dat[i]=(data_real[i]?1:0);
            }
        }
        TinyMAT_writeDatElement_i8a(mat, dat, nentries);
        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
        if (dat) free(dat);
    }
}


TinyMATWriterFile* TinyMATWriter_open(const char* filename, const char* description, size_t bufSize) {
    TinyMATWriterFile* mat=TinyMAT_fopen(filename, bufSize);

    if (TinyMATWriter_fOK(mat)) {
        // setup and write Description field (116 bytes)
        char stdmsg[512];
        for (int i=0; i<512; i++) stdmsg[i]='\0';
        time_t rawtime;
        ::time(&rawtime);
        struct tm timeinfo;
        ::gmtime_s(&timeinfo, &rawtime);
        sprintf_s(stdmsg, 512,"MATLAB 5.0 MAT-file, written by TinyMAT, %d-%02d-%02d %02d:%02d:%02d UTC", 1900+timeinfo.tm_year, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        size_t slstd=strlen(stdmsg);
        char desc[116];
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
        TinyMAT_fwrite(desc, 1, 116,mat);
        
        // write "Header Subsystem Data Offset Field" (not used, i.e. write 00000000)
        TinyMAT_writeU32(mat, static_cast<uint32_t>(0x00000000));
        TinyMAT_writeU32(mat, static_cast<uint32_t>(0x00000000));
        
        // write "Header Flag Fields"
        TinyMAT_writeU16(mat, static_cast<uint16_t>(0x0100)); // version
        TinyMAT_write8(mat, (int8_t)'I'); // endian indicator
        TinyMAT_write8(mat, (int8_t)'M');
        return mat;
    } else {
        delete mat;
        return NULL;
    }
}

#define TINYMAT_mxCELL_CLASS_arrayflags 0x00000001
#define TINYMAT_mxSTRUCT_CLASS_arrayflags 0x00000002


void TinyMATWriter_writeDoubleList(TinyMATWriterFile *mat, const char *name, const std::list<double> &data, bool columnVector)
{
    mat->addStructItemName(name);
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxDOUBLE_CLASS_arrayflags, 0};


    size_bytes+=16; // array flags
    size_bytes+=16; // dimensions flags
    size_bytes+=8+(uint32_t)TinyMAT_DatElement_realstringlen8bit(name); // array name
    size_bytes+=8+8*(uint32_t)data.size(); // actual data

    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    if (columnVector) {
        TinyMAT_writeU32(mat, (uint32_t)1);
        TinyMAT_writeU32(mat, (uint32_t)data.size());
    } else {
        TinyMAT_writeU32(mat, (uint32_t)data.size());
        TinyMAT_writeU32(mat, (uint32_t)1);
    }

    // write field name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    double * d=NULL;
    if (data.size()>0) {
        d=(double*)malloc(data.size()*sizeof(double));
        int i=0;
        for (std::list<double>::const_iterator it=data.begin(); it!=data.end(); it++) {
            d[i]=*it;
            i++;
        }
    }

    // write data type
    TinyMAT_writeDatElement_dbla(mat, d, (uint32_t)data.size());
    if (d) free(d);
}


void TinyMATWriter_writeDoubleVector(TinyMATWriterFile *mat, const char *name, const std::vector<double> &data, bool columnVector)
{
    mat->addStructItemName(name);
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxDOUBLE_CLASS_arrayflags, 0};


    size_bytes+=16; // array flags
    size_bytes+=16; // dimensions flags
    size_bytes+=8+(uint32_t)TinyMAT_DatElement_realstringlen8bit(name); // array name
    size_bytes+=8+8*(uint32_t)data.size(); // actual data

    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    if (columnVector) {
        TinyMAT_writeU32(mat, (uint32_t)1);
        TinyMAT_writeU32(mat, (uint32_t)data.size());
    } else {
        TinyMAT_writeU32(mat, (uint32_t)data.size());
        TinyMAT_writeU32(mat, (uint32_t)1);
    }

    // write field name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    double * d=NULL;
    if (data.size()>0) {
        d=(double*)malloc(data.size()*sizeof(double));
        int i=0;
        for (std::vector<double>::const_iterator it=data.begin(); it!=data.end(); it++) {
            d[i]=*it;
            i++;
        }
    }

    // write data type
    TinyMAT_writeDatElement_dbla(mat, d, (uint32_t)data.size());
    if (d) free(d);
}




void TinyMATWriter_writeEmptyMatrix(TinyMATWriterFile *mat, const char *name)
{

  mat->addStructItemName(name);
  uint32_t size_bytes = 0;
  uint32_t arrayflags[2] = { TINYMAT_mxDOUBLE_CLASS_arrayflags, 0 };


  size_bytes += 16; // array flags
  size_bytes += 16; // dimensions flags
  size_bytes += 8 + (uint32_t)TinyMAT_DatElement_realstringlen8bit(name); // array name
  size_bytes += 8 + 8 * 0; // actual data

                           // write tag header
  TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
  TinyMAT_writeU32(mat, size_bytes);

  // write arrayflags
  TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

  // write field dimensions
  TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
  TinyMAT_writeU32(mat, (uint32_t)8);
  TinyMAT_writeU32(mat, static_cast<uint32_t>(0));
  TinyMAT_writeU32(mat, static_cast<uint32_t>(0));

  // write field name
  TinyMAT_writeDatElement_stringas8bit(mat, name);

  // write no-double-data element
  TinyMAT_writeDatElement_dbla(mat, NULL, 0);

}

void TinyMATWriter_writeString(TinyMATWriterFile *mat, const char *name, const char *data)
{
    TinyMATWriter_writeString(mat, name, data, (uint32_t)strlen(data));
}


void TinyMATWriter_writeString(TinyMATWriterFile *mat, const char *name, const std::string &data)
{
    TinyMATWriter_writeString(mat, name, data.c_str(), (uint32_t)data.size());
}

void TinyMATWriter_writeString(TinyMATWriterFile *mat, const char *name, const char *data, uint32_t slen)
{
    mat->addStructItemName(name);
    uint32_t size_bytes=0;
    uint32_t arrayflags[2];
    arrayflags[0]=TINYMAT_mxCHAR_CLASS_CLASS_arrayflags;
    arrayflags[1]=0;



    size_bytes+=16; // array flags
    size_bytes+=16; // dimensions flags
    size_bytes+=8+(uint32_t)TinyMAT_DatElement_realstringlen8bit(name); // array name
    size_bytes+=8+(uint32_t)TinyMAT_DatElement_realstringlen16bit(data); // actual data

    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    TinyMAT_writeU32(mat, 1);
    TinyMAT_writeU32(mat, slen);

    // write field name
    TinyMAT_writeDatElement_stringas8bit(mat, name);


    // write data type
    TinyMAT_writeDatElement_string(mat, data, slen);
}





void TinyMATWriter_close(TinyMATWriterFile* mat) {
    if (mat) {
        while (mat->structures.size()>0) {
            TinyMATWriter_endStruct(mat);
        }
        if (mat) TinyMAT_fclose(mat);
    }
}

std::string TinyMAT_combineStrings(const std::vector<std::string>& fieldnames, int32_t* maxlen_out=NULL, int32_t minlen=32) {
    std::vector<std::string> names;
    int32_t maxlen=0;
    for (auto i = fieldnames.begin(); i != fieldnames.end(); ++i) {
        std::string ni=*i;
        while (static_cast<int32_t>(ni.size())>(minlen-1)) {
            ni.erase(ni.size()-1, 1);
        }
        if ((int32_t)ni.size()>maxlen) maxlen=(int32_t)ni.size();
        names.push_back(ni);
    }
    if (maxlen<minlen) maxlen=minlen;
    // pad all field names to maxlen
    std::string joinednames;
    for (size_t ii=0; ii<names.size(); ii++) {
        while ((int64_t)names[ii].size()<maxlen) {
            names[ii].push_back('\0');
        }
        joinednames.append(names[ii]);
    }
    if (maxlen_out) *maxlen_out=maxlen;
    return joinednames;
}



void TinyMATWriter_startStruct(TinyMATWriterFile *mat, const char *name) {
    mat->addStructItemName(name);
    mat->startStruct();

    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxSTRUCT_CLASS_arrayflags, 0};
    


    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    mat->lastStruct().sizepos=TinyMAT_ftell(mat);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    TinyMAT_write32(mat, (int32_t)1);
    TinyMAT_write32(mat, (int32_t)1);

    // write struct name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    mat->lastStruct().data_start=TinyMAT_ftell(mat);
}


void TinyMATWriter_endStruct(TinyMATWriterFile* mat) {
    /*
        This function is a bit mean:
          It first reads the data written in the array so far (since the associated call to TinyMATWriter_startStruct(mat, name))
          into a memory array. Then it takes the collected field names from the uppermost TinyMATWriterStruct item in the
          structures-stack of TinyMATWriterFile, writes these names as item-names for the struct and rewrites the temporarily
          stored data. This way, the API can internally collect the item names, which have to be put into the file BEFORE the
          actual data.
    */
    TinyMATWriterStruct& struc=mat->lastStruct();

    long start=TinyMAT_ftell(mat);
    long ssize=start-struc.data_start;
    uint8_t* tmpdata=(uint8_t*)malloc(ssize*sizeof(uint8_t));
    TinyMAT_fseek(mat, struc.data_start);
    TinyMAT_fread(tmpdata, ssize, 1, mat);


    int32_t maxlen=0;
    std::string joinednames=TinyMAT_combineStrings(struc.itemnames, &maxlen);


    TinyMAT_fseek(mat, struc.data_start);
    // write field name length
    TinyMAT_writeDatElementS_i32(mat, maxlen);

    // write field names
    TinyMAT_writeDatElement_stringas8bit(mat, joinednames.c_str(), (uint32_t)joinednames.size());
    
    TinyMAT_fwrite(tmpdata, ssize, 1, mat);

    free(tmpdata);


    long endpos=TinyMAT_ftell(mat);
    TinyMAT_fseek(mat, struc.sizepos);
    uint32_t size_bytes=endpos-struc.sizepos-4;
    TinyMAT_writeU32(mat, size_bytes);
    TinyMAT_fseek(mat, endpos);
    mat->endStruct();
}


void TinyMATWriter_writeStruct(TinyMATWriterFile *mat, const char *name, const std::map<std::string, double> &data)
{
    mat->addStructItemName(name);
    mat->startStruct();
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxSTRUCT_CLASS_arrayflags, 0};
    std::map<std::string, double>::const_iterator i;
    std::vector<std::string> names;
    int32_t maxlen=0;
    for (i = data.begin(); i != data.end(); ++i) {
        std::string ni=i->first;
        while (ni.size()>31) {
            ni.erase(ni.size()-1, 1);
        }
        if ((int32_t)ni.size()>maxlen) maxlen=(int32_t)ni.size();
        names.push_back(ni);
    }
    if (maxlen<32) maxlen=32;
    // pad all field names to maxlen
    std::string joinednames;
    for (size_t ii=0; ii<names.size(); ii++) {
        while ((int64_t)names[ii].size()<maxlen) {
            names[ii].push_back('\0');
        }
        joinednames.append(names[ii]);
    }



    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    long sizepos;
    sizepos=TinyMAT_ftell(mat);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    TinyMAT_write32(mat, (int32_t)1);
    TinyMAT_write32(mat, (int32_t)1);

    // write struct name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    // write field name length
    TinyMAT_writeDatElementS_i32(mat, maxlen);

    // write field names
    TinyMAT_writeDatElement_stringas8bit(mat, joinednames.c_str(), (uint32_t)joinednames.size());

    // write data type
    for (i = data.begin(); i != data.end(); ++i) {
        double v=i->second;
        TinyMATWriter_writeMatrix2D_colmajor(mat, "", &v, 1, 1);
    }

    long endpos;
    endpos=TinyMAT_ftell(mat);
    TinyMAT_fseek(mat, sizepos);
    size_bytes=endpos-sizepos-4;
    TinyMAT_writeU32(mat, size_bytes);
    TinyMAT_fseek(mat, endpos);
    mat->endStruct();
}

TINYMATWRITER_LIB_EXPORT void TinyMATWriter_startCellArray(TinyMATWriterFile * mat, const char * name, const int32_t * sizes, uint32_t ndims)
{
  mat->addStructItemName(name);
  mat->startCell();

  uint32_t size_bytes = 0;
  uint32_t arrayflags[2] = { TINYMAT_mxCELL_CLASS_arrayflags, 0 };


  // write tag header
  TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
  mat->lastCell().sizepos = TinyMAT_ftell(mat);
  TinyMAT_writeU32(mat, size_bytes);

  // write arrayflags
  TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

  // write field dimensions
  TinyMAT_writeDatElement_i32a(mat, sizes, ndims);

  // write struct name
  TinyMAT_writeDatElement_stringas8bit(mat, name);

  mat->lastCell().data_start = TinyMAT_ftell(mat);


}

TINYMATWRITER_LIB_EXPORT void TinyMATWriter_endCellArray(TinyMATWriterFile * mat)
{
  TinyMATWriterCell& cell = mat->lastCell();

  long endpos = TinyMAT_ftell(mat);
  TinyMAT_fseek(mat, cell.sizepos);
  uint32_t size_bytes = endpos - cell.sizepos - 4;
  TinyMAT_writeU32(mat, size_bytes);
  TinyMAT_fseek(mat, endpos);

  mat->endCell();
}


void TinyMATWriter_writeStringList(TinyMATWriterFile *mat, const char *name, const std::list<std::string> &data)
{
    mat->addStructItemName(name);
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};

    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    long sizepos=TinyMAT_ftell(mat);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    TinyMAT_write32(mat, (int32_t)1);
    TinyMAT_write32(mat, (int32_t)data.size());

    // write field name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    // write data type
    for (std::list<std::string>::const_iterator it=data.begin(); it!=data.end(); it++) {
        std::string a=*it;
        TinyMATWriter_writeString(mat, "", a.c_str(), (uint32_t)a.size());
    }

    long endpos=TinyMAT_ftell(mat);
    TinyMAT_fseek(mat, sizepos);
    size_bytes=endpos-sizepos-4;
    TinyMAT_writeU32(mat, size_bytes);
    TinyMAT_fseek(mat, endpos);
}


void TinyMATWriter_writeStringVector(TinyMATWriterFile *mat, const char *name, const std::vector<std::string> &data)
{
    mat->addStructItemName(name);
    uint32_t size_bytes=0;
    uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};

    // write tag header
    TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
    long sizepos=TinyMAT_ftell(mat);
    TinyMAT_writeU32(mat, size_bytes);

    // write arrayflags
    TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

    // write field dimensions
    TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
    TinyMAT_writeU32(mat, (uint32_t)8);
    TinyMAT_write32(mat, (int32_t)1);
    TinyMAT_write32(mat, (int32_t)data.size());

    // write field name
    TinyMAT_writeDatElement_stringas8bit(mat, name);

    // write data type
    for (std::vector<std::string>::const_iterator it=data.begin(); it!=data.end(); it++) {
        std::string a=*it;
        TinyMATWriter_writeString(mat, "", a.c_str(), (uint32_t)a.size());
    }

    long endpos=TinyMAT_ftell(mat);
    TinyMAT_fseek(mat, sizepos);
    size_bytes=endpos-sizepos-4;
    TinyMAT_writeU32(mat, size_bytes);
    TinyMAT_fseek(mat, endpos);
}


#ifdef TINYMAT_USES_QVARIANT
    void TinyMATWriter_writeQVariantList(TinyMATWriterFile *mat, const char *name, const QVariantList &data)
    {
        mat->addStructItemName(name);
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};





        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
        TinyMAT_writeU32(mat, (uint32_t)8);
        TinyMAT_write32(mat, (int32_t)1);
        TinyMAT_write32(mat, (int32_t)data.size());

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        for (int i=0; i<data.size(); i++) {
            if (data[i].type()==QVariant::String) {
                QByteArray a=data[i].toString().toLatin1();
                TinyMATWriter_writeString(mat, "", a.data(), a.size());
            } else if (data[i].type()==QVariant::Map) {
                QVariantMap a=data[i].toMap();
                TinyMATWriter_writeQVariantMap(mat, "", a);
            } else if (data[i].type()==QVariant::List) {
                QVariantList a=data[i].toList();
                //std::cout<<i<<" "<<j<<" "<<QString(a).toStdString()<<"  length="<<a.size()<<"\n";
                TinyMATWriter_writeQVariantList(mat, "", a);

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
                TinyMATWriter_writeMatrix2D_colmajor<double>(mat, "", NULL, 0, 0);
            }
        }

        long endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }

    void TinyMATWriter_writeQStringList(TinyMATWriterFile *mat, const char *name, const QStringList &data)
    {
        mat->addStructItemName(name);
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};

        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
        TinyMAT_writeU32(mat, (uint32_t)8);
        TinyMAT_write32(mat, (int32_t)1);
        TinyMAT_write32(mat, (int32_t)data.size());

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        for (int i=0; i<data.size(); i++) {
            QByteArray a=data[i].toLatin1();
            TinyMATWriter_writeString(mat, "", a.data(), a.size());
        }

        long endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
    }

    void TinyMATWriter_writeQVariantMatrix_listofcols(TinyMATWriterFile *mat, const char *name, const QList<QList<QVariant> > &data)
    {
        mat->addStructItemName(name);
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxCELL_CLASS_arrayflags, 0};


        int32_t maxrows=0;
        for (int i=0; i<data.size(); i++) {
            for (int j=0; j<data[i].size(); j++) {
                maxrows=qMax(maxrows, data[i].size());
            }
        }


        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos;
        sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
        TinyMAT_writeU32(mat, (uint32_t)8);
        TinyMAT_write32(mat, (int32_t)maxrows);
        TinyMAT_write32(mat, (int32_t)data.size());

        // write field name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write data type
        for (int j=0; j<maxrows; j++) {
            for (int i=0; i<data.size(); i++) {
                QVariant v;
                if (j<data[i].size()) {
                    v=data[i].operator[](j);
                }
                //std::cout<<"+++ "<<i<<"/"<<j<<":   "<<TinyMAT_ftell(mat)<<" "<<(TinyMAT_ftell(mat)%8)<<"  write '"<<v.toString().toStdString()<<"'\n";
                if (v.type()==QVariant::String) {
                    QByteArray a=v.toString().toLatin1();
                    //std::cout<<i<<" "<<j<<" "<<QString(a).toStdString()<<"  length="<<a.size()<<"\n";
                    TinyMATWriter_writeString(mat, "", a.data(), a.size());
                } else if (v.type()==QVariant::Map) {
                    QVariantMap a=v.toMap();
                    TinyMATWriter_writeQVariantMap(mat, "", a);

                } else if (v.type()==QVariant::List) {
                    QVariantList a=v.toList();
                    //std::cout<<i<<" "<<j<<" "<<QString(a).toStdString()<<"  length="<<a.size()<<"\n";
                    TinyMATWriter_writeQVariantList(mat, "", a);
                } else if (v.canConvert(QVariant::Double)) {
                    double a=v.toDouble();
                    //std::cout<<i<<" "<<j<<" "<<a<<"\n";
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
                    //std::cout<<i<<" "<<j<<" "<<"EMPTY"<<"\n";
                    TinyMATWriter_writeMatrix2D_colmajor<double>(mat, "", NULL, 0, 0);
                }
                //std::cout<<"### "<<i<<"/"<<j<<":   "<<TinyMAT_ftell(mat)<<" "<<(TinyMAT_ftell(mat)%8)<<"\n";
            }
        }

        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        //fsetpos(mat->file, &sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
        //fsetpos(mat->file, &endpos);
        //std::cout<<endpos<<" "<<TinyMAT_ftell(mat)<<"\n";
    }




    void TinyMATWriter_writeQVariantMap(TinyMATWriterFile *mat, const char *name, const QVariantMap &data)
    {
        mat->addStructItemName(name);
        mat->startStruct();
        uint32_t size_bytes=0;
        uint32_t arrayflags[2]={TINYMAT_mxSTRUCT_CLASS_arrayflags, 0};
        QVariantMap::ConstIterator i;
        QList<QByteArray> names;
        int32_t maxlen=0;
        for (i = data.begin(); i != data.end(); ++i) {
            QString ni=i.key();
            QByteArray n=ni.toLocal8Bit();
            n=n.left(31);
            maxlen=qMax(maxlen, (int32_t)n.size());
            names<<n;
        }
        if (maxlen<32) maxlen=32;
        // pad all field names to maxlen
        for (int ii=0; ii<names.size(); ii++) {
            while (names[ii].size()<maxlen) {
                names[ii].append('\0');
            }
        }



        // write tag header
        TinyMAT_writeU32(mat, (uint32_t)TINYMAT_miMATRIX);
        long sizepos;
        sizepos=TinyMAT_ftell(mat);
        TinyMAT_writeU32(mat, size_bytes);

        // write arrayflags
        TinyMAT_writeDatElement_u32a(mat, arrayflags, 2);

        // write field dimensions
        TinyMAT_writeU32(mat, static_cast<uint32_t>(TINYMAT_miINT32));
        TinyMAT_writeU32(mat, (uint32_t)8);
        TinyMAT_write32(mat, (int32_t)1);
        TinyMAT_write32(mat, (int32_t)1);

        // write struct name
        TinyMAT_writeDatElement_stringas8bit(mat, name);

        // write field name length
        TinyMAT_writeDatElementS_i32(mat, maxlen);

        // write field names
        QByteArray joinednames;//=names.join();
        for (int i=0; i<names.size(); i++)  {
            joinednames+=names[i];
        }
        TinyMAT_writeDatElement_i8a(mat, (int8_t*)joinednames.data(), joinednames.size());

        // write data type
        for (i = data.begin(); i != data.end(); ++i) {
            QVariant v=i.value();
            QString n=i.key();

            if (v.type()==QVariant::String) {
                QByteArray a=v.toString().toLatin1();
                TinyMATWriter_writeString(mat, "", a.data(), a.size());
            } else if (v.type()==QVariant::List) {
                QVariantList a=v.toList();
                TinyMATWriter_writeQVariantList(mat, "", a);
            } else if (v.type()==QVariant::Map) {
                QVariantMap a=v.toMap();
                TinyMATWriter_writeQVariantMap(mat, "", a);
            } else if (v.canConvert(QVariant::Double)) {
                double a=v.toDouble();
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
                TinyMATWriter_writeMatrix2D_colmajor<double>(mat, "", NULL, 0, 0);
            }
        }

        long endpos;
        endpos=TinyMAT_ftell(mat);
        TinyMAT_fseek(mat, sizepos);
        size_bytes=endpos-sizepos-4;
        TinyMAT_writeU32(mat, size_bytes);
        TinyMAT_fseek(mat, endpos);
        mat->endStruct();
    }

#endif

#ifdef TINYMAT_USES_OPENCV
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeCVMat(TinyMATWriterFile* mat, const char* name, const cv::Mat& img) {
    if (img.rows<=0 || img.cols<=0) {
      //throw std::runtime_error("OpenCV Matrix has too many dimensions or is empty");
      TinyMATWriter_writeEmptyMatrix(mat, name);
    } else {
      //mat->addStructItemName(name);
      int32_t sizes[2] = { img.cols, img.rows };
      uint32_t ndims = 2;
      cv::Mat tmp = img.clone(); // make a full copy of the input matrix. The copy will contain the data in continuous form!!!
      if (tmp.depth() == CV_8U) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (uint8_t*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_8S) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (int8_t*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_16U) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (uint16_t*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_16S) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (int16_t*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_32S) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (int32_t*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_32F) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (float*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else if (tmp.depth() == CV_64F) {
        TinyMATWriter_writeMultiChannelMatrixND_rowmajor(mat, name, (double*)tmp.data, sizes, ndims, (uint32_t)img.channels());
      } else {
        throw std::runtime_error("OpenCV Matrix has a datatype which is not supported by TinyMATWriter_writeCVMat()");
      }
    }
}

template <>
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeContainerAsRow(TinyMATWriterFile* mat, const char* name, const std::vector<cv::Point2d>& data_vec) {
  if (data_vec.size() <= 0) TinyMATWriter_writeEmptyMatrix(mat, name);
  int32_t siz[2] = { 2, (int32_t)data_vec.size() };
  double* tmp = (double*)malloc(data_vec.size() * 2 * sizeof(double));
  int i = 0;
  for (auto& it : data_vec) {

    tmp[0 + i] = it.x;
    tmp[(data_vec.size()) + i] = it.y;
    i++;
  }
  TinyMATWriter_writeMatrix2D_rowmajor<double>(mat, name, tmp, (int32_t)data_vec.size(), 2);
  free(tmp);
}

template <>
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeContainerAsRow(TinyMATWriterFile* mat, const char* name, const std::vector<cv::Point2i>& data_vec) {
  if (data_vec.size() <= 0) TinyMATWriter_writeEmptyMatrix(mat, name);
  int32_t siz[2] = { 2, (int32_t)data_vec.size() };
  int* tmp = (int*)malloc(data_vec.size() * 2 * sizeof(int));
  int i = 0;
  for (auto& it : data_vec) {

    tmp[0 + i] = it.x;
    tmp[(data_vec.size()) + i] = it.y;
    i++;
  }
  TinyMATWriter_writeMatrix2D_rowmajor<int>(mat, name, tmp, (int32_t)data_vec.size(), 2);
  free(tmp);
}

template <>
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeContainerAsRow(TinyMATWriterFile* mat, const char* name, const std::vector<cv::Point2f>& data_vec) {
  if (data_vec.size() <= 0) TinyMATWriter_writeEmptyMatrix(mat, name);
  int32_t siz[2] = { 2, (int32_t)data_vec.size() };
  float* tmp = (float*)malloc(data_vec.size() * 2 * sizeof(float));
  int i = 0;
  for (auto& it : data_vec) {

    tmp[0 + i] = it.x;
    tmp[(data_vec.size()) + i] = it.y;
    i++;
  }
  TinyMATWriter_writeMatrix2D_rowmajor<float>(mat, name, tmp, (int32_t)data_vec.size(), 2);
  free(tmp);
}

template <>
TINYMATWRITER_LIB_EXPORT void TinyMATWriter_writeContainerAsRow(TinyMATWriterFile* mat, const char* name, const std::vector<cv::Point2l>& data_vec) {
  if (data_vec.size() <= 0) TinyMATWriter_writeEmptyMatrix(mat, name);
  int32_t siz[2] = { 2, (int32_t)data_vec.size() };
  int64_t* tmp = (int64_t*)malloc(data_vec.size() * 2 * sizeof(int64_t));
  int i = 0;
  for (auto& it : data_vec) {

    tmp[0 + i] = it.x;
    tmp[(data_vec.size()) + i] = it.y;
    i++;
  }
  TinyMATWriter_writeMatrix2D_rowmajor<int64_t>(mat, name, tmp, (int32_t)data_vec.size(), 2);
  free(tmp);
}

#endif

