/*
    bdir.cpp


    Basic class used for directory functions
    Copyright (C) 1999-2024  W. Schwotzer

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

#include "stdafx.h"
#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif
#ifdef HAVE_DIRENT_H
    #include <dirent.h>
#else
    #define dirent direct
#endif

#ifdef _MSC_VER
    #include <direct.h>
#endif

#include "bdir.h"


bool BDirectory::Exists(const std::string &aPath)
{
    struct stat sbuf;

    return !stat(aPath.c_str(), &sbuf) && (S_ISDIR(sbuf.st_mode));
}

bool BDirectory::Remove(const std::string &aPath)
{
#ifdef _MSC_VER
    return _rmdir(aPath.c_str()) >= 0;
#else
#ifdef __GNUC__
    return rmdir(aPath.c_str()) >= 0;
#else
#error "Unsupported platform"
#endif
#endif
}

bool BDirectory::Create(const std::string &aPath, int mode /* = 0x0755 */)
{
#ifdef _MSC_VER
    return _mkdir(aPath.c_str()) >= 0;
#else
#ifdef __GNUC__
    return mkdir(aPath.c_str(), mode) >= 0;
#else
#error "Unsupported platform"
#endif
#endif
}

#ifdef _WIN32
bool BDirectory::RemoveRecursive(const std::string &aPath)
{
    auto basePath = aPath;
    WIN32_FIND_DATA pentry;

    if (basePath[basePath.length() - 1] != PATHSEPARATOR)
    {
        basePath += PATHSEPARATOR;
    }

    std::string pattern = basePath + "*.*";

    auto handle = FindFirstFile(pattern.c_str(), &pentry);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            auto dirEntry = basePath + pentry.cFileName;

            if (pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (pentry.cFileName[0] != '.')
                {
                    RemoveRecursive(dirEntry);
                }
            }
            else
            {
                remove(dirEntry.c_str());
            }
        }
        while (FindNextFile(handle, &pentry) != 0);

        FindClose(handle);
    }

    BDirectory::Remove(basePath);
    return true;
}
#else
#ifdef __linux__
bool BDirectory::RemoveRecursive(const std::string &aPath)
{
    auto basePath = aPath;
    struct stat sbuf;

    if (basePath[basePath.length() - 1] == PATHSEPARATOR)
    {
        basePath = basePath.substr(0, basePath.length() - 1);
    }

    auto pdir = opendir(basePath.c_str());

    if (pdir != nullptr)
    {
        struct dirent *pentry;

        while ((pentry = readdir(pdir)) != nullptr)
        {
            auto dirEntry = basePath + PATHSEPARATORSTRING + pentry->d_name;

            if (!stat(dirEntry.c_str(), &sbuf) &&
                (S_ISREG(sbuf.st_mode)))
            {
                remove(dirEntry.c_str());
            }
            else if (S_ISDIR(sbuf.st_mode) && pentry->d_name[0] != '.')
            {
                RemoveRecursive(dirEntry);
            }
        } // while

        closedir(pdir);
    }

    BDirectory::Remove(basePath);
    return true;
}
#else
#error "Unsupported platform"
#endif
#endif

#ifdef _WIN32
tPathList BDirectory::GetSubDirectories(const std::string &aPath)
{
    std::vector<std::string> subDirList;
    auto basePath = aPath;
    WIN32_FIND_DATA pentry;

    if (basePath[basePath.length() - 1] != PATHSEPARATOR)
    {
        basePath += PATHSEPARATOR;
    }

    auto pattern = basePath + "*.*";

    auto handle = FindFirstFile(pattern.c_str(), &pentry);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
                pentry.cFileName[0] != '.')
            {
                subDirList.push_back(pentry.cFileName);
            }
        }
        while (FindNextFile(handle, &pentry) != 0);

        FindClose(handle);
    }

    return subDirList;
}
#else
#ifdef __linux__
tPathList BDirectory::GetSubDirectories(const std::string &aPath)
{
    std::vector<std::string> subDirList;
    auto basePath = aPath;

    if (basePath[basePath.length() - 1] == PATHSEPARATOR)
    {
        basePath = basePath.substr(0, basePath.length() - 1);
    }

    auto pdir = opendir(basePath.c_str());

    if (pdir != nullptr)
    {
        struct dirent *pentry;

        while ((pentry = readdir(pdir)) != nullptr)
        {
            struct stat sbuf;
            auto dirEntry = basePath + PATHSEPARATORSTRING + pentry->d_name;

            if (stat(dirEntry.c_str(), &sbuf) == 0 &&
                S_ISDIR(sbuf.st_mode) &&
                pentry->d_name[0] != '.')
            {
                subDirList.push_back(pentry->d_name);
            }
        }

        closedir(pdir);
    }

    return subDirList;
}
#else
#error "Unsupported platform"
#endif
#endif

tPathList BDirectory::GetFiles(const std::string &aPath)
{
    std::vector<std::string> fileList;
    auto basePath = aPath;
#ifdef _WIN32
    WIN32_FIND_DATA pentry;

    if (basePath[basePath.length() - 1] != PATHSEPARATOR)
    {
        basePath += PATHSEPARATOR;
    }

    auto pattern = basePath + "*.*";

    auto handle = FindFirstFile(pattern.c_str(), &pentry);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((pentry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
                (pentry.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == 0 &&
                (pentry.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == 0)
            {
                fileList.push_back(pentry.cFileName);
            }
        }
        while (FindNextFile(handle, &pentry) != 0);

        FindClose(handle);
    }

#else
#ifdef __linux__

    if (basePath[basePath.length() - 1] == PATHSEPARATOR)
    {
        basePath = basePath.substr(0, basePath.length() - 1);
    }

    auto pdir = opendir(basePath.c_str());

    if (pdir != nullptr)
    {
        struct dirent *pentry;

        while ((pentry = readdir(pdir)) != nullptr)
        {
            struct stat sbuf;
            auto dirEntry = basePath + PATHSEPARATORSTRING + pentry->d_name;

            if (stat(dirEntry.c_str(), &sbuf) == 0 &&
                S_ISREG(sbuf.st_mode))
            {
                fileList.push_back(pentry->d_name);
            }
        }

        closedir(pdir);
    }

#else
#error "Unsupported platform"
#endif
#endif
    return fileList;
}
