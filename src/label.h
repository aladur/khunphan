/*******************************************************************************
**
** label.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __LABEL_H__
#define __LABEL_H__

#include "misc1.h"

#include <set>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <string>
#include "kpconfig.h"



class Label;
class KPConfig;
class KPUIBase;

typedef std::set<Label *> tActivated;


class Label
{
public:
    Label();
    virtual ~Label();
    static void PreInitialize(const char *, unsigned int, bool,
                              const KPConfig *, bool always = true);
    static void DeactivateAll();
    static void SetActive(Label *pLabel);
    void Initialisiere();
    void Initialisiere(const char[]);
    void InitialisiereKDL(const char[]);
    void male();
    void Positioniere(float,float,float,tKPAlignment pos = A_LINKS);
    void SetzeText(const char[]);
    void SetzeTextKDL(const char[]);
    int  FormatText(const char *format, ...);
    int  FormatTextKDL(const char *format, ...);
    int  vsprintf(const char *format, va_list arg_ptr);
    bool HasInputFocus()
    {
        return hasInputFocus;
    };
    void SetMaxCharacters(unsigned int value)
    {
        MaxCharacters = value;
    };
    unsigned int GetMaxCharacters()
    {
        return MaxCharacters;
    };
    bool Zeichen(char);
    const char *Text();

    bool IsDeactivated() const;
    void Desaktiviere();
    void Angewaehlt();
    void Eingeblendet();
    void VollSichtbar();
    int Animiere(int);
    void SetzeSignal(int);
    int GetSignal(void)
    {
        return Signal;
    };
    int Maustaste(tMouseButton,tMouseEvent,int,int, KPUIBase &);

    void SetzeMaxBreite(float);

    void SetInputFocus(bool state);

    float TextfeldHeight();

    void GeneriereDisplayList();

private:
    static tActivated activated;
    static unsigned int texture;

    void SetzeFormat(const char *);
    void StarteAnimation();
    void CheckValidString(int min_size, const char *srcString = NULL);
    std::string format;
    char *pString;
    int   size;
    float AspectRatio;
    float x, y, Height, Alpha;
    float old_x, old_y, old_Height, old_Alpha;
    float target_x, target_y, target_Height, target_Alpha;
    int Alignment;
    bool  InAnimation;
    int Signal;
    int Time;
    int DisplayList;
    bool  hasInputFocus;
    int lineCount;
    unsigned int MaxCharacters;
    float maxWidth;

    static int   left[];
    static int   right[];
    static short textureSource;
};

#endif
