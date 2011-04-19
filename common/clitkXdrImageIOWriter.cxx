/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
/**
 * @file   clitkXdrImageIO.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:12:20 2008
 *
 * @brief
 *
 *
 */

#include "clitkXdrImageIO.h"
#include "clitkCommon.h"

#include <sys/stat.h>

//From mbfield.h
#if !defined(unix) && !defined(__APPLE__)
//#define _read  readfix
#endif
#define AVSINT ptrdiff_t
#define AVS_ERROR
#define AVS_OK

//From portdefs.h
#if defined(unix) || defined(__APPLE__)
#define O_BINARY 0
#define setmode(a,b) 0
#endif

#ifndef __LARGE__
#  if defined(__GNUC__) || defined(unix) || defined(__APPLE__)

typedef long long Q_INT64;
typedef unsigned long long Q_UINT64;
#    define Q_INT64_CONST(x) (x##ll)
#    define Q_UINT64_CONST(x) (x##llu) /* gcc also allows ull */
/* When using MINGW with MS(V)CRT DLL, use MS format modifier. */
#    ifdef __MSVCRT__
#      define Q_INT64_FORMAT "I64"
#    else
#      define Q_INT64_FORMAT "L"
#    endif
#  elif defined(__BORLANDC__) || defined(__WATCOMC__) || defined(_MSC_VER)
typedef __int64 Q_INT64;
typedef unsigned __int64 Q_UINT64;
#    define Q_INT64_CONST(x) (x##i64)
#    define Q_UINT64_CONST(x) (x##ui64) /* i64u is not allowed! */
#    ifdef _MSC_VER
#      define Q_INT64_FORMAT "I64"
#    else
#      define Q_INT64_FORMAT "L"
#    endif
#  else
#    error No 64 bit integers known for this compiler, edit portdefs.h.
#  endif
#endif

bool clitk::XdrImageIO::CanWriteFile(const char* FileNameToWrite)
{
  std::string filename(FileNameToWrite);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("xdr")) return false;
  return true;
}

void clitk::XdrImageIO::Write(const void* buffer)
{
  char *s = const_cast<char*>("");
  WriteImage( m_FileName.c_str(), s, s, 0, -1, 0, 2, 0, 0, 0, 0, buffer);
}

// Based on a true story by the Nederlands Kanker Instituut (AVS_WXDR.CPP from the 20091216)

/************************************************************************/
/*                             INCLUDE FILES                            */
/************************************************************************/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#if !defined(unix) && !defined(__APPLE__)
#include <io.h>
#endif
#include <fcntl.h>
#include <errno.h>

#include <algorithm>

#ifdef WIN32
// don't use min() and max() macros indirectly defined by windows.h,
// but use portable std::min() and std:max() instead
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

/************************************************************************/
/*                    DEFINES, ENUMERATED TYPES AND CONSTANTS           */
/************************************************************************/

#pragma pack (1)

// Fields with data size>8GB (having UINT_MAX short pixels) cannot be compressed using
// NKI_MODE2 struct because iOrgSize has type "unsigned int". In that case use NKI_MODE2_64BITS.
// The type of structure is indicated as follows:
//
// iOrgSize==0: NKI_MODE2_64BITS
// otherwise  : NKI_MODE2
//
// Compression modes 1 and 3 (without CRCs) only use the first 2 members (iOrgSize and iMode).

typedef struct {
  unsigned int iOrgSize;          /* in pixels (i.e. shorts) */
  unsigned int iMode;             /* 1, 2, 3 or 4 */
  unsigned int iCompressedSize;   /* in bytes, excluding header */
  unsigned int iOrgCRC;           /* CRC of the data (no coords etc) */
  unsigned int iCompressedCRC;	  /* CRC of the compressed data, excluding this header */
} NKI_MODE2;

typedef struct {
  unsigned int iOrgSize;          /* in pixels (i.e. shorts) */
  unsigned int iMode;             /* 1, 2, 3 or 4 */
  unsigned int iCompressedSize;   /* in bytes, excluding header */
  unsigned int iOrgCRC;           /* CRC of the data (no coords etc) */
  unsigned int iCompressedCRC;	  /* CRC of the compressed data, excluding this header */
  unsigned int iPad;              /* unused */
  Q_UINT64     i64OrgSize;        /* used for more than UINT_MAX pixels, indicated by iOrgSize==0 (0-vector not compressed) */
  Q_UINT64     i64CompressedSize; /* value in BYTES, used for more than UINT_MAX PIXELS, indicated by iCompressedSize==0 */
  Q_UINT64     i64Future1;
  Q_UINT64     i64Future2;
} NKI_MODE2_64BITS;

