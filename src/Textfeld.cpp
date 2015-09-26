/*******************************************************************************
**
** Textfeld.cpp Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**              Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include "misc1.h"
#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>  // needed for access
#endif
#include <string>
#include "btexture.h"
#include "Textfeld.h"
#include "kpuibase.h"


int Textfeld::links[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  9, 11,  9,  9,  8,  8,  7,  8,  7,  7,  7,  6,  8,  9,  7,
    9,  7,  8,  7,  7,  7,  8,  7,  7,  7,  9,  6,  8,  7,  7,  7,
    9,  7,  9,  8,  9,  9,  9,  8,  8,  8,  7,  8,  8,  7,  8,  7,
    9,  8,  9,  7,  6,  8,  6,  7,  7,  6,  6, 10,  7,  7,  9,  5,
    5,  8,  9,  8,  8,  8,  6,  7,  8,  8,  8,  8,  8,  7,  8,  7,
    9,  8,  9,  8,  7,  8,  7,  7,  7,  7,  7,  7,  9,  6,  8,  0,

    0,  0,  0,  8,  7, 10,  8,  8,  3,  7,  0,  9,  7,  0,  0,  0,
    0,  7,  8,  8,  7, 10,  7,  8,  3,  9,  0, 10,  7,  0,  0,  0,
    0,  9, 11,  8, 10,  8, 10,  9,  4,  8,  7,  9,  7,  0,  7,  0,
    0,  8,  8,  8,  7,  8,  8,  9,  7,  7,  7,  9,  8,  8,  8,  7,
    8,  7,  8,  7,  7,  7,  7,  8,  8,  8,  8,  8,  4,  7,  3,  4,
    6,  9,  8,  8,  8,  8,  8,  0,  7,  8,  8,  8,  8,  6,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  4,  6,  3,  3,
    9,  9,  8,  8,  8,  8,  8,  0,  7,  8,  8,  8,  8,  6,  8,  6
};

int Textfeld::rechts[]=
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    10, 15, 22, 28, 29, 33, 33, 15, 15, 14, 20, 29, 14, 21, 14, 23,
    31, 17, 26, 26, 30, 26, 27, 25, 27, 27, 15, 14, 28, 28, 28, 25,
    37, 32, 29, 30, 32, 24, 24, 34, 32, 14, 21, 30, 23, 39, 32, 34,
    29, 36, 29, 27, 25, 32, 30, 43, 30, 28, 27, 19, 23, 16, 27, 26,
    15, 28, 29, 24, 28, 27, 20, 27, 27, 13, 13, 26, 13, 38, 26, 27,
    28, 28, 20, 23, 19, 27, 25, 37, 26, 26, 24, 19, 12, 19, 28,  0,

    0,  0,  0, 28, 25, 43, 28, 28, 18, 47,  0, 17, 43,  0,  0,  0,
    0, 16, 16, 25, 25, 23, 26, 22, 18, 44,  0, 17, 42,  0,  0,  0,
    0, 15, 28, 31, 29, 30, 13, 28, 17, 37, 20, 27, 29,  0, 36,  0,
    0, 30, 20, 20, 17, 27, 29, 15, 15, 13, 20, 27, 42, 41, 42, 24,
    32, 32, 32, 31, 31, 31, 40, 30, 24, 24, 24, 24, 15, 17, 18, 17,
    32, 33, 35, 35, 34, 35, 35,  0, 35, 32, 32, 32, 32, 27, 28, 27,
    28, 28, 28, 28, 28, 28, 43, 24, 27, 27, 27, 27, 14, 17, 18, 17,
    28, 27, 28, 28, 28, 28, 28,  0, 28, 26, 26, 26, 26, 25, 28, 25
};

unsigned int Textfeld::texture       = 0;
short        Textfeld::textureSource = 0;

tActivated Textfeld::activated;

Textfeld::Textfeld() : pString(NULL), size(0),
    x(0), y(0),
    Hoehe(0), Aspekt(0), Alpha(0),
    alt_x(0),  alt_y(0),  alt_Hoehe(0),  alt_Alpha(0),
    soll_x(0), soll_y(0), soll_Hoehe(0), soll_Alpha(0),
    Ausrichtung(0), InAnimation(false), Signal(0), Zeit(0),
    TextfeldIndex(0), hasInputFocus(false), lineCount(0),
    MaxCharacters(32), maxWidth(0)
{
}

Textfeld::~Textfeld()
{
    delete [] pString;
}

void Textfeld::DeactivateAll()
{
    tActivated::iterator it;

    for (it = activated.begin(); it != activated.end(); ++it)
    {
        (*it)->Desaktiviere();
    }

    Textfeld::activated.clear();
}

void Textfeld::SetActive(Textfeld *pTextfeld)
{
    if (activated.find(pTextfeld) == activated.end())
    {
        activated.insert(pTextfeld);
    }
}

void Textfeld::PreInitialize(const char *TextureName, unsigned int TextureSize,
                             bool Nearest, const KPConfig *pConfig,
                             bool always /*= true*/)
{
    BTexture *pTexture = new BTexture;
    std::string file1, file2;
    const char *texels;

    file1 = pConfig->GetDirectory(KP_TEXTURE_DIR) + TextureName +
            PATHSEPARATORSTRING + "characters.png";
    file2 = pConfig->GetDirectory(KP_TEXTURE_DIR) + "characters.png";

    if (!always)
    {
        if ((!access(file1.c_str(), R_OK) && textureSource == 1) ||
            (!access(file2.c_str(), R_OK) && textureSource == 2))
        {
            delete pTexture;
            return; // texture from file1 or file2 already loaded
        }
    }

    if((texels = pTexture->ReadTextureFromFile(file1.c_str(),
                 TEX_WITH_ALPHA)) == NULL)
    {
        if ((texels = pTexture->ReadTextureFromFile(file2.c_str(),
                      TEX_WITH_ALPHA)) == NULL)
        {
            message(mtError, "*** Error reading texture from file '%s'\n",
                    file2.c_str());
            delete pTexture;
            exit(1);
        }
        else
        {
            textureSource = 2;
        }
    }
    else
    {
        textureSource = 1;
    }

    if (BTexture::GetExpToBase2(TextureSize) == -1)
    {
        message(mtError, " *** Error scaling texture %s. Program aborted\n",
                file2.c_str());
        delete pTexture;
        exit(1);
    }

    texels = pTexture->Rescale(BTexture::GetExpToBase2(TextureSize),
                               TEX_SMALLER | TEX_RESCALE_AVG );

    unsigned int width  = pTexture->GetWidth();
    unsigned int height = pTexture->GetHeight();

    if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
    {
        message(mtWarning,
                "*** Warning: width or height of '%s' is not a power of 2\n",
                file2.c_str());
    }

    if (texture == 0)
    {
        glGenTextures(1, &texture);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texels);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete pTexture;
}

