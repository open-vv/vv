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

// std include
#include <iostream>
#include <fstream>
#include <sstream>

//defines
#define MAXDIM 5
#ifdef _WIN32
#ifdef memicmp
#undef memicmp
#endif
#define memicmp _memicmp
#else
#define memicmp strncasecmp
#endif


// Based on a true story by the Nederlands Kanker Instituut (AVS_RXDR.CPP from the 20080414)
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

/************************************************************************/
/*                    DEFINES, ENUMERATED TYPES AND CONSTANTS           */
/************************************************************************/
enum {
  OK,
  ER_ILLCOMMFUNCT,
  ER_INARGUMENTS,
  ER_XDR_NDIM,
  ER_XDR_DIM,
  ER_XDR_NSPACE,
  ER_XDR_VECLEN,
  ER_XDR_DATA,
  ER_XDR_FIELD,
  ER_XDR_OPEN,
  ER_XDR_NOCTRLL,
  ER_XDR_READ,
  ER_OUTOFMEMORY,
  ER_DECOMPRESSION,
  ER_NOT_HANDLED
};

typedef struct {
  unsigned int iOrgSize;
  unsigned int iMode;
  unsigned int iCompressedSize;
  unsigned int iOrgCRC;
  unsigned int iCompressedCRC;	/* Excluding this header */
} NKI_MODE2;


/************************************************************************/
/*                             GLOBAL VARIABLES                         */
/************************************************************************/
const char* gl_ErrorMsg[] = {
  "",
  "Command or function not supported in this way.",
  "Error in arguments",
  "XDR file header NDIM error",
  "XDR file header DIMn error",
  "XDR file header NSPACE error",
  "XDR file header VECLEN error",
  "XDR file header DATA(type) error",
  "XDR file header FIELD(coordinate type) error",
  "XDR file could not be opened",
  "XDR file header contains no ^L",
  "XDR file reading error",
  "Out of memory",
  "Decompression failed",
  "Format not handled by clitkXdrImageIO (RECTILINEAR or IRREGULAR field)"
};


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

/* simple XDR file reader */

/* help routine, scans XDR file header for keyword entry, returns NULL
   if not found, else returns pointer to rest of line
*/

static char *scan_header(const char *file, const char *name, int offset, int removespaces)
{
  int i, j, iStringLength;
  static char temp[512];
  FILE *f;
  char *p, *q;

  if ((f = fopen(file, "rt")) == NULL) return NULL;
  if (offset) fseek(f, offset, SEEK_SET);

  for (i=0; i<200; ) {
    if (fgets(temp, 500, f) == NULL      ) break;       /* end of file */

    if (removespaces) {
      temp[500] = 0;
      p = q = temp;			/* remove spaces */
      iStringLength = strlen(temp);
      for (j=0; j<iStringLength; j++)
        if (*q!=' ' && *q!=8) *p++ = *q++;
        else q++;
      *p++ = 0;
    }

    if (temp[0] == 12                    ) break;       /* ^L end of header */
    if (temp[0] != '#') i++;                            /* The first 200 non comment lines must be read before data is opened. */
    if ((p = strchr(temp+1, '=')) == NULL) continue;    /* no '=' */
    if (memicmp(temp, name, p-temp)      ) continue;    /* no match */

    p++;                                                /* match, skip = */
    if (p[strlen(p)-1] == '\n')                         /* remove \n */
      p[strlen(p)-1] = 0;

    fclose (f);
    return p;
  }

  fclose(f);
  return NULL;
}

static int get_nki_compressed_size(FILE *f)
{
  NKI_MODE2		Header;
  int			iMode;

  fread((void *)&Header, sizeof(Header), 1 , f);

  iMode = Header.iMode;

  switch (iMode) {
  case  1:
  case  3:
    return 0;
  case  2:
  case  4:
    return Header.iCompressedSize + sizeof(Header);
  default:
    return 0;
  }
}

/* decoder for NKI private compressed pixel data
   arguments: dest    = (in) points to area where destination data is written (short)
              src     = (in) points compressed source data (byte stream)
              size    = (in) number of bytes source data (safety)

   The return value is the number of pixels that have been processed.

   The compressed data looks like:
   (number of pixels)-1 times:
     OR 1 byte   = LL     7  bits signed (difference pixel[1] - pixel[0]);
     OR 2 bytes  = HHLL   15 bits signed (difference pixel[1] - pixel[0]) xored with 0x4000;
     OR 3 bytes  = 7FHHLL 16 bits absolute pixel data if 15 bits difference is exceeded
     OR 2 bytes  = 80NN   run length encode NN zero differences (max 255)
for mode 3 and 4 added:
     OR 2 bytes  = CONN   encode NN 4 bit differences (max 255)

   Performance on typical CT or MRI is >2x compression and a very good speed

   This code is not valid on HIGHENDIAN (high byte first) machines
*/

