/*
    sprinter.h


    Copyright (C) 2015-2024  W. Schwotzer

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

#ifndef SPRINTER_H
#define SPRINTER_H


#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iterator>


class sprinter
{
private:
    template<typename T>
    static std::string replaceStringByT(const std::string &format,
                                        const std::string &formatItem,
                                        T&& replacement)
    {
       // loop and replace any formatItem by replacement until no
       //formatItem is found any more.
       std::string::iterator it;
       std::string result(format);

       while ((it = std::search(result.begin(), result.end(),
                    formatItem.begin(), formatItem.end())) != result.end())
       {
           std::stringstream stream;
           std::string temp;

           std::copy(result.begin(), it, std::back_inserter(temp));
           stream << temp << replacement;
           temp.clear();
           it += formatItem.size();
           copy(it, result.end(), std::back_inserter(temp));
           stream << temp;
           result = stream.str();
       }

       return result;
    }

    // template specialization for one parameter p1
    template<typename T>
    static std::string fprint(int idx, const std::string &format, T&& p1)
    {
        std::string formatItem = "{" + std::to_string(idx) + "}";

        return replaceStringByT(format, formatItem, p1);
    }

    // template for recursively creating a formatted string for
    // n given parameters of any type.
    // Each parameter has to support operator<<
    template<typename T, typename... Args>
    static std::string fprint(int idx, const std::string &format, T&& p1,
                              Args&&... args)
    {
        std::string formatItem = "{" + std::to_string(idx) + "}";
        std::string result = replaceStringByT(format, formatItem, p1);

        return fprint(idx + 1, result, args...);
    }

public:
    // template function for printing any parameter of any type into a string
    // using variadic templates.
    // The syntax of the format string is a subset of the .NET
    // String.Format syntax:
    // {0} is replaced by the first parameter
    // ...
    // {n} is replaced by the nth parameter
    template<typename... Args>
    static std::string print(const std::string &format, Args&&... args)
    {
        return fprint(0, format, args...);
    }


    // regex search for ".*\\{[0-9]+\\}.*" in parameter text.
    // using std::regex would be really nice here but produces
    // too much code bloat.
    static inline bool isformatstring(const std::string &text)
    {
        auto it = text.cbegin();

        // Parse opening curly brackets
        while ((it = std::find(it, text.cend(), '{')) != text.cend())
        {
            // Parse one or multiple digits
            auto it2 = std::find_if(++it, text.cend(),
                         [](const std::string::value_type &ch){
                             return !std::isdigit(ch);
                         });

            // If at least one digit parse closing curly brackets
            if ((it != it2) && (it2 != text.cend()) && (*it2 == '}'))
            {
                return true;
            }

            // Parse failed. Continue with next non-digit character
            it = it2;
        };

        return false;
    }
};

#endif