void Textfeld::Initialisiere()
{
    x=y=Hoehe=Aspekt=Alpha=0.0;
    alt_x=alt_y=alt_Hoehe=alt_Alpha=0.0;
    soll_x=soll_y=soll_Hoehe=soll_Alpha=0.0;
    Ausrichtung=A_LINKS;
    InAnimation=false;
    Signal=0;
    Zeit=0;
    hasInputFocus=false;
}

void Textfeld::Initialisiere(const char TextZ[])
{
    Initialisiere();
    SetzeText(TextZ);
}

void Textfeld::InitialisiereKDL(const char TextZ[])
{
    Initialisiere();
    SetzeTextKDL(TextZ);
}

void Textfeld::male()
{
    if (Alpha)
    {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glTranslatef(x, y, 0);
        glScalef(Hoehe,Hoehe,1);
        glColor4f(1.0,1.0,1.0,Alpha);
        glCallList(TextfeldIndex);
        glPopMatrix();
    }
}

void Textfeld::Positioniere(float X, float Y, float H, tKPAlignment A)
{

    if (A != A_EGAL)
    {
        Ausrichtung=A;
    }

    switch (Ausrichtung)
    {
        case A_LINKS:
        {
            soll_x = X;
        }
        break;
        case A_MITTE:
        {
            soll_x = X-0.5f*Aspekt*H;
        }
        break;
        case A_RECHTS:
        {
            soll_x = X-Aspekt*H;
        }
        break;
    }

    soll_y     = Y;
    soll_Hoehe = H;
    soll_Alpha = MOD_EINGEBLENDET;

    if (Alpha == MOD_AUSGEBLENDET)
    {

        x=((soll_x-8)/1.5f)+8;
        y=((soll_y-6)/1.5f)+6;
        Hoehe=soll_Hoehe/1.5f;

        /*
          ax=((24.0*rand())/RAND_MAX)-4;
          ay=((14.0*rand())/RAND_MAX)-1;
          bx=ax+2*(bx_-ax_);
          by=ay+2*(by_-ay_);
        */
    }

    Signal=0;

    StarteAnimation();
}

