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
    static void FadeOutAll();
    static void SetActive(Label *pLabel);
    void Initialize();
    void Initialize(const char[]);
    void InitializeNDL(const char[]);
    void Draw();
    void SetPosition(float,float,float,tKPAlignment pos = A_LEFT);
    void SetText(const char[]);
    void SetTextNDL(const char[]);
    int  FormatText(const char *format, ...);
    int  FormatTextNDL(const char *format, ...);
    int  vsprintf(const char *format, va_list arg_ptr);
    inline bool HasInputFocus()
    {
        return hasInputFocus;
    };
    inline void SetMaxCharacters(unsigned int value)
    {
        MaxCharacters = value;
    };
    inline unsigned int GetMaxCharacters()
    {
        return MaxCharacters;
    };
    bool AddCharacter(char);
    std::string GetText();

    void SetFadeOut();
    void SetSelected();
    void SetFadeIn();
    void SetFullyVisible();
    int Animate(int);
    void SetSignal(int);
    inline int GetSignal(void)
    {
        return Signal;
    };
    int MouseEvent(tMouseButton,tMouseEvent,int,int, KPUIBase &);
    void SetMaxWidth(float);
    void SetInputFocus(bool state);
    float GetHeight();
    void RecreateDisplayList();

private:
    static tActivated activated;
    static unsigned int texture;

    void SetFormat(const char *);
    void StartAnimation();
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