static int global_len;

static int nki_private_decompress(short int *dest, signed char *src, int size)
{
  int			npixels, retvalue, mode, iMode, val, j;
  NKI_MODE2*		pHeader = (NKI_MODE2*)src;
  unsigned long		iCRC=0, iCRC2=0;
  //unsigned char*	pDestStart = (unsigned char*)dest;
  signed char           *save, *end;

  retvalue = npixels = pHeader->iOrgSize;
  iMode = pHeader->iMode;		// safety: this value is checked in case statement

  if (npixels<1) return 0;		// safety: check for invalid npixels value

  /* Up till now only Mode=1, 2, 3, and 4 are supported */

  switch (iMode) {
  case 1:
    save = src;

    src += 8;				// mode 1 only has 8 bytes header: iOrgSize and iMode
    end  = src + size - 3;		// for overflow check if we are close to end of input buffer

    *dest = *(short int *)src;
    src += 2;
    npixels--;

    do {
      if (src > end) {		// check whether the last few messages fit in input buffer
        if (src<end+3) val = *src;
        else           val = 0;

        if (val >= -64 && val <= 63)      mode = 1;	// 7 bit difference
        else if (val==0x7f)		    mode = 3;	// 16 bit value
        else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
        else				    mode = 2;

        if (src+mode > end+3)
          return 0;			// safety: overflow input data
      }

      val = *src;

      if (val >= -64 && val <= 63) {	// 7 bit difference
        dest[1] = dest[0] + val;
        dest++;
        src++;
      } else if (val==0x7f) {	// 16 bit value
        dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
        dest++;
        src+=3;
      } else if ((val&0xff)==0x80) {	// run length encoding
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        if (npixels<=0) return 0;	// safety: overflow output data
        do {
          dest[1] = dest[0];
          dest++;
        } while (--mode);
        src+=2;
      } else {
        signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
        dest[1] = dest[0] + diff;	// 15 bit difference
        dest++;
        src+=2;
      }
    } while (--npixels);

    global_len = src-save;

    break;

  case 2:
    src += sizeof(NKI_MODE2);
    save = src;
    end  = src + pHeader->iCompressedSize - 3;

    if (end > src + size - 3)
      end = src + size - 3;		// may occur if pHeader is corrupted

    *dest = val = *(short int *)src;
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
    src+=2;

    npixels--;

    do {
      if (src > end) {		// check whether the last few messages fit in input buffer
        if (src<end+3) val = *src;
        else           val = 0;

        if (val >= -64 && val <= 63)      mode = 1;	// 7 bit difference
        else if (val==0x7f)		    mode = 3;	// 16 bit value
        else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
        else				    mode = 2;

        if (src+mode > end+3)
          break;			// safety: overflow input data
      }

      val = *src;

      if (val >= -64 && val <= 63) {	// 7 bits difference
        dest[1] = val = dest[0] + val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src++;
      } else if (val==0x7f) {	// 16 bit value
        dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];

        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src+=3;
      } else if ((val&0xff)==0x80) {	// run length encoding
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        if (npixels<=0) break;	// safety: overflow output data
        do {
          dest[1] = val = dest[0];
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
        } while (--mode);
        src+=2;
      } else {
        signed short diff = ((val^0x40)<<8) + ((unsigned char *)src)[1];
        dest[1] = val = dest[0] + diff;	// 15 bit difference
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src+=2;
      }
    } while (--npixels);

    if (iCRC2 != pHeader->iOrgCRC) {	// if error in output CRC:
      src = save;			// check input CRC
      while (src < end) {
        iCRC = CRC32_table[(unsigned char)iCRC ^ (unsigned char)src[0]] ^ ((iCRC >> 8));
        src++;
      }

      if (iCRC != pHeader->iCompressedCRC) {
        AVSerror("XDR decompression: the file is corrupted");
        retvalue=0;
      } else {
        AVSerror("XDR decompression: internal error");
        retvalue=0;
      }
    }

    global_len = sizeof(NKI_MODE2) + pHeader->iCompressedSize;

    break;

  case 3:
    save = src;

    src += 8;				// mode 3 only has 8 bytes header: iOrgSize and iMode
    end  = src + size - 3;		// for overflow check if we are close to end of input buffer

    *dest = *(short int *)src;
    src += 2;
    npixels--;

    do {
      if (src > end) {		// check whether the last few messages fit in input buffer
        if (src<end+3) val = *src;
        else           val = 0;

        if (val >= -63 && val <= 63)      mode = 1;	// 7 bit difference
        else if (val==0x7f)		    mode = 3;	// 16 bit value
        else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
        else if ((val&0xff)==0xC0)	    mode = 2;	// 4 bit encoding
        else				    mode = 2;

        if (src+mode > end+3)
          return 0;			// safety: overflow input data
      }

      val = *src;

      if (val >= -63 && val <= 63) {	// 7 bit difference
        dest[1] = dest[0] + val;
        dest++;
        src++;
      } else if (val==0x7f) {	// 16 bit value
        dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
        dest++;
        src+=3;
      } else if ((val&0xff)==0x80) {	// run length encoding
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        if (npixels<=0) return 0;	// safety: overflow output data
        do {
          dest[1] = dest[0];
          dest++;
        } while (--mode);
        src+=2;
      } else if ((val&0xff)==0xC0) {	// 4 bit run
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        mode/=2;
        src+=2;
        if (npixels<=0) return 0;	// safety: overflow output data
        do {
          val = *src++;
          dest[1] = dest[0] + (val>>4);
          dest++;
          if (val&8) val |= 0xfffffff0;
          else val &= 0x0f;
          dest[1] = dest[0] + val;
          dest++;
        } while (--mode);
      } else {
        signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
        dest[1] = dest[0] + diff;	// 15 bit difference
        dest++;
        src+=2;
      }
    } while (--npixels);

    global_len = src-save;

    break;

  case 4:
    src += sizeof(NKI_MODE2);
    save = src;
    end  = src + pHeader->iCompressedSize - 3;

    if (end > src + size - 3)
      end = src + size - 3;		// may occur if pHeader is corrupted

    *dest = val = *(short int *)src;
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
    iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
    src += 2;
    npixels--;

    do {
      if (src > end) {		// check whether the last few messages fit in input buffer
        if (src<end+3) val = *src;
        else           val = 0;

        if (val >= -63 && val <= 63)      mode = 1;	// 7 bit difference
        else if (val==0x7f)		    mode = 3;	// 16 bit value
        else if ((val&0xff)==0x80)	    mode = 2;	// run length encoding
        else if ((val&0xff)==0xC0)	    mode = 2;	// 4 bit encoding
        else				    mode = 2;

        if (src+mode > end+3)
          return 0;			// safety: overflow input data
      }

      val = *src;

      if (val >= -63 && val <= 63) {	// 7 bit difference
        dest[1] = val = dest[0] + val;
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src++;
      } else if (val==0x7f) {	// 16 bit value
        dest[1] = val = ((int)(((unsigned char *)src)[1])<<8) + ((unsigned char*)src)[2];
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src+=3;
      } else if ((val&0xff)==0x80) {	// run length encoding
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        if (npixels<=0) return 0;	// safety: overflow output data
        do {
          dest[1] = val = dest[0];
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
          dest++;
        } while (--mode);
        src+=2;
      } else if ((val&0xff)==0xC0) {	// 4 bit run
        mode = ((unsigned char *)src)[1];
        npixels -= mode-1;
        mode/=2;
        src+=2;
        if (npixels<=0) return 0;	// safety: overflow output data
        do {
          val = *src++;
          dest[1] = j = dest[0] + (val>>4);
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)j] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(j>>8)] ^ ((iCRC2 >> 8));
          dest++;
          if (val&8) val |= 0xfffffff0;
          else val &= 0x0f;
          dest[1] = j = dest[0] + val;
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)j] ^ ((iCRC2 >> 8));
          iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(j>>8)] ^ ((iCRC2 >> 8));
          dest++;
        } while (--mode);
      } else {
        signed short diff = ((val^0x40)<<8) + (unsigned char)(src[1]);
        dest[1] = val = dest[0] + diff;	// 15 bit difference
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)val] ^ ((iCRC2 >> 8));
        iCRC2 = CRC32_table[(unsigned char)iCRC2 ^ (unsigned char)(val>>8)] ^ ((iCRC2 >> 8));
        dest++;
        src+=2;
      }
    } while (--npixels);

    if (iCRC2 != pHeader->iOrgCRC)	// if error in output CRC:
      retvalue=0;

    global_len = sizeof(NKI_MODE2) + pHeader->iCompressedSize;

    break;


  default:
    AVSerror("XDR decompression: unsupported mode");
    return 0;
  }

  return retvalue;
}