void Textfeld::SetzeFormat(const char *srcString /* = NULL */)
{
    if (srcString == NULL)
    {
        format.clear();
    }
    else
    {
        format = srcString;
    }
}

void Textfeld::CheckValidString(int min_size, const char *srcString /*= NULL */)
{
    char *sTmp = NULL;

    if (pString == NULL || size < min_size)
    {
        int m = min_size;
        if (m < 50)
        {
            m = 50;    // define a min size of 50 (avoiding mem. allocation)
        }
        sTmp = pString;
        pString = new char [m];
        size = m;
        pString[0] = '\0';
    }

    if (srcString != NULL)
    {
        strcpy(pString, srcString);
    }

    delete [] sTmp;
}

void Textfeld::SetzeText(const char TextZ[])
{
    SetzeTextKDL(TextZ);
    SetzeFormat(pString);
    GeneriereDisplayList();
}

void Textfeld::SetzeTextKDL(const char TextZ[])
{
    CheckValidString(strlen(TextZ) + 1, TextZ);
    SetzeFormat(pString);
}

// Sw: Create a string based on a given format
// if format == NULL use pString as
// formatString
int Textfeld::FormatText(const char *format, ...)
{
    int result = 0;

    va_list arg_ptr;

    va_start(arg_ptr, format);
    result = this->vsprintf(format,arg_ptr);
    va_end(arg_ptr);

    GeneriereDisplayList();

    return result;
}

// Sw: Same as FormatText but without creating
// a Display List
int Textfeld::FormatTextKDL(const char *format, ...)
{
    int result;

    va_list arg_ptr;

    va_start(arg_ptr, format);
    result = this->vsprintf(format,arg_ptr);
    va_end(arg_ptr);

    return result;
}

// Sw: In vsprintf we still have a size limit for the string
#define MAX_SSIZE   (2000)
int Textfeld::vsprintf(const char *_format, va_list arg_ptr)
{
    std::string fmt;

    if (pString == NULL)
    {
        return -1;
    }

    if (_format == NULL)
    {
        if (format.empty())
        {
            format = pString;
        }
        fmt = format;
    }
    else
    {
        fmt = _format;
    }

    CheckValidString(MAX_SSIZE);

#ifdef WIN32
#ifdef _MSC_VER
    _vsnprintf_s(pString, MAX_SSIZE, MAX_SSIZE-1, fmt.c_str(), arg_ptr);
#else
    vsprintf(pString, fmt.c_str(), arg_ptr);
#endif
#else
    vsnprintf(pString, MAX_SSIZE-1, fmt.c_str(), arg_ptr);
#endif

    return strlen(pString);
}

bool Textfeld::Zeichen(char Taste)
{
    if (hasInputFocus)
    {
        CheckValidString(strlen(pString) + 2, pString);
        if (Taste>=32 && Taste<127)  // Add character
        {
            size_t size;

            if (strlen(pString) >= MaxCharacters)
            {
                return true;
            }
            size = strlen(pString);
            pString[size] = Taste;
            pString[size+1] = '\0';
            GeneriereDisplayList();
            return true;
        }
        else if (Taste==8 || Taste==127)     // Delete last character
        {
            for (GLint i=0; i<size; i++)
                if (!pString[i+1])
                {
                    pString[i]='\0';
                }
            GeneriereDisplayList();
            return true;
        }
        else if (Taste==13 || Taste==10) // Commit input with Enter or Line Feed
        {
            soll_Alpha = MOD_EINGEBLENDET;
            hasInputFocus = false;
            if (soll_Alpha!=Alpha)
            {
                StarteAnimation();
            }
            return true;
        }
    }
    return false;
}

const char *Textfeld::Text()
{
    return pString;
}

