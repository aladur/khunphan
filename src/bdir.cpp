/*
    bdir.cpp


    Basic class used for directory functions
    Copyright (C) 1999-2015  W. Schwotzer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "misc1.h"
#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
  #include <sys/types.h>
  #include <unistd.h>
#endif
#ifdef HAVE_DIRENT_H
  #include <sys/types.h>
  #include <dirent.h>
  #define NAMLEN(dirent) strlen((dirent)->d_name)
#else
  #define dirent direct
  #define NAMLEN(dirent) (dirent)->d_namlen
#endif

#ifdef WIN32
# ifdef _MSC_VER
#  include <direct.h>
# endif
#endif

#include "bdir.h"

/********************************************
 ctor / dtor
********************************************/

BDirectory::BDirectory()
{
}

BDirectory::~BDirectory()
{
}

/********************************************
 static functions
********************************************/

bool BDirectory::Exists(const BString &aPath)
{
	struct stat sbuf;

	return !stat(aPath.c_str(), &sbuf) && (S_ISDIR(sbuf.st_mode));
}

bool BDirectory::Remove(const BString &aPath)
{
#if defined(_MSC_VER) || defined(__MINGW32)
	return _rmdir(aPath.c_str()) >= 0;
#endif
#if defined(UNIX) || defined(__CYGWIN32)
	return rmdir(aPath.c_str()) >= 0;
#endif
}

bool BDirectory::Create(const BString &aPath, int mode /* = 0x0755 */)
{
#if defined(_MSC_VER) || defined(__MINGW32)
	return _mkdir(aPath.c_str()) >= 0;
#endif
#if defined(UNIX) || defined(__CYGWIN32)
	return mkdir(aPath.c_str(), mode) >= 0;
#endif
}

#ifdef WIN32
bool BDirectory::RemoveRecursive(const BString &aPath)
{
	BString         basePath;
	BString         dirEntry;
	HANDLE          hdl;
	WIN32_FIND_DATA pentry;

	basePath = aPath;
	if (basePath.lastchar() != PATHSEPARATOR) {
		basePath += PATHSEPARATOR;
	}
	if ((hdl = FindFirstFile(basePath + "*.*", &pentry)) != INVALID_HANDLE_VALUE) {
		do {
			dirEntry = basePath + pentry.cFileName;
			if (pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (pentry.cFileName[0] != '.')
					RemoveRecursive(dirEntry);
			} else
				remove(dirEntry);
		} while (FindNextFile(hdl, &pentry) != 0);
		FindClose(hdl);
	}
	BDirectory::Remove(basePath);
	return true;
}
#else
bool BDirectory::RemoveRecursive(const BString &aPath)
{
	BString         basePath;
	BString         dirEntry;
	DIR             *pd;
	struct stat     sbuf;

	basePath = aPath;
	if (basePath.lastchar() == PATHSEPARATOR)
		basePath.at(0, basePath.length() - 1, basePath);
	if ((pd = opendir(basePath.c_str())) != NULL) {
		struct dirent   *pentry;

		while ((pentry = readdir(pd)) != NULL) {
			dirEntry = basePath + PATHSEPARATORSTRING +
				pentry->d_name;
			if (!stat(dirEntry.c_str(), &sbuf) &&
                            (S_ISREG(sbuf.st_mode))) {
				remove(dirEntry.c_str());
			} else
			if (S_ISDIR(sbuf.st_mode) && pentry->d_name[0] != '.') {
				RemoveRecursive(dirEntry);
			}
		} // while
		closedir(pd);
	}
	BDirectory::Remove(basePath);
	return true;
}
#endif

#ifdef WIN32
tPathList BDirectory::GetSubDirectories(const BString &aPath)
{
	std::vector<BString> subDirList;

	BString         basePath;
	HANDLE          hdl;
	WIN32_FIND_DATA pentry;

	basePath = aPath;
	if (basePath.lastchar() != PATHSEPARATOR) {
		basePath += PATHSEPARATOR;
	}
	if ((hdl = FindFirstFile(basePath + "*.*", &pentry)) != INVALID_HANDLE_VALUE) {
		do {
			if (pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
                pentry.cFileName[0] != '.')
        subDirList.push_back(pentry.cFileName);
    } while (FindNextFile(hdl, &pentry) != 0);
		FindClose(hdl);
	}
  return subDirList;
}
#else
tPathList BDirectory::GetSubDirectories(const BString &aPath)
{
  std::vector<BString> subDirList;

	BString         basePath;
  BString         dirEntry;
  DIR             *pd;
	struct stat     sbuf;

	basePath = aPath;
	if (basePath.lastchar() == PATHSEPARATOR)
		basePath.at(0, basePath.length() - 1, basePath);
	if ((pd = opendir(basePath.c_str())) != NULL) {
		struct dirent   *pentry;

		while ((pentry = readdir(pd)) != NULL) {
      dirEntry = basePath + PATHSEPARATORSTRING + pentry->d_name;
			if (stat(dirEntry.c_str(), &sbuf) == 0 &&
          S_ISDIR(sbuf.st_mode)      &&
          pentry->d_name[0] != '.')
        subDirList.push_back(pentry->d_name);
		} // while
		closedir(pd);
	}
  return subDirList;
}
#endif

tPathList BDirectory::GetFiles(const BString &aPath)
{
	std::vector<BString> fileList;

	BString         basePath;
#ifdef WIN32
	HANDLE          hdl;
	WIN32_FIND_DATA pentry;

	basePath = aPath;
	if (basePath.lastchar() != PATHSEPARATOR) {
		basePath += PATHSEPARATOR;
	}
	if ((hdl = FindFirstFile(basePath + "*.*", &pentry)) != INVALID_HANDLE_VALUE) {
		do {
			if ((pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
				(pentry.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE  ) == 0 && 
				(pentry.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM   ) == 0)
        fileList.push_back(pentry.cFileName);
    } while (FindNextFile(hdl, &pentry) != 0);
		FindClose(hdl);
	}
#else
  BString         dirEntry;
  DIR             *pd;
	struct stat     sbuf;

	basePath = aPath;
	if (basePath.lastchar() == PATHSEPARATOR)
		basePath.at(0, basePath.length() - 1, basePath);
	if ((pd = opendir(basePath.c_str())) != NULL) {
		struct dirent   *pentry;

		while ((pentry = readdir(pd)) != NULL) {
      dirEntry = basePath + PATHSEPARATORSTRING + pentry->d_name;
			if (stat(dirEntry.c_str(), &sbuf) == 0 &&
                            S_ISREG(sbuf.st_mode))
        fileList.push_back(pentry->d_name);
		} // while
		closedir(pd);
	}
#endif
  return fileList;
}  
/********************************************
 member functions
********************************************/

bool BDirectory::Exists(void) const
{
	return Exists(m_path);
}

bool BDirectory::Remove(void) const
{
	return Remove(m_path);
}

bool BDirectory::RemoveRecursive(void) const
{
	return RemoveRecursive(m_path);
}

bool BDirectory::Create(int mode /* = 0x0755 */) const
{
	return Create(m_path, mode);
}

tPathList BDirectory::GetSubDirectories() const
{
  return GetSubDirectories(m_path);
}

tPathList BDirectory::GetFiles() const
{
  return GetFiles(m_path);
}