//====================================================================
// Read image information (copied from XDRreader)
int clitk::XdrImageIO::ReadImageInformationWithError()
{
  int      offset=0;
  itk::Vector<int,MAXDIM> dim;
  int      veclen=1;
  int      total=1;
  unsigned int coords=0,i,j,ndim,nspace;
  char     temp[512];
  FILE     *fstream;
  char     *c;

  long     swap_test = 0x1000000;      /* first byte is 1 when low-endian */
  forcenoswap=0;
  char     *file = const_cast<char *>(m_FileName.c_str());
  AVSType  field=UNIFORM;


  fstream = fopen(file, "rt");
  if (fstream == NULL) return ER_XDR_OPEN;

  fgets(temp, 500, fstream);
  fclose(fstream);

  if (memcmp(temp, "# AVS field file (produced by avs_nfwrite.c)", 44)==0) forcenoswap=1;

  c      = scan_header(file, "ndim", offset, 1);
  if (!c) return ER_XDR_NDIM;

  ndim   = atoi(c);
  if (ndim<1 || ndim>MAXDIM) return ER_XDR_NDIM;
  SetNumberOfDimensions(ndim);

  nspace = ndim;

  for (i=0; i<ndim; i++) {
    sprintf(temp, "dim%d", i+1);
    c = scan_header(file, temp, offset, 1);
    if (!c) return ER_XDR_DIM;
    dim[i]=atoi(c);
    if (dim[i]<1) return ER_XDR_DIM;

    total  *= dim[i];
    coords += dim[i];
  }
  for (i=0; i<ndim; i++) {
    SetDimensions(i,dim[i]);
    SetSpacing(i,1.);
    SetOrigin(i,0.);
  }

  c = scan_header(file, "nspace", offset, 1);
  if (c) nspace = atoi(c);
  if (nspace<1 || ndim > MAXDIM) return ER_XDR_NSPACE;
  if (nspace != ndim) return ER_NOT_HANDLED;

  c = scan_header(file, "veclen", offset, 1);
  if (c) veclen = atoi(c);
  if (veclen<0 /*|| veclen>1000*/) return ER_XDR_VECLEN;
  SetNumberOfComponents(veclen);
  if (veclen==1) SetPixelType(itk::ImageIOBase::SCALAR);
  else          SetPixelType(itk::ImageIOBase::VECTOR);

  c = scan_header(file, "data", offset, 1);
  if (c) {
    if (memicmp(c, "byte",  4) == 0 || memicmp(c, "xdr_byte",  8) == 0) SetComponentType(itk::ImageIOBase::CHAR);
    else if (memicmp(c, "short", 5) == 0 || memicmp(c, "xdr_short", 9) == 0) SetComponentType(itk::ImageIOBase::SHORT);
    else if (memicmp(c, "int" ,  3) == 0 || memicmp(c, "xdr_int" ,  7) == 0) SetComponentType(itk::ImageIOBase::INT);
    else if (memicmp(c, "real",  4) == 0 || memicmp(c, "xdr_real",  8) == 0) SetComponentType(itk::ImageIOBase::FLOAT);
    else if (memicmp(c, "float", 5) == 0 || memicmp(c, "xdr_float", 9) == 0) SetComponentType(itk::ImageIOBase::FLOAT);
    else if (memicmp(c, "double",6) == 0 || memicmp(c, "xdr_double",10)== 0) SetComponentType(itk::ImageIOBase::DOUBLE);
    else return ER_XDR_DATA;

    if (memicmp(c, "xdr_",  4) == 0) forcenoswap=0;
  }

  //Read coords here
  c = scan_header(file, "field", offset, 1);
  if (c) {
    if (memicmp(c, "unifo", 5) == 0) field=UNIFORM, coords=nspace *2;
    else if (memicmp(c, "recti", 5) == 0) field=RECTILINEAR;
    else if (memicmp(c, "irreg", 5) == 0) field=IRREGULAR, coords=total*nspace;
    else return ER_XDR_FIELD;
  } else
    coords=0;

  if (coords) {                      /* expect AVS coordinates ? */
    coords *= sizeof(float);
    fstream = fopen(m_FileName.c_str(), "rb");
    if (fstream == NULL) return ER_XDR_OPEN;

    float *points = (float *)malloc(coords);
    if (points == NULL) return ER_OUTOFMEMORY;

    //Seek to coordinates position in file
    if (fseek(fstream,-static_cast<int>(coords),SEEK_END)) return ER_XDR_READ;
    if (fread( /*(*output)->*/points, 1, coords, fstream ) == coords) {
      /* swap data if read-ok and required (xdr is low-endian) */
      if (!(*(char *)(&swap_test)) && !forcenoswap) {
        c = (char *)/*(*output)->*/points;
        for (i=0; i<coords; i+=4) {
          j = c[i];
          c[i]   = c[i+3];
          c[i+3] = j;
          j = c[i+1];
          c[i+1] = c[i+2];
          c[i+2] = j;
        }
      }
    }

    float *p = points;
    switch (field) {
    case UNIFORM:
      for (i=0; i<GetNumberOfDimensions(); i++) {
        SetSpacing(i,10.*(points[i*2+1]-points[i*2])/(GetDimensions(i)-1));
        SetOrigin(i,10.*points[i*2]);
      }
      break;
    case RECTILINEAR:
      //Rectilinear is reinterpreted as uniform because ITK does not know rectilinear
      //Error if fails
      for (i=0; i<GetNumberOfDimensions(); i++) {
        //Compute mean spacing
        SetSpacing(i,10*(p[GetDimensions(i)-1]-p[0])/(GetDimensions(i)-1));
        SetOrigin(i,10*p[0]);

        //Test if rectilinear image is actually uniform (tolerance 0.1 mm)
        if(i<3) { // Only for first 3 dimensions because spacing is barely used in other dims
          for (j=0; j<GetDimensions(i)-1; j++) {
            if (fabs((p[j+1]-p[j])*10-GetSpacing(i))>0.1) {
              free(points);
              fclose(fstream);
              return ER_NOT_HANDLED;
            }
          }
        }
        p += (int)GetDimensions(i);
      }
      break;
    case IRREGULAR:
      free(points);
      fclose(fstream);
      return ER_NOT_HANDLED;
    }
    free(points);
    fclose(fstream);
  }
  return OK;
}