bool Textfeld::IsDeactivated() const
{
    return ((soll_Alpha != Alpha && (soll_Alpha == MOD_AUSGEBLENDET)) ||
            (Alpha == MOD_AUSGEBLENDET));
}

void Textfeld::Desaktiviere()
{
    soll_Alpha = MOD_AUSGEBLENDET;

    soll_x=((x-8)*1.5f)+8;
    soll_y=((y-6)*1.5f)+6;
    soll_Hoehe=Hoehe*1.5f;

    hasInputFocus = false;

    if (soll_Alpha!=Alpha)
    {
        StarteAnimation();
    }
}

void Textfeld::SetInputFocus(bool state)
{
    hasInputFocus = state;

    if (hasInputFocus)
    {
        VollSichtbar();
    }
    else
    {
        if (Alpha && soll_Alpha)
        {
            Eingeblendet();
        }
    }
}

void Textfeld::Angewaehlt()
{
    Alpha = MOD_ANGEWAEHLT;
    soll_Alpha = MOD_EINGEBLENDET;
    if (soll_Alpha!=Alpha)
    {
        StarteAnimation();
    }
}

void Textfeld::Eingeblendet()
{
    soll_Alpha = MOD_EINGEBLENDET;
    if (soll_Alpha!=Alpha)
    {
        StarteAnimation();
    }
}

void Textfeld::VollSichtbar()
{
    soll_Alpha =MOD_VOLLSICHTBAR;
    if (soll_Alpha!=Alpha)
    {
        StarteAnimation();
    }
}

int Textfeld::Animiere(int Faktor)
{
    if (!InAnimation)
    {
        return 0;
    }
    Zeit+=Faktor;
    if (Zeit>=TOTAL_ANIMATIONTIME)
    {
        x=soll_x;
        y=soll_y;
        Hoehe=soll_Hoehe;
        Alpha=soll_Alpha;
        InAnimation=false;
        return 1;
    }
    else
    {
        GLfloat Faktor = 0.5f - 0.5f * cos(M_PIf * Zeit / TOTAL_ANIMATIONTIME);

        x=(soll_x-alt_x)*Faktor+alt_x;
        y=(soll_y-alt_y)*Faktor+alt_y;
        Hoehe=(soll_Hoehe-alt_Hoehe)*Faktor+alt_Hoehe;
        Alpha=(soll_Alpha-alt_Alpha)*Faktor+alt_Alpha;
        return 0;
    }
}

void Textfeld::SetzeSignal(int NeuesSignal)
{
    Signal=NeuesSignal;
}

void Textfeld::StarteAnimation()
{
    SetActive(this);
    InAnimation=true;
    Zeit=0;
    alt_x=x;
    alt_y=y;
    alt_Hoehe=Hoehe;
    alt_Alpha=Alpha;
}