#pragma pack ()

// Changed next to static function in stead of macro so it can
// have a return value to check in the calling function.
// It could be made inline as well, but there is no real time
// punishment from the extra layer of function calls.

// note: some compilers do not like comments ending in a backslash.
// so use macro functions to exclude.


/************************************************************************/
/*                             GLOBAL VARIABLES                         */
/************************************************************************/

static const unsigned long CRC32_table[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
  0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
  0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
  0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/************************************************************************/
/*                             MODULE FUNCTIONS                         */
/************************************************************************/

#ifdef __WATCOMC__
_WCRTLINK
#endif
int writefix(int file, const void *buf, unsigned int count)
{
  int j, k, total=0;

  for (unsigned i=0; i<count; i+=16384) {
    j = count - i;
    if (j>16384) j=16384;

    k=write(file, (char *)buf+i, j);
    if (k < 0) return k;

    total += k;

    if (k != j) break;
  }

  return total;
}


/*
  Version of write() that takes special action in case of
  standard output.  Based on commented out macro above.
  This function overloads the <cstdio> (or stdio.h for old style C++)
  write() function.
*/

// Like the original macro, we do /not/ want writefix from mbfield.c.
#ifdef write
#undef write
#endif
static int wxdr_write(int handle, const void * buf, unsigned len)
{
  // if (handle == 1) // stdout
  if (handle == fileno(stdout)) {
#ifdef WIN32
    // Behave as C standard library write(): return number of bytes
    // written or -1 and errno set on error.
    fflush(stdout);
    DWORD dwBytesWritten;
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, len,
                   &dwBytesWritten, NULL)) {
      // There is no simple 1-to-1 mapping between GetLastError()
      // values that WriteFile() can return (quite a lot) and the two
      // errno values that write() can return.  So return EACCES in
      // almost all cases.
      switch (GetLastError()) {
      case ERROR_INVALID_HANDLE:
        errno = EBADF ;
        break;
      default:
        errno = EACCES;
        break;
      }
      return -1;
    } else
      return (int)dwBytesWritten; // May still be < len!
    // And... write() may write a maximum of UINT_MAX-1 bytes, whereas
    // WriteFile() may write UINT_MAX bytes at once.  But since
    // int(UINT_MAX) == -1 this will pose an actual problem in the
    // (far?) future.
#else // !WIN32
    //const int oldmode = setmode(handle, O_BINARY);//commented out by joel
    const int iBytesWritten = write(handle, buf, len);
    const int saveerrno = errno; // setmode() may change errno.
    //if (oldmode != -1) setmode(handle, oldmode); //commented out by joel
    errno = saveerrno;
    return iBytesWritten;
#endif // !WIN32
  } else
    return write(handle, buf, len);
}

