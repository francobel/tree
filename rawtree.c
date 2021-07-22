//Trying to replicate the functionality of the "tree" 
//program without using the C Runtime Library
#include <windows.h>

//Function Definitions
int listDir(LPCWSTR path, LPCWSTR flag, int level);
void print(LPCWSTR x);

//Excuse my lack of error checks once again, it wasn't part of the challenge :)
int wmain()
{
	int argc;
	//Without CRT you have to get creative to get ARGC and ARGV
	//from the user. Use this method instead to bypass.
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	TCHAR buff[100];
	TCHAR printer[100];
	TCHAR flag[10];
	int pathIndex = 0;

	//Depending on how many arguments are passed, we will decide what to do
	//pathIndex dictates what index of argv holds the path. If a flag isn't
	//detected as an argument, we set the flag variable to /x.
	if (argc == 3)
	{
		pathIndex = 2;
		lstrcpyW(flag, argv[1]);
	}
	else if (argc == 2)
	{
		pathIndex = 1;
		lstrcpyW(flag, TEXT("/x"));
	}
	else if (argc == 1)
	{
		print(TEXT("\nNo path!\n\n"));
		ExitProcess(1);
	}

	//We pass the path to ExpandEnvironmentStrings and if an environmental
	//variable is detected, the variable will be replaced with the full
	//path it represents. The full path will be placed in the buff variable.
	ExpandEnvironmentStrings(argv[pathIndex], buff, 100);

	//The user's input, the full resolved path 
	//and the flag are printed to the terminal.
	lstrcpyW(printer, TEXT("\nUser Input: "));
	lstrcatW(printer, argv[pathIndex]);
	lstrcatW(printer, TEXT("\n"));
	print(printer);

	lstrcpyW(printer, TEXT("Resolves to: "));
	lstrcatW(printer, buff);
	lstrcatW(printer, TEXT("\n"));
	print(printer);

	//Remember, if the flag is /x that means the 
	//user didn't provide a flag as an argument.
	if (!lstrcmpW(flag, TEXT("/x")))
		print(TEXT("Flag: No flag provided\n\n"));
	else
	{
		lstrcpyW(printer, TEXT("Flag: "));
		lstrcatW(printer, flag);
		lstrcatW(printer, TEXT("\n\n"));
		print(printer);
	}

	//Pass the current directory we want to list the contents of and flag 
	//to listDir. The level parameter will provide the level of indentation.
	listDir(buff, flag, -1);

	//New line to make output look clean. ;)
	print(TEXT("\n"));

	return 0;
}

int listDir(LPCWSTR path, LPCWSTR flag, int level)
{
	TCHAR pathArr[MAX_PATH];
	TCHAR buff[MAX_PATH];
	TCHAR printer[100];
	WIN32_FIND_DATA findData;
	HANDLE findHandle = INVALID_HANDLE_VALUE;
	level++;

	//Path is copied into pathArr then '\*' is concatenated to the end.
	//This indicate we want to list everything within the current directory.
	//Ex: C:\Windows\System32\*
	lstrcpyW(pathArr, path);
	lstrcatW(pathArr, TEXT("\\*"));

	//First file is found within selected path.
	findHandle = FindFirstFile(pathArr, &findData);

	//Do-while loop iterates through the dirs/files in path
	do {
		//We check if the next object is a file or a directory. If its a directory we print 
		//out the name and recursively list its contents. If it's a file and the /f flag is  
		//active we print out the filename. If it's a file and /f flag is down we do nothing.
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//All directories contain '.' (current directory) and '..' (last directory).
			//We use this if statement to ignore these redundant directories.
			if (lstrcmpW(findData.cFileName, TEXT(".")) && lstrcmpW(findData.cFileName, TEXT("..")))
			{
				//level variable sets level of indentation
				for (int i = 0; i < level; i++)
					print(TEXT("\t"));

				//We print directory name
				lstrcpyW(printer, findData.cFileName);
				lstrcatW(printer, TEXT(" - [Dir]\n"));
				print(printer);

				//We copy the current path to buff and append to it the name
				//of the directory we found. This is so we can recursively 
				//enumrate the contents of this newly found directory.
				lstrcpyW(buff, path);
				lstrcatW(buff, TEXT("\\"));
				lstrcatW(buff, findData.cFileName);

				//Call function recursively with updated 
				//path and level with the same flag.
				listDir(buff, flag, level);
			}
		}
		else if (!lstrcmpW(flag, TEXT("/f")))
		{
			//level variable sets level of indentation
			for (int i = 0; i < level; i++) print(TEXT("\t"));

			//If /f flag is up then we print out filenames
			lstrcpyW(printer, findData.cFileName);
			lstrcatW(printer, TEXT("\n"));
			print(printer);
		}

	} while (FindNextFile(findHandle, &findData) != 0);

	//Close handle after we're done
	FindClose(findHandle);

	return 0;
}

//Wrapper around WriteConsole function
//This really makes you appreciate the simple things.
void print(LPCWSTR x)
{
	HANDLE std_out;
	int i;
	int size = lstrlenW(x);

	std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	AttachConsole(ATTACH_PARENT_PROCESS);
	WriteConsoleW(std_out, x, size, &i, NULL);
}
