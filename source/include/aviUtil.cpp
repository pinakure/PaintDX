
#include "stdafx.h"


#define AVIIF_KEYFRAME  0x00000010L // this frame is a key frame.


#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>
#include <string>
#include "paintdx.hpp"

BOOL AVI_Init()
{
        /* first let's make sure we are running on 1.1 */
        WORD wVer = HIWORD(VideoForWindowsVersion());
        if (wVer < 0x010a){
             /* oops, we are too old, blow out of here */
             //MessageBeep(MB_ICONHAND);
             MessageBoxA(NULL, "Cant't init AVI File - Video for Windows version is to old", "Error", MB_OK|MB_ICONSTOP);
             return FALSE;
        }

        AVIFileInit();

        return TRUE;
}

BOOL AVI_FileOpenWrite(PAVIFILE * pfile, const char *filename)
{
        HRESULT hr = AVIFileOpenA(pfile,          // returned file pointer
                       filename,                  // file name
                       OF_WRITE | OF_CREATE,      // mode to open file with
                       NULL);                     // use handler determined
                                                  // from file extension....
        if (hr != AVIERR_OK)
                return FALSE;

        return TRUE;
}

DWORD getFOURCC(std::string value)
{
	if(!value.compare("DIB"))
	{
		return mmioFOURCC(value.at(0),value.at(1),value.at(2),' ');
	}
	else if(!(value.compare("CVID"))
		 || !(value.compare("IV32"))
		 || !(value.compare("MSVC"))
		 || !(value.compare("IV50")))
	{
		return mmioFOURCC(value.at(0), value.at(1), value.at(2), value.at(3));
	}
	else
	{
		return NULL;
	}
}

// Fill in the header for the video stream....
// The video stream will run in rate ths of a second....
BOOL AVI_CreateStream(PAVIFILE pfile, PAVISTREAM &ps, int rate, // sample/second
                      unsigned long buffersize, int rectwidth, int rectheight,
					  std::string _compressor)
{
		AVISTREAMINFOA strhdr;
		memset(&strhdr, 0, sizeof(strhdr));
        strhdr.fccType                = streamtypeVIDEO;// stream type
		//strhdr.fccHandler             = getFOURCC(_compressor.c_str());
        strhdr.fccHandler             = 0; // no compression!
		//strhdr.fccHandler             = mmioFOURCC('D','I','B',' '); // Uncompressed
		//strhdr.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
		//strhdr.fccHandler             = mmioFOURCC('I','V','3','2'); // Intel video 3.2
		//strhdr.fccHandler             = mmioFOURCC('M','S','V','C'); // Microsoft video 1
		//strhdr.fccHandler             = mmioFOURCC('I','V','5','0'); // Intel video 5.0
		//strhdr.dwFlags                = AVISTREAMINFO_DISABLED;
		//strhdr.dwCaps                 = 
		//strhdr.wPriority              = 
		//strhdr.wLanguage              = 
        strhdr.dwScale                = 1;
        strhdr.dwRate                 = rate;               // rate fps
		//strhdr.dwStart                =  
		//strhdr.dwLength               = 
		//strhdr.dwInitialFrames        = 
        strhdr.dwSuggestedBufferSize  = buffersize;
		strhdr.dwQuality              = -1; // use the default
		//strhdr.dwSampleSize           = 
        SetRect(&strhdr.rcFrame, 0, 0,              // rectangle for stream
            (int) rectwidth,
            (int) rectheight);
		//strhdr.dwEditCount            = 
		//strhdr.dwFormatChangeCount    =
		//strcpy(strhdr.szName, "Full Frames (Uncompressed)");

        // And create the stream;
        HRESULT hr = AVIFileCreateStreamA(pfile,             // file pointer
                                 &ps,                // returned stream pointer
                                 &strhdr);          // stream header
        if (hr != AVIERR_OK) {
                return FALSE;
        }

        return TRUE;
}