/*
  Checked write().
  Behaves like win32 WriteFile() and returns a Boolean to indicate
  success or failure, where failure almost invariably means disc full.

  !!! SIDE EFFECT !!!

  In case of failure, this function issues an AVS error message
  and closes the file (if handle != 1).  It is up to the calling
  function to return the AVS_ERROR state and before that do things
  like close other files, free memory etc.  This way, there is less
  chance of erroneously duplicated code, like in:
    written = write(f, temp, strlen(temp));
    if (written == -1 || written != strlen(temp))
    { AVSerror(...);
      if (f != fileno(stdout)) close(f);
      return AVS_ERROR;
    }
    written = write(f, buf, buflength)
    if (written == -1 || written != strlen(temp)) {
      // oops, wrong length copy'n'pasted

  If more elaborate error handling is needed then the calling
  functuon should use the (overloaded) write() and act on its return
  value (and the value of errno) accordingly.

  It does /not/ close stdout.

  !!! SIDE EFFECT !!!

  Note that checked_write() takes a size_t for len, whereas write() takes
  an unsigned int of 4 bytes. On a 64 bits OS a size_t will be an 8 byte integer,
  enabling more than UINT_MAX bytes to write at once.
*/
static bool checked_write(int handle, const void * buf, size_t len, char **buffer)
{
  if (buffer && !handle) {
    memcpy(*buffer, buf, len);
    (*buffer) += len;
    return true;
  }
  if (buffer && handle) {
    (*buffer) += len;
    return true;
  } else {
    for(int i=0; i<2; i++) {
      int byteswritten;
      size_t remaining;
      int chunksize;

      //If write fails, test if not related to big buffer problem
      //Bug report http://support.microsoft.com/kb/899149 entitled
      //"You cannot call the fwrite function to write to a buffer
      // that is larger than 64 MB in Visual C++ 2005,
      // in Visual C++ .NET 2003, or in Visual C++ .NET 2002"
      // NB: same thing for write function in binary mode
      if (i==0) {
        remaining = len;
        // call wxdr_write (for handle!=fileno(stdout) a wrapper for write) several times
        // to interpret the signed 32-bit return value correctly
        while (remaining>0) {
          chunksize = (int)std::min(remaining, (size_t)INT_MAX);
          byteswritten = wxdr_write(handle, buf, chunksize);
          if (byteswritten == chunksize)
            remaining -= chunksize;
          else
            break; // try writefix in the next round
        }
        if (remaining == 0)
          return true;
      } else {
        remaining = len;
        // call writefix (in mbfield.c) several times to interpret the signed 32-bit
        // return value correctly. writefix uses chunks of 16384 bytes
        while (remaining>0) {
          chunksize = (int)std::min(remaining, (size_t)INT_MAX);
          byteswritten = writefix(handle, buf, chunksize);
          if (byteswritten == chunksize)
            remaining -= chunksize;
          else
            break; // even writefix failed: return error
        }
        if (remaining == 0)
          return true;
      }
    }
    // Note: file is open in binary mode, no need to compensate
    // for a value of byteswritten > len due to \n -> \r\n conversions.
    // (write() on a text stream is implementation dependent.)
    if (handle != fileno(stdout)) close(handle);
    AVSerror("Avs_wxdr: write failed, disk full?");
    return false;
  }
}

/* coder for NKI private compressed pixel data
   arguments: dest    = (in) points to area where compressed destination data is written (byte)
              src     = (in) points to uncompressed source data (short)
              npixels = (in) number of pixels to compress

   The return value is the number of bytes in the compressed data (maximal 3*npixels+10, typical 0.52*npixels)

   if iMode == 1 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=1
   - The rest is the compressed image

   if iMode == 2 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=2
   - The following 4 bytes contain the size of the compressed image (in bytes)
   - The following 4 bytes contain the CRC of the original image
   - The following 4 bytes contain the CRC of the compressed image
   - The rest is the compressed image
   - The compressed size will be even (padded by a zero if necessary).

   if iMode == 3 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=3
   - The rest is the compressed image, including 4 bit differences

   if iMode == 4 then
   - The first 4 bytes contain the number of short-int pixels
   - The following 4 bytes contain iMode=4
   - The following 4 bytes contain the size of the compressed image (in bytes)
   - The following 4 bytes contain the CRC of the original image
   - The following 4 bytes contain 0
   - The rest is the compressed image, including 4 bit differences
   - The compressed size will be even (padded by a zero if necessary).

   iMode 1 and iMode 2 are identical, except for the CRC data that is included for iMode 2
   iMode 3 and iMode 4 are identical, except for the CRC data that is included for iMode 4
*/

// optimized settings for the 4 bit run compressor (mode 3 and 4)

#define MINZEROS 5		// shortest RLE (2 byte overhead, but breaks 4bit run)
#define MIN4BIT  6		// shortest 4 bit run (6 bytes compressed to 5 bytes)

// This internal routine converts an 8 bit difference string into a 4 bit one
static signed char *recompress4bit(int n, signed char *dest)
{
  signed char *p, *q;
  int val;

  n = n & 0xfe;
  dest -= n;
  p = dest;
  val = (((int)p[0])<<4) | (p[1]&15);
  p += 2;
  *dest++ = -0x40; // 192 (0xc0) does not fit between -128..127: maps to -64 (0x40) in 2's complement
  *dest++ = (signed char)n;
  q = dest++;
  n -= 2;
  while(n>0) {
    *dest++ = (signed char)((((int)p[0])<<4) | (p[1]&15));
    p += 2;
    n -= 2;
  }
  q[0] = (signed char)val;

  return dest;
}


