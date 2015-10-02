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
    static void PreInitialize(const std::string &, unsigned int, bool,
                              const KPConfig *, bool always = true);
    static void FadeOutAll();
    static void SetActive(Label *pLabel);
    void Initialize();
    void Initialize(const std::string &textOrFormat);
    void Draw();
    void SetPosition(float,float,float,tKPAlignment pos = A_LEFT);
    void SetText(const std::string &text);
    void SetTextNDL(const std::string &text);
    int  FormatText(...);
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
    std::string GetText()
    {
        return labelText;
    }

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

    void StartAnimation();
    std::string format;
    std::string labelText;
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
