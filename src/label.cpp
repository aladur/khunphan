/*******************************************************************************
**
** label.cpp Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**           Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#include "stdafx.h"
#include <GL/gl.h>
#include <math.h>
#include <string>
#include <stdexcept>
#include "btexture.h"
#include "label.h"
#include "kpuibase.h"
#include "kpconfig.h"
#include "sprinter.h"


const int Label::left[] =
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

const int Label::right[] =
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

unsigned int Label::Texture = 0;
std::string Label::TextureSource;

tActivated Label::activated;

Label::Label(const std::string &textOrFormat) :
    size(0), AspectRatio(0), x(0), y(0),
    Height(0), Alpha(0.0f),
    old_x(0),  old_y(0),  old_Height(0),  old_Alpha(0.0f),
    target_x(0), target_y(0), target_Height(0), target_Alpha(0.0f),
    Alignment(A_LEFT), Signal(0),
    DisplayList(0), hasInputFocus(false), lineCount(0),
    MaxCharacters(32), maxWidth(0),
    animationTimer(TOTAL_ANIMATIONTIME, false)
{
    if (Texture == 0)
    {
        throw std::logic_error("Label::PreInitialize has to be called first");
    }

    if (sprinter::isformatstring(textOrFormat))
    {
        format = textOrFormat;
    }
    else
    {
        labelText = textOrFormat;
    }
}

Label::Label(const Label &src) :
    format(src.format),
    labelText(src.labelText), oldLabelText(src.oldLabelText),
    size(src.size), AspectRatio(src.AspectRatio), x(src.x), y(src.y),
    Height(src.Height), Alpha(src.Alpha),
    old_x(src.old_x),  old_y(src.old_y),
    old_Height(src.old_Height), old_Alpha(src.old_Alpha),
    target_x(src.target_x), target_y(src.target_y),
    target_Height(src.target_Height), target_Alpha(src.target_Alpha),
    Alignment(src.Alignment),
    Signal(src.Signal),
    DisplayList(0), hasInputFocus(src.hasInputFocus), lineCount(src.lineCount),
    MaxCharacters(src.MaxCharacters), maxWidth(src.maxWidth),
    animationTimer(src.animationTimer)
{
}

Label::~Label()
{
    if (DisplayList != 0)
    {
        glDeleteLists(DisplayList, 1);
        DisplayList = 0;
    }
}

Label &Label::operator=(const Label &src)
{
    if (&src != this)
    {
        format = src.format;
        labelText = src.labelText;
        oldLabelText = src.oldLabelText;
        size = src.size;
        AspectRatio = src.AspectRatio;
        x = src.x;
        y = src.y;
        Height = src.Height;
        Alpha = src.Alpha;
        old_x = src.old_x;
        old_y = src.old_y;
        old_Height = src.old_Height;
        old_Alpha = src.old_Alpha;
        target_x = src.target_x;
        target_y = src.target_y;
        target_Height = src.target_Height;
        target_Alpha = src.target_Alpha;
        Alignment = src.Alignment;
        Signal = src.Signal;
        hasInputFocus = src.hasInputFocus;
        lineCount = src.lineCount;
        MaxCharacters = src.MaxCharacters;
        maxWidth = src.maxWidth;
        animationTimer = src.animationTimer;

        RecreateDisplayList();
    }

    return *this;
}

void Label::FadeOutAll()
{
    for (auto it = activated.begin(); it != activated.end(); ++it)
    {
        (*it)->SetFadeOut();
    }

    Label::activated.clear();
}

void Label::SetActive(Label *pLabel)
{
    if (activated.find(pLabel) == activated.end())
    {
        activated.insert(pLabel);
    }
}

void Label::PreInitialize(const std::string &TextureName,
                          unsigned int TextureSize,
                          bool Nearest, const KPConfig &config)
{
    BTexture texture;

    auto file1 = config.GetDirectory(KP_TEXTURE_DIR) + TextureName +
                 PATHSEPARATORSTRING + "characters.png";

    auto *texels = texture.ReadTextureFromFile(file1, TEX_WITH_ALPHA);

    if (texels == nullptr)
    {
        auto file2 = config.GetDirectory(KP_TEXTURE_DIR) + "characters.png";

        texels = texture.ReadTextureFromFile(file2, TEX_WITH_ALPHA);

        if (texels == nullptr)
        {
            std::stringstream message;

            message << "Error reading texture from file '" << file2 << "'";
            throw std::runtime_error(message.str());
        }
        else
        {
            TextureSource = file2;
        }
    }
    else
    {
        TextureSource = file1;
    }

    if (BTexture::GetExpToBase2(TextureSize) == -1)
    {
        std::stringstream message;

        message << "Error scaling texture from file '" << TextureSource << "'";
        throw std::runtime_error(message.str());
    }

    texels = texture.Rescale(BTexture::GetExpToBase2(TextureSize),
                             TEX_SMALLER | TEX_RESCALE_AVG);

    auto width  = texture.GetWidth();
    auto height = texture.GetHeight();

    if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
    {
        message(mtWarning,
                "*** Warning: width or height of '%s' is not a power of 2\n",
                TextureSource.c_str());
    }

    if (Texture == 0)
    {
        glGenTextures(1, &Texture);
    }

    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Label::Draw()
{
    RecreateDisplayList();

    if (Alpha)
    {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glTranslatef(x, y, 0);
        glScalef(Height, Height, 1);
        glColor4f(1.0, 1.0, 1.0, Alpha);
        glCallList(DisplayList);
        glPopMatrix();
    }
}

void Label::SetPosition(float X, float Y, float H, tKPAlignment A)
{

    if (A != A_DONTCARE)
    {
        Alignment = A;
    }

    switch (Alignment)
    {
        case A_LEFT:
        {
            target_x = X;
        }
        break;

        case A_CENTERED:
        {
            target_x = X - 0.5f * AspectRatio * H;
        }
        break;

        case A_RIGHT:
        {
            target_x = X - AspectRatio * H;
        }
        break;
    }

    target_y     = Y;
    target_Height = H;
    target_Alpha = MOD_FADEIN;

    if (Alpha == MOD_FADEOUT)
    {

        x = ((target_x - 8) / 1.5f) + 8;
        y = ((target_y - 6) / 1.5f) + 6;
        Height = target_Height / 1.5f;

        /*
        ax = ((24.0 * rand()) / RAND_MAX) - 4;
        ay = ((14.0 * rand()) / RAND_MAX) - 1;
        bx = ax + 2 * (bx_ - ax_);
        by = ay + 2 * (by_ - ay_);
        */
    }

    Signal = 0;

    StartAnimation();
}