static size_t nki_private_compress(signed char  *dest, short int  *src, size_t npixels, int iMode)
{
  unsigned long		iCRC;
  unsigned long		iCRC2;
  unsigned int          iHeaderSize=8;                      // value for iMode==1 and iMode==3
  register int		val;
  size_t                i,j;
  NKI_MODE2*		pHeader = (NKI_MODE2*)dest;
  NKI_MODE2_64BITS*     pHeader_64bits = (NKI_MODE2_64BITS*)dest;
  size_t		iBufferSize;

  iBufferSize = (npixels / 2) * 3;		            // Buffer is sizeof(NKI_MODE2_64BITS) + 10 bytes larger

  /* Up till now only Mode=1 .. 4 are supported */
  if ((iMode < 1) || (iMode > 4))
    return 0;

  /* Create the header */
  pHeader->iMode = iMode;

  if (sizeof(int*)>sizeof(int) && npixels>UINT_MAX) {       // On a 64 bits OS we want to store files>4GB
    pHeader_64bits->iOrgSize   = 0;                         // This indicates>4GB file (0-vector is not compressed)
    pHeader_64bits->i64OrgSize = npixels;
    iHeaderSize = sizeof(NKI_MODE2_64BITS);
    dest += sizeof(NKI_MODE2_64BITS);
  } else {
    pHeader->iOrgSize = (unsigned int)(npixels & UINT_MAX); // store 32 bit number as first member

    if (iMode==2 || iMode==4)
      iHeaderSize = sizeof(NKI_MODE2);
    dest += iHeaderSize;
  }

  /* Create the compressed image */

  if (iMode == 1) {
    *(short int *)dest = *src;
    dest+=2;

    npixels--;

    do {
      val = src[1] - src[0];
      src++;

      if (val == 0) {                          /* run length-encode zero differences */
        for (i=2;; i++) {
          if (i>=npixels || src[i-1]!=src[-1] || i==256) {
            if (i==2)
              *dest++=0;
            else {
              *dest++  =  -128; // hexadecimal 0x80
              *dest++  = (signed char)(i-1);
              npixels -= (i-2);
              src     += (i-2);
            }
            break;
          }
        }
      } else if (val >= -64 && val <= 63) {     /* small difference coded as one byte */
        *dest = (signed char)val;
        dest++;
      } else if (val >= -0x3F00 && val <= 0x3EFF) { /* large differences coded as two bytes */
        dest[0] = (signed char)((val>>8) ^ 0x40);
        dest[1] = (signed char)val;
        dest+=2;
      } else {                                  /* if very large differences code abs val as three bytes */
        *dest++ = 0x7F;
        *dest++ = (signed char)(src[0]>>8);
        *dest++ = (signed char)(src[0]);
      }
      /* Are we beyond the allocated memory? */
      if ((size_t)(dest - (signed char*)pHeader) > iBufferSize)
        return 0;
    } while (--npixels);
  }

  else if (iMode == 2) {
    iCRC  = 0;
    iCRC2 = 0;

    *(short int *)dest = val = *src;
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char) val    ] ^ ((iCRC2 >> 8));
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) val    ] ^ ((iCRC  >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(val>>8)] ^ ((iCRC  >> 8));
    dest+=2;
    npixels--;

    do {
      val = src[1] - src[0];
      src++;
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));

      if (val == 0) {                          /* run length-encode zero differences */
        for (i=2;; i++) {
          if (i>=npixels || src[i-1]!=src[-1] || i==256) {
            if (i==2) {
              *dest++=0;
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0    ] ^ ((iCRC2 >> 8));
            } else {
              *dest++  =  -128; // hexadecimal 0x80
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0x80 ] ^ ((iCRC2 >> 8));
              *dest++  = (signed char)(i-1);
              iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (i-1)] ^ ((iCRC2 >> 8));
              npixels -= (i-2);

              for (j=0; j<i-2; j++) {
                src++;
                iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
                iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));
              }
            }
            break;
          }
        }
      } else if (val >= -64 && val <= 63) {     /* small difference coded as one byte */
        *dest = (signed char)val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
        dest++;
      } else if (val >= -0x3F00 && val <= 0x3EFF) { /* large differences coded as two bytes */
        dest[0] = (signed char)((val>>8) ^ 0x40);
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)dest[0] ] ^ ((iCRC2 >> 8));
        dest[1] = (signed char)val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
        dest+=2;
      } else {                                  /* if very large differences code abs val as three bytes */
        dest[0] = 0x7F;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0x7f                   ] ^ ((iCRC2 >> 8));
        val     = src[0];
        dest[1] = (signed char)(val>>8);
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest[2] = (signed char)val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val     ] ^ ((iCRC2 >> 8));
        dest+=3;
      }
      /* Are we beyond the allocated memory? */
      if ((size_t)(dest - (signed char*)pHeader) > iBufferSize)
        return 0;
    } while (--npixels);

    if ((dest - (signed char*)pHeader - iHeaderSize)<UINT_MAX) // store 32 bit number as third member
      pHeader->iCompressedSize =
        (unsigned int)(dest - (signed char*)pHeader - iHeaderSize);
    else                                                       // store 64 bit number in extended structure
      pHeader_64bits->i64CompressedSize = dest - (signed char*)pHeader -iHeaderSize;

    /* Pad it to get an even length */
    if (pHeader->iCompressedSize & 1) {
      *dest++ = 0;
      iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ 0] ^ ((iCRC2 >> 8));
      pHeader->iCompressedSize++;
    }

    pHeader->iOrgCRC        = iCRC;
    pHeader->iCompressedCRC = iCRC2;
  }

  /* Create the compressed image - compressor with added 4 bit run */

  else if (iMode == 3) {
    int n4bit=0;
    *(short int *)dest = *src;
    dest+=2;
    npixels--;

    do {
      val = src[1] - src[0];
      src++;

      if (val == 0) {                          	/* run length-encode zero differences */
        for (i=2;; i++) {
          if (i>=npixels || src[i-1]!=src[-1] || i==256) {
            if (i<=MINZEROS) {	       	/* too short run -> write zeros */
              for (j=0; j<i-1; j++) {
                *dest++=0;
                n4bit++;

                if(n4bit>=254) {		/* maximum length 4 bit run */
                  dest  = recompress4bit(n4bit, dest);
                  n4bit = 0;
                }
              }
            } else {
              if (n4bit>=MIN4BIT)		/* end (and write) 4 bit run */
                dest  = recompress4bit(n4bit, dest);

              n4bit=0;
              *dest++  = -128; // hexadecimal 0x80
              *dest++  = (signed char)(i-1);
            }

            npixels -= (i-2);
            src     += (i-2);
            break;
          }
        }
      } else if (val >= -63 && val <= 63) {     /* small difference coded as one byte */
        if (val >= -8 && val <= 7) {
          *dest++ = (signed char)val;
          n4bit++;

          if(n4bit>=254) {		/* maximum length 4 bit run */
            dest  = recompress4bit(n4bit, dest);
            n4bit=0;
          }
        } else if(n4bit>=MIN4BIT) {		/* end and write 4 bit run */
          j = val;
          dest  = recompress4bit(n4bit, dest);
          n4bit=0;
          *dest++ = (signed char)j;
        } else {
          *dest++ = (signed char)val;			/* end 4 bit run */
          n4bit  = 0;
        }
      } else if (val >= -0x3F00 && val <= 0x3EFF) { /* large differences coded as two bytes */
        j = val;

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        dest[0] = (signed char)((j>>8) ^ 0x40);
        dest[1] = (signed char)j;
        dest+=2;
      } else {                                  /* if very large differences code abs val as three bytes */
        j = src[0];

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        *dest++ = 0x7F;
        *dest++ = (signed char)(j>>8);
        *dest++ = (signed char)j;
      }
      /* Are we beyond the allocated memory? */
      if ((size_t)(dest - (signed char*)pHeader) > iBufferSize)
        return 0;
    } while (--npixels);
  }

  /* Create the compressed image - compressor with added 4 bit run and CRC */

  else if (iMode == 4) {
    int n4bit=0;
    iCRC  = 0;

    *(short int *)dest = val = *src;
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) val    ] ^ ((iCRC  >> 8));
    iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(val>>8)] ^ ((iCRC  >> 8));
    dest+=2;
    npixels--;

    do {
      val = src[1] - src[0];
      src++;
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
      iCRC  = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));

      if (val == 0) {                          	/* run length-encode zero differences */
        for (i=2;; i++) {
          if (i>=npixels || src[i-1]!=src[-1] || i==256) {
            if (i<=MINZEROS) {	       	/* too short run -> write zeros */
              for (j=0; j<i-1; j++) {
                *dest++=0;
                n4bit++;

                if(n4bit>=254) {		/* maximum length 4 bit run */
                  dest  = recompress4bit(n4bit, dest);
                  n4bit = 0;
                }
              }
            } else {
              if (n4bit>=MIN4BIT)		/* end (and write) 4 bit run */
                dest  = recompress4bit(n4bit, dest);

              n4bit=0;
              *dest++  = -128; // hexadecimal 0x80
              *dest++  = (signed char)(i-1);
            }

            npixels -= (i-2);
            for (j=0; j<i-2; j++) {
              src++;
              iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char) src[0]    ] ^ ((iCRC  >> 8));
              iCRC = CRC32_table[(unsigned char)iCRC  ^ (unsigned char)(src[0]>>8)] ^ ((iCRC  >> 8));
            }
            break;
          }
        }
      } else if (val >= -63 && val <= 63) {     /* small difference coded as one byte */
        if (val >= -8 && val <= 7) {
          *dest++ = (signed char)val;
          n4bit++;

          if(n4bit>=254) {		/* maximum length 4 bit run */
            dest  = recompress4bit(n4bit, dest);
            n4bit=0;
          }
        } else if(n4bit>=MIN4BIT) {		/* end and write 4 bit run */
          j = val;
          dest  = recompress4bit(n4bit, dest);
          n4bit=0;
          *dest++ = (signed char)j;
        } else {
          *dest++ = (signed char)val;		/* end 4 bit run */
          n4bit  = 0;
        }
      } else if (val >= -0x3F00 && val <= 0x3EFF) { /* large differences coded as two bytes */
        j = val;

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        dest[0] = (signed char)((j>>8) ^ 0x40);
        dest[1] = (signed char)j;
        dest+=2;
      } else {                                  /* if very large differences code abs val as three bytes */
        j = src[0];

        if(n4bit>=MIN4BIT)			/* end (and write) 4 bit run */
          dest  = recompress4bit(n4bit, dest);

        n4bit=0;
        *dest++ = 0x7F;
        *dest++ = (signed char)(j>>8);
        *dest++ = (signed char)j;
      }
      /* Are we beyond the allocated memory? */
      if ((size_t)(dest - (signed char*)pHeader) > iBufferSize)
        return 0;
    } while (--npixels);

    if ((dest - (signed char*)pHeader - iHeaderSize)<UINT_MAX) // store 32 bit number as third member
      pHeader->iCompressedSize =
        (unsigned int)(dest - (signed char*)pHeader - iHeaderSize);
    else {                                                     // store 64 bit number in extended structure
      pHeader_64bits->iCompressedSize = 0;
      pHeader_64bits->i64CompressedSize = dest - (signed char*)pHeader -iHeaderSize;
    }

    /* Pad it to get an even length */
    if (pHeader->iCompressedSize & 1) {
      *dest++ = 0;
      pHeader->iCompressedSize++;
    }

    pHeader->iOrgCRC        = iCRC;
    pHeader->iCompressedCRC = 0;
  }

  return dest - (signed char*)pHeader;
}


