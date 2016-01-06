/*******************************************************************************
**
** label.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
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
    Label(const std::string &textOrFormat = std::string(""));
    Label(const Label &src);
    virtual ~Label();
    Label &operator=(const Label &src);
    static void PreInitialize(const std::string &, unsigned int, bool,
                              const KPConfig &);
    static void FadeOutAll();
    static void SetActive(Label *pLabel);
    void Draw();
    void SetPosition(float,float,float,tKPAlignment pos = A_LEFT);
    void SetTextOrFormat(const std::string &textOrFormat);
    int  FormatText(int count, ...);
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
    void DebugPrint();

private:
    static tActivated activated;
    static unsigned int Texture;
    static std::string TextureSource;
    static const int   left[];
    static const int   right[];

    void StartAnimation();
    std::string format;
    std::string labelText;
    std::string oldLabelText;
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
};

#endif
