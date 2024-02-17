/*
    kpuserinterfacefactory.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2016-2024  W. Schwotzer

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

#ifndef KPUSERINTERFACEFACTORY_H
#define KPUSERINTERFACEFACTORY_H

#include "stdafx.h"
#include <stdexcept>
#include "kpconfig.h"
#include "kpsdl12userinterface.h"
#include "kpsdl2userinterface.h"
#include "kpglutuserinterface.h"


class KPUIBase;
class KPnode;

class KPuserInterfaceFactory
{
public:
    static std::unique_ptr<KPUIBase> Create(int type, KPnode &rootNode,
                       KPConfigPtr config)
    {
        std::unique_ptr<KPUIBase> uiBasePtr;

        switch (type)
        {
#ifdef HAVE_SDL2

            case 0:
                uiBasePtr.reset(new KPSdl2UserInterface(rootNode, config));
                break;
#else
#ifdef HAVE_SDL

            case 0:
                uiBasePtr.reset(new KPSdl12UserInterface(rootNode, config));
                break;
#endif
#endif

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)

            case 1:
                uiBasePtr.reset(new KPGlutUserInterface(rootNode, config));
                break;
#endif

            default:
                throw std::runtime_error("No user interface initialized");
        }

        return uiBasePtr;
    }
};

#endif

