#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

//Excuse my lack of error checks, it wasn't part of the challenge :)
int _tmain(int argc, wchar_t* argv[])
{
	TCHAR buff[100];
	TCHAR flag[10];
	int pathIndex = 0;

	//Depending on how many arguments are passed, we will decide what to do
	//pathIndex dictates what index of argv holds the path. If a flag isn't
	//detected as an argument, we set the flag variable to /x.
	if (argc == 3)
	{
		pathIndex = 2;
		StringCchCopy(flag, MAX_PATH, argv[1]);
	}
	else if (argc == 2)
	{
		pathIndex = 1;
		StringCchCopy(flag, MAX_PATH, TEXT("/x"));
	}
	else if (argc == 1)
	{
		_tprintf(TEXT("\nNo path!\n"));
		exit(1);
	}

	//We pass the path to ExpandEnvironmentStrings and if an environmental
	//variable is detected, the variable will be replaced with the full
	//path it represents. The full path will be placed in the buff variable.
	ExpandEnvironmentStrings(argv[pathIndex], buff, 100);

	//The user's input, the full resolved path 
	//and the flag are printed to the terminal.
	_tprintf(TEXT("\nUser Input: %s\n"), argv[pathIndex]);
	_tprintf(TEXT("Resolves to: %s\n"), buff);

	//Remember, if the flag is /x that means the 
	//user didn't provide a flag as an argument.
	if (!_tcscmp(flag, TEXT("/x"))) 
		_tprintf(TEXT("Flag: No flag provided\n\n"));
	else 
		_tprintf(TEXT("Flag: %s\n\n"), flag);

	//Pass the current directory we want to list the contents of and flag 
	//to listDir. The level parameter will provide the level of indentation.
	listDir(buff, flag, -1);

	//New line to make output look clean. ;)
	_tprintf(TEXT("\n"));

	return 0;
}

int listDir(STRSAFE_LPCWSTR path, STRSAFE_LPCWSTR flag, int level)
{
	TCHAR pathArr[MAX_PATH];
	TCHAR buff[MAX_PATH];
	WIN32_FIND_DATA findData;
	HANDLE findHandle = INVALID_HANDLE_VALUE;
	level++; //level variable is increased in every level of recursion

	//Path is copied into pathArr then '\*' is concatenated to the end.
	//This indicate we want to list everything within the current directory.
	//Ex: C:\Windows\System32\*
	StringCchCopy(pathArr, MAX_PATH, path);
	StringCchCat(pathArr, MAX_PATH, TEXT("\\*"));

	//First file is found within selected path
	findHandle = FindFirstFile(pathArr, &findData);

	//Do-while loop iterates through the dirs/files in path
	do{
		//We check if the next object is a file or a directory. If its a directory we print 
		//out the name and recursively list its contents. If it's a file and the /f flag is  
		//active we print out the filename. If it's a file and /f flag is down we do nothing.
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//All directories contain '.' (current directory) and '..' (last directory).
			//We use this if statement to ignore these redundant directories.
			if (_tcscmp(findData.cFileName, TEXT(".")) && _tcscmp(findData.cFileName, TEXT("..")))
			{
				//level variable sets level of indentation
				for (int i = 0; i < level; i++) 
					_tprintf(TEXT("\t"));

				//We print directory name
				_tprintf(TEXT("%s - [Dir]\n"), findData.cFileName);

				//We copy the current path to buff and append to it the name
				//of the directory we found. This is so we can recursively 
				//enumrate the contents of this newly found directory.
				StringCchCopy(buff, MAX_PATH, path);
				StringCchCat(buff, MAX_PATH, TEXT("\\"));
				StringCchCat(buff, MAX_PATH, findData.cFileName);

				//Call function recursively with updated 
				//path and level with the same flag.
				listDir(buff, flag, level);
			}
		}
		else if(!_tcscmp(flag, TEXT("/f")))
		{
			//If /f flag is up then we print out filenames
			for (int i = 0; i < level; i++) _tprintf(TEXT("\t"));
				_tprintf(TEXT("%s \n"), findData.cFileName);
		}

	} while (FindNextFile(findHandle, &findData) != 0);

	//Close handle after we're done
	FindClose(findHandle);

	return 0;
}
