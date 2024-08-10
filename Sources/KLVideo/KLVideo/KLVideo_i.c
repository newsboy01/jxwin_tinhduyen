/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Mar 02 21:36:23 2024
 */
/* Compiler settings for D:\SOURCE_TINHDUYEN\SwordOnline\SwordOnline\Sources\KLVIDEO\KLVideo\KLVideo.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_KLVIDEOLib = {0x1C51AA48,0x4DC5,0x4FE0,{0x9B,0x0B,0x07,0x85,0x99,0x4D,0x5A,0x21}};


#ifdef __cplusplus
}
#endif