void Label::SetTextOrFormat(const std::string &textOrFormat)
{
    oldLabelText = labelText;

    if (sprinter::isformatstring(textOrFormat))
    {
        format = textOrFormat;
    }
    else
    {
        labelText = textOrFormat;
    }

    RecreateDisplayList();
}

int Label::FormatText(int count, ...)
{
    auto result = 0;

    if (!format.empty())
    {
        va_list arg_ptr;

        oldLabelText = labelText;
        va_start(arg_ptr, count);
        result = sprinter::vsprintf(labelText, format.c_str(), arg_ptr);
        va_end(arg_ptr);

        RecreateDisplayList();
    }

    return result;
}

bool Label::AddCharacter(char key)
{
    if (hasInputFocus)
    {
        if (key >= ' ' && key <= '~')
        {
            // Valid ASCII character so add it if it fits in
            if (labelText.size() < MaxCharacters)
            {
                oldLabelText = labelText;
                labelText.push_back(key);
                RecreateDisplayList();
            }

            return true;
        }
        else if (key == '\b' || key == 127)
        {
            // Delete last character
            if (!labelText.empty())
            {
                oldLabelText = labelText;
                labelText = labelText.substr(0, labelText.size() - 1);
                RecreateDisplayList();
            }

            return true;
        }
        else if (!labelText.empty() && (key == '\r' || key == '\n'))
        {
            // If the text contains at least one character
            // the input will be commited with Enter or Line Feed
            target_Alpha = MOD_FADEIN;
            hasInputFocus = false;

            if (target_Alpha != Alpha)
            {
                StartAnimation();
            }

            return true;
        }
    }

    return false;
}

