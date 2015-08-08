#include "globalfunctions.h"

#include <windows.h>

bool checkBoundaries(UVector2& pos, UVector2& leftTop, UVector2& squareSize)
{
	if(pos.x > leftTop.x && pos.x < leftTop.x + squareSize.x)
	{
		if(pos.y < leftTop.y && pos.y > leftTop.y - squareSize.y)
		{
			return true;
		}		
	}
	return false;
}

int loadfile(unsigned char** pbBuffer, unsigned long& dwArraySize, const char* strFilename)
{
	HANDLE hFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwBytesRead = 0;

	try
	{
		//Open a file
		hFile = ::CreateFile( strFilename,
							  FILE_READ_DATA,
							  FILE_SHARE_READ,
							  NULL,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
		//Check if the handle is valid
		if(hFile == INVALID_HANDLE_VALUE || hFile == NULL)
			throw(GetLastError());
		//Allocate space for the file
		dwFileSize = ::GetFileSize(hFile, NULL);

		if(dwFileSize < 0 || dwFileSize == INVALID_FILE_SIZE)
		{
			throw(GetLastError());
		}
		else
		{
			//Copy the buffer to memory.
			*pbBuffer = NULL;
			*pbBuffer = (unsigned char*)malloc(dwFileSize);		
		
			//Read the file
			if(!::ReadFile(hFile, *pbBuffer, dwFileSize, &dwBytesRead, NULL))
			{		
				throw(GetLastError());
			}

			if(dwFileSize != dwBytesRead)
			{
				throw(GetLastError());
			}
		}

		dwArraySize = dwFileSize;
		//Close the handle since it is not needed.
		::CloseHandle(hFile);
	}
	catch(DWORD Error)
	{
		//An error occurred so cleanup and exit.
		::CloseHandle(hFile);
		SAFE_DELETE_ARRAY(*pbBuffer);
		dwArraySize = 0;

		return Error;
	}

	//return 0 is succeed
	return 0;
}

int writefile(unsigned char* pbBuffer, unsigned long dwArraySize, const char* strFilename)
{
	HANDLE hFile = NULL;
	DWORD dwBytesWritten = 0;

	try
	{
		//Open a stream to a file.
		hFile = ::CreateFile( strFilename,
							  FILE_WRITE_DATA,
							  FILE_SHARE_WRITE,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);

		if(hFile == INVALID_HANDLE_VALUE || hFile == NULL)
			throw(GetLastError());
		//Write the pbBuffer into the file.
		if(!::WriteFile(hFile, pbBuffer, dwArraySize, &dwBytesWritten, NULL))
		{
			throw(GetLastError());
		}
		if(dwBytesWritten != dwArraySize)
		{
			throw(GetLastError());
		}
		//Close the handle since it not needed anymore.
		::CloseHandle(hFile);
	}
	catch(DWORD Error)
	{
		//An error occurred so close handle and exit.
		::CloseHandle(hFile);

		return Error;
	}

	return 0;
}

unsigned short getWordInverted(unsigned int Offset, unsigned char* pbData)
{
	if(pbData == NULL)
	{
		//If there is no array return 0.
		return 0;
	}
	else
	{
		//Invert the unsigned short and return.
		return pbData[Offset] + (pbData[Offset + 1]<< 8);
	}
}

unsigned long getDWordInverted(unsigned int Offset, unsigned char* pbData)
{
	if(pbData == NULL)
	{
		//If there is no array return 0.
		return 0;
	}
	else
	{
		//Return the unsigned long at pbData[Offset] inverted.
		return pbData[Offset] + (pbData[Offset + 1] << 8) + (pbData[Offset + 2] << 16) +( pbData[Offset + 3] << 24);
	}
}

void writeWord(unsigned int* Offset, unsigned short Value, unsigned char* pbData)
{
	//Write a word into the array first HIBYTE then LOBYTE.
	pbData[*Offset] = HIBYTE(Value);
	pbData[*Offset+1] = LOBYTE(Value);
	
	Offset+=2;
}

void writeDWord(unsigned int* Offset, unsigned long Value, unsigned char* pbData)
{
	//Write a double word into the array.
	pbData[*Offset] = HIBYTE(HIWORD(Value));
	pbData[*Offset+1] = LOBYTE(HIWORD(Value));
	pbData[*Offset+2] = HIBYTE(LOWORD(Value));
	pbData[*Offset+3] = LOBYTE(LOWORD(Value));
	
	Offset+=4;
}