std::string getFOURCCVAsString(DWORD value)
{
	std::string returnValue = "";
	DWORD ch0 = value & 0x000000FF;
	returnValue += (char) ch0;
	DWORD ch1 = (value & 0x0000FF00)>>8;
	returnValue += (char) ch1;
	DWORD ch2 = (value & 0x00FF0000)>>16;
	returnValue += (char) ch2;
	DWORD ch3 = (value & 0xFF000000)>>24;
	returnValue += (char) ch3;

	return returnValue;
}

std::string dumpAVICOMPRESSOPTIONS(AVICOMPRESSOPTIONS opts)
{
	std::string tmp = "";
	std::string returnValue = "Dump of AVICOMPRESSOPTIONS\n";

	tmp = "DWORD  fccType = streamtype(" + getFOURCCVAsString(opts.fccType) + ")\n";
	returnValue += tmp;
 
	tmp = "DWORD  fccHandler = " + getFOURCCVAsString(opts.fccHandler) + "\n";
	returnValue += tmp;

	tmp = "DWORD  dwKeyFrameEvery = " + std::to_string(opts.dwKeyFrameEvery) + "\n";
	returnValue += tmp;

	tmp = "DWORD  dwQuality = " + std::to_string(opts.dwQuality) + "\n";
	returnValue += tmp;

	tmp = "DWORD  dwBytesPerSecond = " + std::to_string(opts.dwBytesPerSecond) + "\n";
	returnValue += tmp;

	if (opts.dwFlags & (AVICOMPRESSF_DATARATE == AVICOMPRESSF_DATARATE)) { tmp = "DWORD  fccType = AVICOMPRESSF_DATARATE\n"; }
	else if(opts.dwFlags & (AVICOMPRESSF_INTERLEAVE == AVICOMPRESSF_INTERLEAVE)){tmp = "DWORD  fccType = AVICOMPRESSF_INTERLEAVE\n";}
	else if(opts.dwFlags & (AVICOMPRESSF_KEYFRAMES == AVICOMPRESSF_KEYFRAMES)){tmp = "DWORD  fccType = AVICOMPRESSF_KEYFRAMES\n";}
	else if(opts.dwFlags & (AVICOMPRESSF_VALID == AVICOMPRESSF_VALID)){tmp = "DWORD  fccType = AVICOMPRESSF_VALID\n";}
	else { tmp = "DWORD  dwFlags = Unknown(" + std::to_string(opts.dwFlags) + ")\n"; }
	returnValue += tmp;

	tmp += "LPVOID lpFormat = " + std::to_string((int)opts.lpFormat) + "\n";
	returnValue += tmp;

	tmp += "DWORD  cbFormat = " + std::to_string((int)opts.cbFormat) + "\n";
	returnValue += tmp;

	tmp = "LPVOID lpParms = " + std::to_string((int)opts.lpParms) + "\n";
	returnValue += tmp;

	tmp = "DWORD  cbParms = " + std::to_string((int)opts.cbParms) + "\n";
	returnValue += tmp;

	tmp = "DWORD  dwInterleaveEvery = "+std::to_string(opts.dwInterleaveEvery)+"\n";
	
	returnValue += tmp;

	return returnValue;
}