void clitk::XdrImageIO::WriteImage(const char* file, char* headerinfo, char* headerfile, int raw,
                                   int offset, char bLittleEndian, int iNkiCompression,
                                   int wcoords, int append, int getsize, char *tobuffer, const void* data)
{
  AVSINT   total=1;
  unsigned int      i;
  AVSINT   coords=0;
  int      f=0;
  char     temp[256];
  char     *c;
  char     cSwap;
  FILE     *fp;
  long     swap_test = 0x1000000;
  signed char*	pCompressed = NULL;
  size_t   FilePos=0;
  char	   **buffer = NULL;
  int	   len=0;
  char     *buf2;
  size_t   slen;

  if (bLittleEndian)
    swap_test = 0x00000001;

  if (getsize) {
    swap_test = 0xffffffff;	// never swap to save time
    buffer    = (char **) &len;
    f         = 1;
  }

  if (tobuffer) {
    buf2   = (char *)tobuffer;
    buffer = &buf2;
    f      = 0;
  }

  for (i=0; i<GetNumberOfDimensions(); i++) {
    total  *= GetDimensions(i);
    coords += GetDimensions(i);
  }

  /* Try allocate the compressed fielddata - compression disabled if alloc fails */
  if ((iNkiCompression > 0) &&
      (GetComponentType() == itk::ImageIOBase::SHORT) &&
      (GetPixelType() == itk::ImageIOBase::SCALAR)) {
    pCompressed = (signed char *)malloc((total/2) * 3 + sizeof(NKI_MODE2_64BITS) + 10);
    if (pCompressed==NULL) {
      iNkiCompression = 0;
      AVSwarning("Avs_wxdr: not enough memory to compress data, saving uncompressed");
    }
  }

  if (!(tobuffer || getsize)) {
    if (offset != -1) {
      f = open(file, O_RDWR, 0);
      if (f < 0) {
        AVSerror("Avs_wxdr: Opening " << file << "failed.\n" << strerror(errno));
        free(pCompressed);
        return AVS_ERROR;
      }
      lseek(f, offset, SEEK_SET);
    } else {
      if (strlen(file)==0)
        f = fileno(stdout);
      else {
        if (append)
          f = open(file, O_RDWR | O_APPEND, 0);
        else
          f = creat(file, S_IWRITE | S_IREAD);
      }

      if (f < 0) {
        AVSerror("Avs_wxdr: Creating " << file << " failed.\n" << strerror(errno));
        free(pCompressed);
        return AVS_ERROR;
      }
    }
  }

  if (!raw) {
    sprintf(temp, "# AVS wants to have the first line starting with its name\n");
    slen = strlen(temp);

    if (!checked_write(f, temp, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;

    slen = strlen(headerinfo);
    if (slen && !checked_write(f, headerinfo, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;

    if (!checked_write(f, "\n", 1, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos++;

    if (strlen(headerfile)) {
      fp = fopen(headerfile, "rt");
      if (fp) {
        for (;;) {
          if (fgets(temp, 255, fp) == NULL) break;
          slen = strlen(temp);
          if (!checked_write(f, temp, slen, buffer)) {
            fclose(fp);
            free(pCompressed);
            return AVS_ERROR;
          }
          FilePos += slen;
        }
        fclose(fp);
        if (!checked_write(f, "\n", 1, buffer)) {
          free(pCompressed);
          return AVS_ERROR;
        }
        FilePos++;
      }
    }

    sprintf(temp, "ndim=%d\n", GetNumberOfDimensions());
    slen = strlen(temp);
    if (!checked_write(f, temp, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;
  }

  for (i=0; i<GetNumberOfDimensions(); i++) {
    if (!raw) {
      sprintf(temp, "dim%d=%d\n", i+1, GetDimensions(i));
      slen = strlen(temp);
      if (!checked_write(f, temp, slen, buffer)) {
        free(pCompressed);
        return AVS_ERROR;
      }
      FilePos += slen;
    }
  }

  if (!raw) {
    sprintf(temp, "nspace=%d\n", GetNumberOfDimensions());
    slen = strlen(temp);
    if (!checked_write(f, temp, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;

    sprintf(temp, "veclen=%d\n", GetNumberOfComponents());
    slen = strlen(temp);
    if (!checked_write(f, temp, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;

    switch(GetComponentType()) {
    case itk::ImageIOBase::CHAR   :
      strcpy(temp, "data=byte\n");
      break;
    case itk::ImageIOBase::SHORT  :
      strcpy(temp, "data=xdr_short\n");
      break;
    case itk::ImageIOBase::INT    :
      strcpy(temp, "data=xdr_integer\n");
      break;
    case itk::ImageIOBase::FLOAT  :
      strcpy(temp, "data=xdr_real\n");
      break;
    case itk::ImageIOBase::DOUBLE :
      strcpy(temp, "data=xdr_double\n");
      break;
    default               :
      if (f != fileno(stdout)) close(f);
      free(pCompressed);
      return AVS_ERROR;
    }
    slen = strlen(temp);
    if (!checked_write(f, temp, slen, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
    FilePos += slen;
  }


  //FilePos = tell(f);
ONCE_AGAIN:


  //switch(input->uniform)
  //{ case UNIFORM     :
  strcpy(temp, "field=uniform\n");
  coords = GetNumberOfDimensions() * 2;
  //       break;
  //  case RECTILINEAR : strcpy(temp, "field=rectilinear\n");
  //       break;
  //  case IRREGULAR   : strcpy(temp, "field=irregular\n");
  //       coords = total * input->nspace;
  //       break;
  //  default          : if (f != fileno(stdout)) close(f);
  //                     free(pCompressed);
  //    		       return;
  //}

  if (!raw) {
    if (!checked_write(f, temp, strlen(temp), buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }

    if ((iNkiCompression > 0) &&
        (GetComponentType() == itk::ImageIOBase::SHORT) &&
        (GetPixelType() == itk::ImageIOBase::SCALAR)) {
      sprintf(temp, "nki_compression=%d", iNkiCompression);
      if (!checked_write(f, temp, strlen(temp), buffer)) {
        free(pCompressed);
        return AVS_ERROR;
      }
    }

    temp[0] = temp[1] = 12;
    if (!checked_write(f, temp, 2, buffer)) {
      free(pCompressed);
      return AVS_ERROR;
    }
  }

  total *= GetPixelSize();

  if ((!raw) && (iNkiCompression > 0) &&
      (GetComponentType() == itk::ImageIOBase::SHORT) &&
      (GetPixelType() == itk::ImageIOBase::SCALAR)) {
    size_t	iCompressedLength;

    iCompressedLength = nki_private_compress(pCompressed,
                        (short int *)(data), total/2, iNkiCompression);

    if (iCompressedLength > 0) {
      if (!checked_write(f, pCompressed, iCompressedLength, buffer)) {
        free(pCompressed);
        return AVS_ERROR;
      }
      free(pCompressed);
      goto WRITE_COORDS;
    }

    /* Compressionratio was poor: let's write uncompressed */
    iNkiCompression = 0;
    total /= 2;
    free(pCompressed);
    pCompressed = NULL;
    lseek(f, (unsigned int)FilePos, SEEK_SET); // use _lseeki64 just in case header size > UINT_MAX bytes
    goto ONCE_AGAIN;
  }

  /* swap data if required (xdr is low-endian) */

  if (!(*(char *)(&swap_test))) {
    if (GetComponentSize()==2) {
      c = (char *)data;
      for (i=0; i<total; i+=2) {
        cSwap  = c[i];
        c[i]   = c[i+1];
        c[i+1] = cSwap;
      }
    } else if (GetComponentSize()==4) {
      c = (char *)data;
      for (i=0; i<total; i+=4) {
        cSwap = c[i];
        c[i]   = c[i+3];
        c[i+3] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+2];
        c[i+2] = cSwap;
      }
    } else if (GetComponentSize()==8) {
      c = (char *)data;
      for (i=0; i<total; i+=8) {
        cSwap = c[i];
        c[i]   = c[i+7];
        c[i+7] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+6];
        c[i+6] = cSwap;
        cSwap = c[i+2];
        c[i+2] = c[i+5];
        c[i+5] = cSwap;
        cSwap = c[i+3];
        c[i+3] = c[i+4];
        c[i+4] = cSwap;
      }
    }
  }

  if (total) {
    if (!checked_write(f, data, total, buffer))
      return AVS_ERROR;
  }

  /* swap data back if was swapped before writing */

  if (!(*(char *)(&swap_test))) {
    if (GetComponentSize()==2) {
      c = (char *)data;
      for (i=0; i<total; i+=2) {
        cSwap = c[i];
        c[i]   = c[i+1];
        c[i+1] = cSwap;
      }
    } else if (GetComponentSize()==4) {
      c = (char *)data;
      for (i=0; i<total; i+=4) {
        cSwap = c[i];
        c[i]   = c[i+3];
        c[i+3] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+2];
        c[i+2] = cSwap;
      }
    } else if (GetComponentSize()==8) {
      c = (char *)data;
      for (i=0; i<total; i+=8) {
        cSwap = c[i];
        c[i]   = c[i+7];
        c[i+7] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+6];
        c[i+6] = cSwap;
        cSwap = c[i+2];
        c[i+2] = c[i+5];
        c[i+5] = cSwap;
        cSwap = c[i+3];
        c[i+3] = c[i+4];
        c[i+4] = cSwap;
      }
    }
  }

WRITE_COORDS:
  float *points;
  points = (float *)malloc(sizeof(float)*GetNumberOfDimensions()*2);
  for (i=0; i<GetNumberOfDimensions(); i++) {
    points[i*2  ] = 0.1 *  GetOrigin(i);
    points[i*2+1] = 0.1 * (GetOrigin(i) + GetSpacing(i)*(GetDimensions(i)-1));
  }

  if (coords && !raw) {             		/* write AVS coordinates ? */
    coords *= sizeof(float);
    if (!(*(char *)(&swap_test))) {
      c = (char *)(points);              /* swap bytes */
      for (i=0; i<coords; i+=4) {
        cSwap = c[i];
        c[i]   = c[i+3];
        c[i+3] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+2];
        c[i+2] = cSwap;
      }
    }

    if (!checked_write(f, points, coords, buffer))
      return AVS_ERROR;

    if (!(*(char *)(&swap_test))) {
      c = (char *)(points);              /* swap bytes back */
      for (i=0; i<coords; i+=4) {
        cSwap = c[i];
        c[i]   = c[i+3];
        c[i+3] = cSwap;
        cSwap = c[i+1];
        c[i+1] = c[i+2];
        c[i+2] = cSwap;
      }
    }
  }

  if (!(tobuffer || getsize))
    if (f != fileno(stdout)) close(f);

  if (getsize) return;
  return AVS_OK;
}
