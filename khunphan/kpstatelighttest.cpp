/*
    kpstatelighttest.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2006  W. Schwotzer

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

#include <stdio.h>

#include "kpstatelighttest.h"
#include "kpmenu.h"
#include "light.h"
#include "Kamera.h"


KPstateLightTest::KPstateLightTest()
{
}

void KPstateLightTest::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
{
  KPstate::Initialize(pContext, pOldState);

  // Do some initialization stuff here:

   KPConfig::Instance().CameraPosition = 1;
   pContext->GetCamera().ladePosition(KPConfig::Instance().CameraPosition);
  UpdateDisplay(pContext);
}

void KPstateLightTest::UpdateDisplay(KPstateContext *pContext)
{
  KPstate::UpdateDisplay(pContext);

  KPmenu &menu = pContext->GetMenu();

  menu.TextfeldArray[T_OK]->Positioniere(8,1,1,A_MITTE);
  menu.TextfeldArray[T_OK]->SetzeSignal(S_BACK);

  StartAnimation();
}

void  KPstateLightTest::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
{
  CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);

  switch (key) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
              KPConfig::Instance().CameraPosition = key - '1' + 1;
              pContext->GetCamera().ladePosition(KPConfig::Instance().CameraPosition);
              break;
  }
}

tKPMenuState KPstateLightTest::ESCKeyAction (KPstateContext *pContext)
{
  return KPState_Shutdown;
}

void  KPstateLightTest::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal)
  {
    case S_BACK:
      pContext->ChangeState(KPState_Shutdown);
        break;
      default:
           HandleMouseClick(pContext, button, state, x, y);
        break;
  }
}

void  KPstateLightTest::HandleMouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  static int mouse_x=0, mouse_y=0;
  float lx, ly, lz;

  if (button == KP_LEFT_MB && state == KP_BUTTON_PRESS)
  {
      mouse_x = x; // save current mouse position
      mouse_y = y;
  }
  else if (button == KP_LEFT_MB && state == KP_BUTTON_RELEASE)
  {
      // calculate mouse direction
      int diff_x = x - mouse_x;
      int diff_z = -(y - mouse_y);
      pContext->GetLight().GetPosition(lx, ly, lz);
      lx += diff_x;
      lz += diff_z;
      pContext->GetLight().SetPosition(lx, ly, lz);
      DEBUGPRINT3("Light position x=%f y=%f z=%f\n", lx, ly, lz);
  } else if (button == KP_RIGHT_MB && state == KP_BUTTON_PRESS)
  {
      mouse_x = x; // save current mouse position
      mouse_y = y;
  }
  else if (button == KP_RIGHT_MB && state == KP_BUTTON_RELEASE)
  {
      // calculate mouse direction
      int diff_y = -(y - mouse_y);
      pContext->GetLight().GetPosition(lx, ly, lz);
      ly += diff_y;
      pContext->GetLight().SetPosition(lx, ly, lz);
      DEBUGPRINT3("Light position x=%f y=%f z=%f\n", lx, ly, lz);
  }
  
}

