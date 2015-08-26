/*
    bthreadfactory.cpp


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#include "misc1.h"
#include "bthreadfactory.h"
#ifdef LINUX
#include "blinuxthreadimp.h"
#endif
#ifdef WIN32
#include "bwin32threadimp.h"
#endif

BThreadFactory *BThreadFactory::instance = NULL;

BThreadFactory &BThreadFactory::Instance()
{
   if (instance == NULL)
   {
      instance = new BThreadFactory;
      atexit ( BThreadFactory::finalize );
   }
   return *instance;
};

BThreadImp *BThreadFactory::CreateBThreadImp()
{
#ifdef WIN32
  return new BWin32ThreadImp;
#else
  #ifdef LINUX
  return new BLinuxThreadImp();
  #else
    return NULL; // Sw: Which other platforms to support ???
  #endif
#endif
};