void Label::SetFadeOut()
{
    target_Alpha = MOD_FADEOUT;

    target_x = ((x - 8) * 1.5f) + 8;
    target_y = ((y - 6) * 1.5f) + 6;
    target_Height = Height * 1.5f;

    hasInputFocus = false;

    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Label::SetInputFocus(bool state)
{
    hasInputFocus = state;

    if (hasInputFocus)
    {
        SetFullyVisible();
    }
    else
    {
        if (Alpha && target_Alpha)
        {
            SetFadeIn();
        }
    }
}

void Label::SetSelected()
{
    Alpha = MOD_SELECTED;
    target_Alpha = MOD_FADEIN;

    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Label::SetFadeIn()
{
    target_Alpha = MOD_FADEIN;

    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Label::SetFullyVisible()
{
    target_Alpha = MOD_FULLYVISIBLE;

    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

// duration is the time since the last call to Animate() in ms
void Label::Animate(unsigned int duration)
{
    if (animationTimer.IsStarted())
    {
        if (animationTimer.Add(duration))
        {
            x = target_x;
            y = target_y;
            Height = target_Height;
            Alpha = target_Alpha;
        }
        else
        {
            GLfloat localFactor;
            GLdouble relativeTime = animationTimer.GetRelativeTime();

            localFactor = 0.5f - 0.5f *
                          cos(M_PIf * static_cast<GLfloat>(relativeTime));
            x      = (target_x - old_x) * localFactor + old_x;
            y      = (target_y - old_y) * localFactor + old_y;
            Height = (target_Height - old_Height) * localFactor + old_Height;
            Alpha  = (target_Alpha - old_Alpha) * localFactor + old_Alpha;
        }
    }
}

void Label::SetSignal(int NewSignal)
{
    Signal = NewSignal;
}

void Label::StartAnimation()
{
    SetActive(this);
    animationTimer.Restart();
    old_x = x;
    old_y = y;
    old_Height = Height;
    old_Alpha = Alpha;
}

void Label::RecreateDisplayList()
{
    auto isForceRecreate = false;
    int x, y;
    const auto w = 1.0f / 16.0f;

    if (!DisplayList)
    {
        DisplayList = glGenLists(1);

        if (DisplayList == 0)
        {
            // Could be caused if display list is totally full
            // or any other error.
            throw std::runtime_error("Error creating a display list");
        }

        isForceRecreate = true;
    }

    if (!isForceRecreate && oldLabelText.compare(labelText) == 0)
    {
        return;
    }

    if (!maxWidth)
    {
        AspectRatio = 0.0;
        GLuint c = 0;
        std::string::const_iterator it;

        lineCount = 1;
        glNewList(DisplayList, GL_COMPILE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPushMatrix();

        for (it = labelText.begin(); it != labelText.end(); ++it)
        {
            c = static_cast<unsigned char>(*it);
            AspectRatio += (right[c] - left[c] + 4) / 64.0f; // Sw: added
            glTranslatef(-left[c] / 64.0f, 0, 0);
            glBindTexture(GL_TEXTURE_2D, Texture);
            glBegin(GL_QUADS);
            x = c % 16;
            y = 15 - (c / 16);
            glTexCoord2f(w * x,     w * y);
            glVertex2f(0.0, 0.0);
            glTexCoord2f(w * (x + 1), w * y);
            glVertex2f(1.0, 0.0);
            glTexCoord2f(w * (x + 1), w * (y + 1));
            glVertex2f(1.0, 1.0);
            glTexCoord2f(w * x,     w * (y + 1));
            glVertex2f(0.0, 1.0);
            glEnd();
            glTranslatef((right[c] + 4) / 64.0f, 0, 0);
        }

        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
        glEndList();

    }
    else
    {
        decltype(labelText.size()) start = 0;
        decltype(start) Pos = 0;
        decltype(start) lastSpace = 0;
        auto spaceCount = 0;
        auto lineWidth = 0.0f;
        auto lineWidthUntilLastSpace = 0.0f;

        AspectRatio = 0.0;
        lineCount   = 0;
        glNewList(DisplayList, GL_COMPILE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        while (Pos < labelText.size())
        {
            GLuint c = 0;

            start      = Pos;
            spaceCount = 0;
            lastSpace  = 0;
            lineWidth  = 0.0f;

            while (Pos < labelText.size() && (lineWidth < maxWidth * 1.05))
            {
                c = static_cast<unsigned char>(labelText[Pos]);

                if (c == ' ')
                {
                    lineWidthUntilLastSpace = lineWidth;
                    lastSpace = Pos;
                    spaceCount++;
                }

                lineWidth += (right[c] - left[c] + 4) / 64.0f;
                Pos++;
            }

            if (Pos < labelText.size()) // Not at the end: justify left, right
            {
                GLfloat delta = (maxWidth - lineWidthUntilLastSpace) /
                                (spaceCount - 1.0f);
                GLuint c;
                glPushMatrix();
                glTranslatef(0, -lineCount * 0.7f, 0);
                Pos = start;

                while (Pos < labelText.size() && Pos < lastSpace)
                {
                    c = static_cast<unsigned char>(labelText[Pos++]);
                    glTranslatef(-left[c] / 64.0f, 0, 0);
                    glBindTexture(GL_TEXTURE_2D, Texture);
                    glBegin(GL_QUADS);
                    x = c % 16;
                    y = 15 - (c / 16);
                    glTexCoord2f(w * x,     w * y);
                    glVertex2f(0.0, 0.0);
                    glTexCoord2f(w * (x + 1), w * y);
                    glVertex2f(1.0, 0.0);
                    glTexCoord2f(w * (x + 1), w * (y + 1));
                    glVertex2f(1.0, 1.0);
                    glTexCoord2f(w * x,     w * (y + 1));
                    glVertex2f(0.0, 1.0);
                    glEnd();
                    glTranslatef((right[c] + 4) / 64.0f, 0, 0);

                    if (c == 32)
                    {
                        glTranslatef(delta, 0, 0);
                    }
                }

                glPopMatrix();
                lineCount++;
            }
            else     // At the end: justify left
            {
                auto delta = 0.0f;

                if (lineWidth > maxWidth)
                {
                    delta = (maxWidth - lineWidth) / (spaceCount + 0.0f);
                }

                GLuint c;
                glPushMatrix();
                glTranslatef(0, -lineCount * 0.7f, 0);
                Pos = start;

                while (Pos < labelText.size())
                {
                    c = static_cast<unsigned char>(labelText[Pos++]);
                    glTranslatef(-left[c] / 64.0f, 0, 0);
                    glBindTexture(GL_TEXTURE_2D, Texture);
                    glBegin(GL_QUADS);
                    x = c % 16;
                    y = 15 - (c / 16);
                    glTexCoord2f(w * x,     w * y);
                    glVertex2f(0.0, 0.0);
                    glTexCoord2f(w * (x + 1), w * y);
                    glVertex2f(1.0, 0.0);
                    glTexCoord2f(w * (x + 1), w * (y + 1));
                    glVertex2f(1.0, 1.0);
                    glTexCoord2f(w * x,     w * (y + 1));
                    glVertex2f(0.0, 1.0);
                    glEnd();
                    glTranslatef((right[c] + 4) / 64.0f, 0, 0);

                    if (c == 32)
                    {
                        glTranslatef(delta, 0, 0);
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

int Label::MouseEvent(tMouseButton button, tMouseEvent event,
                      int x_, int y_, KPUIBase &ui)
{
    auto xf = 16.0f * x_ / ui.GetValue(KP_WINDOW_WIDTH);
    auto yf = 12.0f - 12.0f * y_ / ui.GetValue(KP_WINDOW_HEIGHT);

    if (target_Alpha > 0.0 &&
        Signal != 0 &&
        x <= xf && xf <= x + Height * AspectRatio &&
        y <= yf && yf <= y + Height)
    {
        if (button == KP_LEFT_MB)
        {
            if (event == KP_BUTTON_PRESS)
            {
                SetSelected();
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

float Label::GetHeight()
{
    return 0.7f * lineCount;
}


void Label::SetMaxWidth(float maxWidth_)
{
    if (maxWidth_ < 0)
    {
        maxWidth_ = 0;
    }

    maxWidth = maxWidth_;
}

void Label::DebugPrint(void)
{
    std::cout << "format='" << format
              << "' old text='" << oldLabelText << "'"
              << "' text='" << labelText << "'"
              << std::endl;
}