void Textfeld::GeneriereDisplayList()
{
    CheckValidString(50);

    if (!TextfeldIndex)
    {
        TextfeldIndex=glGenLists(1);
    }

    int x,y;
    float w  = 1.0 / 16.0;
    if (!maxWidth)
    {
        Aspekt   = 0.0;
        GLint  p = 0;
        GLuint c = 0;
        lineCount= 1;
        glNewList(TextfeldIndex, GL_COMPILE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();
        p=0;
        while ((c = static_cast<unsigned char>(pString[p++])) != '\0')
        {
            Aspekt+=(rechts[c]-links[c]+4)/64.0f; // Sw: added
            glTranslatef(-links[c]/64.0f,0,0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBegin(GL_QUADS);
            x = c % 16;
            y = 15 - (c / 16);
            glTexCoord2f(w*x,     w*y);
            glVertex2f(0.0,0.0);
            glTexCoord2f(w*(x+1), w*y);
            glVertex2f(1.0,0.0);
            glTexCoord2f(w*(x+1), w*(y+1));
            glVertex2f(1.0,1.0);
            glTexCoord2f(w*x,     w*(y+1));
            glVertex2f(0.0,1.0);
            glEnd();
            glTranslatef((rechts[c]+4)/64.0f,0,0);
        }
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
        glEndList();
    }


    if (maxWidth)
    {
        Aspekt=0.0;

        GLint  start     = 0;
        GLuint character = 0;
        GLint  Pos       = 0;
        GLint  spaceCount= 0;
        GLint  lastSpace = 0;
        lineCount        = 0;
        GLfloat lineWidth= 0.0;
        GLfloat lineWidthUntilLastSpace = 0.0;
        glNewList(TextfeldIndex,GL_COMPILE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        while (pString[Pos])
        {

            start      = Pos;
            spaceCount = 0;
            lastSpace  = 0;
            lineWidth  = 0.0;

            while ((character = static_cast<unsigned char>(pString[Pos])) &&
                   (lineWidth < maxWidth * 1.05))
            {
                if (character==32)
                {
                    lineWidthUntilLastSpace = lineWidth;
                    lastSpace = Pos;
                    spaceCount++;
                }
                lineWidth += (rechts[character]-links[character]+4)/64.0f;
                Pos++;
            }

            if (pString[Pos])   // noch nicht am Ende: Blocksatz
            {
                GLfloat delta=(maxWidth - lineWidthUntilLastSpace)/
                              (spaceCount - 1.0f);
                GLuint c;
                glPushMatrix();
                glTranslatef(0, -lineCount * 0.7f, 0);
                Pos=start;
                while ((c = static_cast<unsigned char>(pString[Pos++])) &&
                       Pos <= lastSpace)
                {
                    glTranslatef(-links[c]/64.0f,0,0);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glBegin(GL_QUADS);
                    x = c % 16;
                    y = 15 - (c / 16);
                    glTexCoord2f(w*x,     w*y);
                    glVertex2f(0.0,0.0);
                    glTexCoord2f(w*(x+1), w*y);
                    glVertex2f(1.0,0.0);
                    glTexCoord2f(w*(x+1), w*(y+1));
                    glVertex2f(1.0,1.0);
                    glTexCoord2f(w*x,     w*(y+1));
                    glVertex2f(0.0,1.0);
                    glEnd();
                    glTranslatef((rechts[c]+4)/64.0f,0,0);
                    if (c==32)
                    {
                        glTranslatef(delta,0,0);
                    }
                }
                glPopMatrix();
                lineCount++;
            }
            else     // am Ende: Linksbuendig
            {
                GLfloat delta=0;
                if (lineWidth > maxWidth)
                {
                    delta=(maxWidth- lineWidth)/(spaceCount + 0.0f);
                }
                GLuint c;
                glPushMatrix();
                glTranslatef(0, -lineCount * 0.7f, 0);
                Pos=start;
                while ((c = static_cast<unsigned char>(pString[Pos++])))
                {
                    glTranslatef(-links[c]/64.0f,0,0);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glBegin(GL_QUADS);
                    x = c % 16;
                    y = 15 - (c / 16);
                    glTexCoord2f(w*x,     w*y);
                    glVertex2f(0.0,0.0);
                    glTexCoord2f(w*(x+1), w*y);
                    glVertex2f(1.0,0.0);
                    glTexCoord2f(w*(x+1), w*(y+1));
                    glVertex2f(1.0,1.0);
                    glTexCoord2f(w*x,     w*(y+1));
                    glVertex2f(0.0,1.0);
                    glEnd();
                    glTranslatef((rechts[c]+4)/64.0f,0,0);
                    if (c==32)
                    {
                        glTranslatef(delta,0,0);
                    }
                }
                glPopMatrix();
                lineCount++;
                break;
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glEndList();
    }
}

int Textfeld::Maustaste(tMouseButton button, tMouseEvent event,
                        int x_, int y_, KPUIBase &ui)
{
    GLfloat xf=16.0f*x_/ ui.GetValue(KP_WINDOW_WIDTH);
    GLfloat yf=12.0f-12.0f*y_/ ui.GetValue(KP_WINDOW_HEIGHT);
    if (soll_Alpha>0.0 &&
        Signal!=0 &&
        x<=xf && xf<=x+Hoehe*Aspekt && y<=yf && yf<=y+Hoehe)
    {
        if (button == KP_LEFT_MB)
        {
            if (event == KP_BUTTON_PRESS)
            {
                Angewaehlt();
                return -1;
            }
            else
            {
                return Signal;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

float Textfeld::TextfeldHoehe()
{
    return 0.7f * lineCount;
}


void Textfeld::SetzeMaxBreite(float mb)
{
    if (mb < 0)
    {
        mb = 0;
    }
    maxWidth = mb;
}
