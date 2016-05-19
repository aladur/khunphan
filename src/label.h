/*******************************************************************************
**
** label.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef LABEL_H
#define LABEL_H

#include "stdafx.h"

#include <set>
#include <string>
#include "bmanualtimer.h"
#include "sprinter.h"


class Label;
class KPConfig;
class KPUIBase;
enum class Signal;

typedef std::set<Label *> tActivated;


class Label
{
public:
    Label(const std::string &textOrFormat = std::string(""));
    Label(const Label &src);
    ~Label();

    Label &operator=(const Label &src);
    static void PreInitialize(const std::string &, unsigned int, bool,
                              const KPConfig &);
    static void FadeOutAll();
    static void SetActive(Label *pLabel);
    void Draw();
    void SetPosition(float, float, float, AlignItem pos = AlignItem::Left);
    void SetTextOrFormat(const std::string &textOrFormat);
    template <typename... Args>
    void FormatText(Args&& ...args)
    {
        if (!format.empty())
        {
            oldLabelText = labelText;
            labelText = sprinter::print(format, args...);

            RecreateDisplayList();
        }
    }
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
    void Animate(unsigned int);
    void SetSignal(Signal);
    Signal MouseEvent(MouseButton, MouseButtonEvent, int, int, KPUIBase &);
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
    AlignItem Alignment;
    Signal signal;
    int DisplayList;
    bool  hasInputFocus;
    int lineCount;
    unsigned int MaxCharacters;
    float maxWidth;
    BManualTimer animationTimer;
};

#endif