//====================================================================
// Read image information (copied from XDRreader)
void clitk::XdrImageIO::ReadImageInformation()
{
  int result = ReadImageInformationWithError();
  if (result) ITKError("clitk::XdrImageIO::ReadImageInformation",result);
}


//====================================================================
// Read Image Content (copied from Xdr reader)
int clitk::XdrImageIO::ReadWithError(void * buffer)
{
  //AVSINT   dim[5];
  int      /*ndim,*/ nspace/*, veclen=1, data=AVS_TYPE_BYTE, field=UNIFORM*/;
  int      iNkiCompression = 0;
  int      j, coords=0,  datasize=0, HeaderSize;
  unsigned int i,iNumRead,total=1;
  char     temp[512];
  FILE     *fstream;
  char     *c;
  char     *buff;
  //AVSfield FieldTemplate;
  long     swap_test = 0x1000000;      /* first byte is 1 when low-endian */
  //int      forcenoswap=0;
  char     *file = const_cast<char *>(m_FileName.c_str());
  int      offset=0;
  // AVSType  field=UNIFORM;

  for (i=0; i<GetNumberOfDimensions(); i++) coords += GetDimensions(i);

  total = GetImageSizeInPixels();
  nspace = GetNumberOfDimensions();

  c = scan_header(file, "field", offset, 1);
  if (c) {
    if (memicmp(c, "unifo", 5) == 0) /*field=UNIFORM,*/ coords=nspace*2;
    else if (memicmp(c, "recti", 5) == 0) /*field=RECTILINEAR*/;
    else if (memicmp(c, "irreg", 5) == 0) /*field=IRREGULAR,*/ coords=total*nspace;
    else return ER_XDR_FIELD;
  } else
    coords=0;

  c = scan_header(file, "nki_compression", offset, 1);
  if (c) iNkiCompression = atoi(c);

  c = scan_header(file, "coord1[0]", offset, 1);
  if (c) HeaderSize = 32768;
  else HeaderSize = 2048;

  fstream = fopen(file, "rb");
  if (fstream == NULL)
    return ER_XDR_OPEN;

  if (offset) fseek(fstream, offset, SEEK_SET);

  while (1) {
    if (fgets(temp, 500, fstream) == NULL )
      return ER_XDR_NOCTRLL; /* end of file */

    if (temp[0] == 10) continue;

    if (temp[0] == 12) {
      fseek(fstream, -2, SEEK_CUR);
      break;
    } /* ^L end of header */

    if (temp[0] != '#') break;
  }

  buff = (char*)malloc(HeaderSize);
  if (buff == NULL) {
    return ER_OUTOFMEMORY;
  }
  memset(buff, 0, HeaderSize);
  iNumRead = fread(buff, 1, HeaderSize, fstream);
  if (iNumRead < 1) {
    free(buff);
    fclose(fstream);
    return ER_XDR_READ;
  }

  for (i=0; i<iNumRead; i++) {
    if (buff[i] == 12) break;
  }

  free(buff);

  if (i==iNumRead) return ER_XDR_NOCTRLL;

  total = GetImageSizeInBytes();

  //We add casts because the resulting quantity can be negative.
  //There is no risk of looping because i and iNumRead are about the size of the header
  fseek(fstream, static_cast<int>(i)+2-static_cast<int>(iNumRead), SEEK_CUR);

  if (total && iNkiCompression) {
    long		iCurPos;
    unsigned long iSize;
    signed char*	pCompressed;

    /* Read or guess the size of the compressed data */
    iCurPos = ftell(fstream);
    iSize = get_nki_compressed_size(fstream);

    if (iSize==0) {
      fseek(fstream, 0, SEEK_END);
      iSize = ftell(fstream);
      iSize = iSize - iCurPos - coords;

      // Get compressed size from header if possible; else use uncompressed size as safe estimate
      if (iSize>total && offset) iSize=total+8;
    }

    fseek(fstream, iCurPos, SEEK_SET);

    /* Allocate space for the compressed pixels */
    pCompressed = (signed char*)malloc(iSize);
    if (!pCompressed) {
      fclose(fstream);
      return ER_OUTOFMEMORY;
    }

    /* Read the compressed pixels */
    if (fread( (void *)pCompressed, 1, iSize, fstream ) != iSize) {
      fclose(fstream);
      return ER_XDR_READ;
    }

    if (!nki_private_decompress((short*)buffer, pCompressed, iSize)) {
      fclose(fstream);
      return ER_DECOMPRESSION;
    }

    // if (offset)
    fseek(fstream, iCurPos + global_len, SEEK_SET);

    free(pCompressed);
    goto READ_COORDS;
  }


  if (total) {
    if (fread( (void *)buffer, 1, total, fstream ) != total) {
      fclose(fstream);
      return ER_XDR_READ;
    }
  }

  /* swap data if required (xdr is low-endian) */

  datasize = GetComponentSize();
  if (!(*(char *)(&swap_test)) && !forcenoswap) {
    if (datasize==2) {
      c = (char *)buffer;
      for (i=0; i<total; i+=2) {
        j = c[i];
        c[i]   = c[i+1];
        c[i+1] = j;
      }
    } else if (datasize==4) {
      c = (char *)buffer;
      for (i=0; i<total; i+=4) {
        j = c[i];
        c[i]   = c[i+3];
        c[i+3] = j;
        j = c[i+1];
        c[i+1] = c[i+2];
        c[i+2] = j;
      }
    } else if (datasize==8) {
      c = (char *)buffer;
      for (i=0; i<total; i+=8) {
        j = c[i];
        c[i]   = c[i+7];
        c[i+7] = j;
        j = c[i+1];
        c[i+1] = c[i+6];
        c[i+6] = j;
        j = c[i+2];
        c[i+2] = c[i+5];
        c[i+5] = j;
        j = c[i+3];
        c[i+3] = c[i+4];
        c[i+4] = j;
      }
    }
  }

READ_COORDS:
  fclose(fstream);
  return OK;
}

//====================================================================
// Read image information (copied from Xdr reader)
void clitk::XdrImageIO::Read(void * buffer)
{
  int result = ReadWithError(buffer);
  if (result) ITKError("clitk::XdrImageIO::Read",result);
}

//====================================================================
// Read Image Information
bool clitk::XdrImageIO::CanReadFile(const char* FileNameToRead)
{
  char     temp[512];
  FILE     *fstream;

  fstream = fopen(FileNameToRead, "rt");
  if (fstream == NULL)
//    {
//        AVSerror("Couldn't open file " << FileNameToRead);
    return false;
//    }
  fgets(temp, 500, fstream);
  fclose(fstream);

  if (memcmp(temp, "# AVS", 5)==0)
    return true;
  else
    return false;
} ////

void clitk::XdrImageIO::ITKError(std::string funcName, int msgID)
{
  itkExceptionMacro(<< "Error in " << funcName << ". Message: " << gl_ErrorMsg[msgID]);
}