BOOL AVI_SetOptions(PAVISTREAM &ps, PAVISTREAM &psCompressed, LPBITMAPINFOHEADER lpbi,
					std::string _compressor)
{
         
        AVICOMPRESSOPTIONS opts;
        AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};

		memset(&opts, 0, sizeof(opts));
		opts.fccType = streamtypeVIDEO;
		//opts.fccHandler             = getFOURCC(_compressor.c_str());
		opts.fccHandler  = 0;
		//opts.fccHandler            = mmioFOURCC('D','I','B',' '); // Uncompressed
		//opts.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
		//opts.fccHandler             = mmioFOURCC('I','V','3','2'); // Intel video 3.2
		//opts.fccHandler             = mmioFOURCC('M','S','V','C'); // Microsoft video 1
		//opts.fccHandler             = mmioFOURCC('I','V','5','0'); // Intel video 5.0
		//opts.dwKeyFrameEvery = 5;
		//opts.dwQuality
		//opts.dwBytesPerSecond
		//opts.dwFlags                = AVICOMPRESSF_KEYFRAMES;
		//opts.lpFormat 
		//opts.cbFormat
		//opts.lpParms
		//opts.cbParms 
		//opts.dwInterleaveEvery

		/* display the compression options dialog box if specified compressor is unknown */
		if(getFOURCC(_compressor.c_str()) == NULL)
		{
			if (!AVISaveOptions(NULL, 0, 1, &ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
			{
				return FALSE;
			}

			//printf("%s", dumpAVICOMPRESSOPTIONS(opts));
			MessageBoxA(NULL, dumpAVICOMPRESSOPTIONS(opts).c_str(), "AVICOMPRESSOPTIONS", MB_OK);
		}		

        HRESULT hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
        if (hr != AVIERR_OK) {
                return FALSE;
        }

        hr = AVIStreamSetFormat(psCompressed, 0,
                               lpbi,                    // stream format
                               32 * sizeof(RGBQUAD)
                                   );
        if (hr != AVIERR_OK) {
        return FALSE;
        }

        return TRUE;
}

BOOL AVI_SetText(PAVIFILE pfile, PAVISTREAM psText, char *szText, int width, int height, int TextHeight)
{
        // Fill in the stream header for the text stream....
        AVISTREAMINFOA strhdr;
        DWORD dwTextFormat;
        // The text stream is in 60ths of a second....

		memset(&strhdr, 0, sizeof(strhdr));
        strhdr.fccType                = streamtypeTEXT;
        strhdr.fccHandler             = mmioFOURCC('D', 'R', 'A', 'W');
        strhdr.dwScale                = 1;
        strhdr.dwRate                 = 60;
        strhdr.dwSuggestedBufferSize  = sizeof(szText);
        SetRect(&strhdr.rcFrame, 0, (int) height,
            (int) width, (int) height + TextHeight); // #define TEXT_HEIGHT 20

        // ....and create the stream.
        HRESULT hr = AVIFileCreateStreamA(pfile, &psText, &strhdr);
        if (hr != AVIERR_OK) {
                return FALSE;
        }

        dwTextFormat = sizeof(dwTextFormat);
        hr = AVIStreamSetFormat(psText, 0, &dwTextFormat, sizeof(dwTextFormat));
        if (hr != AVIERR_OK) {
                return FALSE;
        }

        return TRUE;
}

BOOL AVI_AddFrame(PAVISTREAM psCompressed, int time, LPBITMAPINFOHEADER lpbi)
{
	int ImageSize = lpbi->biSizeImage;
	if (ImageSize == 0)
	{
		if (lpbi->biBitCount == 24)
		{
			ImageSize = lpbi->biWidth * lpbi->biHeight * 3;
		}
	}
	HRESULT hr = AVIStreamWrite(psCompressed, // stream pointer
		time, // time of this frame
		1, // number to write
		(LPBYTE) lpbi + // pointer to data
		lpbi->biSize +
		lpbi->biClrUsed * sizeof(RGBQUAD),
		ImageSize, // lpbi->biSizeImage, // size of this frame
		AVIIF_KEYFRAME, // flags....
		NULL,
		NULL);
	if (hr != AVIERR_OK)
	{
		std::string str;
		
		str = "Error: AVIStreamWrite, error "+std::to_string(hr);
		MessageBoxA(nullptr, str.c_str(), "Error", MB_OK);
		return FALSE;
	}
	
	return TRUE;
}

BOOL AVI_AddText(PAVISTREAM psText, int time, char *szText)
{
        int iLen = strlen(szText);

        HRESULT hr = AVIStreamWrite(psText,
                        time,
                        1,
                        szText,
                        iLen + 1,
                        AVIIF_KEYFRAME,
                        NULL,
                        NULL);
        if (hr != AVIERR_OK)
                return FALSE;

        return TRUE;
}

BOOL AVI_CloseStream(PAVISTREAM ps, PAVISTREAM psCompressed, PAVISTREAM psText)
{
        if (ps)
                AVIStreamClose(ps);

        if (psCompressed)
                AVIStreamClose(psCompressed);

        if (psText)
                AVIStreamClose(psText);



        return TRUE;
}

BOOL AVI_CloseFile(PAVIFILE pfile)
{
        if (pfile)
                AVIFileClose(pfile);
        
        return TRUE;
}

BOOL AVI_Exit()
{
        AVIFileExit();

        return TRUE;
}


/* Here are the additional functions we need! */


PAVIFILE pfile = NULL; 
PAVISTREAM ps = NULL;
PAVISTREAM psCompressed = NULL; 
int count = 0;

// Initialization... 
bool START_AVI(std::string file_name)
{
    if(! AVI_Init())
	{
		//printf("Error - AVI_Init()\n");
		return false;
	}

    if(! AVI_FileOpenWrite(&pfile, file_name.c_str()))
	{
		//printf("Error - AVI_FileOpenWrite()\n");
		return false;
	}

	return true;
}
  
//Now we can add frames
// ie. ADD_FRAME_FROM_DIB_TO_AVI(yourDIB, "CVID", 25);
bool ADD_FRAME_FROM_DIB_TO_AVI(BITMAPINFOHEADER &info, HANDLE dib, std::string _compressor, int _frameRate)
{
	LPBITMAPINFOHEADER lpbi = &info;
	if(count == 0)
	{
		//lpbi = (LPBITMAPINFOHEADER)GlobalLock(dib);
		int msg = GetLastError();
		
		if(! AVI_CreateStream(pfile, ps, _frameRate, 
			(unsigned long) lpbi->biSizeImage, 
			(int) lpbi->biWidth, 
			(int) lpbi->biHeight, _compressor.c_str()))
		{
			//printf("Error - AVI_CreateStream()\n");
			GlobalUnlock(lpbi);
			return false;
		} 

		if(! AVI_SetOptions(ps, psCompressed, lpbi, _compressor.c_str()))
		{
			//printf("Error - AVI_SetOptions()\n");
			GlobalUnlock(lpbi);
			return false;
		}

		//GlobalUnlock(lpbi);
	}

	//lpbi = (LPBITMAPINFOHEADER)GlobalLock(dib); 
	if(! AVI_AddFrame(psCompressed, count * 1, lpbi))
	{
		return false;
	}

	//GlobalUnlock(lpbi); 
	count++;
	SetWindowTextA((HWND)PaintDX::handle, std::to_string(count).c_str());
	return true;
}

// The end... 
bool STOP_AVI()
{
     if(! AVI_CloseStream(ps, psCompressed, NULL))
	 {
		 //printf("Error - AVI_CloseStream()\n");
		 return false;
	 }

     if(! AVI_CloseFile(pfile))
	 {
		//printf("Error - AVI_CloseFile()\n");
		return false;
	 }
	 
     if(! AVI_Exit())
	 {
		//printf("Error - AVI_Exit()\n");
		return false;
	 }

	 return true;
} 

extern void *GPUGetSnapShot();
bool recording = false;

void startAvi() {
	if (!recording) {
		START_AVI("aviout.avi");
		recording = true;
	}
}

void stopAvi() {
	if (recording) {
		STOP_AVI();
		recording = false;
	}
}

extern int VWIDTH;
extern int VHEIGHT;

void feedAvi() {
	if (!recording)return;
	
	BITMAPINFO bi;
	bi.bmiHeader.biSize = 40;
	bi.bmiHeader.biWidth = VWIDTH;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biHeight = VHEIGHT;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = VWIDTH;
	bi.bmiHeader.biYPelsPerMeter = VHEIGHT;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	
	HDC hdc = GetDC((HWND)PaintDX::handle);
	HBITMAP hbm = (HBITMAP)GPUGetSnapShot();
	ADD_FRAME_FROM_DIB_TO_AVI(bi.bmiHeader, hbm, "DIB", 30);
	
	DeleteObject(hbm);
	
}

