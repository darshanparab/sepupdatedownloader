#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <fstream>
#include <regex>
#include <string>

using namespace std;


HANDLE hFile;
HINTERNET hINETOPEN, hINETURL;
TCHAR uAGENT[15]="SEPDOWNLOADER",*DATA = NULL, SEPU32[23],SEPU64[23],URL[100],FILENAME[25];
DWORD ACCESSTYPE = INTERNET_OPEN_TYPE_PRECONFIG,BLOCKSIZE=1000000, RBYTES=0, BytesWrittenToDisk,BytesWritten;

class GetString
{
	private:
		TCHAR *TSTR;
	public:
		TCHAR *convert(string SSTR);
		~GetString();
};

TCHAR *GetString::convert(string SSTR)
{
	TSTR = new TCHAR[SSTR.length()+1];
	TSTR[SSTR.length()]=0;
	
	for(int i=0;i<SSTR.length();i++)
	{
		memcpy(&TSTR[i],&SSTR[i],sizeof(SSTR[i]));
	}
	cout<<TSTR<<endl;
	return TSTR;
}

GetString::~GetString()
{
	delete[] TSTR;
}

/*--------------------------------------- Download data ---------------------------------------*/
int download(TCHAR *LINK, TCHAR *FILE)
{
/*--------------------------------------- Open download URL ---------------------------------------*/	
	BytesWrittenToDisk=0;
	hINETURL = InternetOpenUrl(hINETOPEN,LINK,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0);
	if(!hINETURL) { return 0; }

/*--------------------------------------- Create file on disk to store downloaded data ---------------------------------------*/
	hFile = CreateFile(FILE,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

/*--------------------------------------- Download file ---------------------------------------*/
	do
	{
		DATA = new TCHAR[BLOCKSIZE];
		ZeroMemory(DATA,BLOCKSIZE);
		InternetReadFile(hINETURL,(LPVOID)DATA,BLOCKSIZE,&RBYTES);
		WriteFile(hFile,DATA,RBYTES,&BytesWritten,NULL);
		delete[] DATA;
		DATA = NULL;
		BytesWrittenToDisk += RBYTES;
		system("cls");
		cout<<BytesWrittenToDisk/1000000<<" MB Downloaded."<<endl;
	} while(RBYTES);
	
	if(hFile) { CloseHandle(hFile); }
	if(hINETURL) { InternetCloseHandle(hINETURL); }
	
	return 0;
}
/*--------------------------------------- End of function ---------------------------------------*/

int main(int argc, char** argv)
{
	TCHAR INDEX[300]="https://www.symantec.com/security_response/definitions/download/detail.jsp?gid=sep";
	string LINE;
	regex REG32("http:\\/\\/definitions\\.symantec\\.com\\/defs\\/[0-9]{8}-[0-9]{3}-v5i32\\.exe"), REG64("http:\\/\\/definitions\\.symantec\\.com\\/defs\\/[0-9]{8}-[0-9]{3}-v5i64\\.exe"), SEPU32("[0-9]{8}-[0-9]{3}-v5i32\\.exe"), SEPU64("[0-9]{8}-[0-9]{3}-v5i64\\.exe");
	smatch URL32, URL64, FILENAME32, FILENAME64;
	ifstream INDEXFILE;
	GetString Test;

	
/*--------------------------------------- Open internet connection ---------------------------------------*/
	hINETOPEN=InternetOpen(uAGENT,ACCESSTYPE,NULL,NULL,0);
	if(!hINETOPEN) { goto END_PROGRAM; }

/*--------------------------------------- Download index file ---------------------------------------*/
	download(INDEX,"index.txt");
	INDEXFILE.open("index.txt");
	while(getline(INDEXFILE,LINE))
	{
		if(regex_search(LINE,URL32,REG32))
		{
			if(regex_search(LINE,FILENAME32,SEPU32))
			{
				strcpy(URL,Test.convert(URL32.str(0)));
				strcpy(FILENAME,Test.convert(FILENAME32.str(0)));
				download(URL,FILENAME);
			}
		}
		else if(regex_search(LINE,URL64,REG64))
		{
			if(regex_search(LINE,FILENAME64,SEPU64))
			{
				strcpy(URL,Test.convert(URL64.str(0)));
				strcpy(FILENAME,Test.convert(FILENAME64.str(0)));
				download(URL,FILENAME);
			}
		}
	}
	INDEXFILE.close();

/*--------------------------------------- download file and store into hFile---------------------------------------*/
	

/*--------------------------------------- Close handles and clear buffers ---------------------------------------*/
	END_PROGRAM:
	if(hFile) { CloseHandle(hFile); }
	if(hINETURL) { InternetCloseHandle(hINETURL); }
	if(hINETOPEN) { InternetCloseHandle(hINETOPEN);}
	DeleteFile(".\\index.txt");
	return 0;
}
/*--------------------------------------- End of program ---------------------------------------*/
