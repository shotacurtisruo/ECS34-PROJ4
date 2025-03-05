#include "StringUtils.h"

namespace StringUtils
{

    std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept
    {
        if (start < 0)
        {
            start = str.size() + start;
        }
        if (end < 0)
        {
            end = str.size() + end;
        }
        auto len = (end - start);
        return str.substr(start, len);
    }

    std::string Capitalize(const std::string &str) noexcept
    {
        std::string res = str;
        res[0] = toupper(res[0]);
        for (size_t i = 1; i < res.size(); i++)
        {
            res[i] = tolower(res[i]);
        }
        return res;
    }

    std::string Upper(const std::string &str) noexcept
    {
        std::string res = str;
        for (size_t i = 0; i < res.size(); i++)
        {
            res[i] = toupper(res[i]);
        }
        return res;
    }

    std::string Lower(const std::string &str) noexcept
    {
        std::string res = str;
        for (size_t i = 0; i < str.size(); i++)
        {
            res[i] = tolower(res[i]);
        }
        return res;
    }

    std::string LStrip(const std::string &str) noexcept
    {
        auto i = 0;
        while (str[i] == ' ')
        {
            i++;
        }
        return str.substr(i);
    }

    std::string RStrip(const std::string &str) noexcept
    {
        auto i = str.length() - 1;
        while (i >= 0 && str[i] == ' ')
        {
            i--;
        }
        int len = i + 1;
        return str.substr(0, len);
    }

    std::string Strip(const std::string &str) noexcept
    {
        auto i = 0;
        while (str[i] == ' ')
        {
            i++;
        }
        auto c = str.length() - 1;
        while (c >= 0 && str[c] == ' ')
        {
            c--;
        }
        int len = c - i + 1;
        return str.substr(i, len);
    }

    std::string Center(const std::string &str, int width, char fill) noexcept
    {
        std::string res = str;
        int leftspace = (width - str.length()) / 2; // had to add this first part b/c of failed tests
        int rightspace = (width - str.length()) / 2;
        if ((width - str.length()) % 2 != 0)
        {
            rightspace = rightspace + 1;
        }

        for (int i = 0; i < leftspace; i++)
        {
            res = fill + res;
        }
        for (int i = 0; i < rightspace; i++)
        {
            res = res + fill;
        }
        return res;
    }

    std::string LJust(const std::string &str, int width, char fill) noexcept
    {
        std::string res = str;
        int ind = width - str.length();
        for (int i = 0; i < ind; i++)
        {
            res = res + fill;
        }
        return res;
    }

    std::string RJust(const std::string &str, int width, char fill) noexcept
    {
        std::string res = str;
        int ind = width - str.length();
        for (int i = 0; i < ind; i++)
        {
            res = fill + res;
        }
        return res;
    }

    std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept
    {
        std::string res = str;
        auto ind = str.find(old);
        while (ind != std::string::npos)
        {
            res = res.replace(ind, old.length(), rep);
            ind = res.find(old);
        }
        return res;
    }

    std::vector<std::string> Split(const std::string &str, const std::string &splt) noexcept
    {
        std::vector<std::string> v; // Vector store stringds
        std::string res = str;      // Make a copy of the input string to edit

        // if the delimiter is empty, split by whitespace
        if (splt == "")
        {
            size_t start = 0;
            for (size_t i = 0; i < res.length(); i++)
            {

                if (res[i] == ' ' || res[i] == '\n' || res[i] == '\t') //|| res[i] == '\t' || res[i] == '\n tabs or new line i dnt think i need unless later
                {
                    if (i > start)
                    {
                        v.push_back(res.substr(start, i - start)); // Add the word before the whitespace
                    }
                    start = i + 1;
                }
            }
            // Add the last part
            if (start < res.length())
            {
                v.push_back(res.substr(start));
            }
        }
        else // Case 2: If a specific delimiter is given, split by the delimiter
        {
            size_t start = 0;
            size_t ind = res.find(splt); // Find the first occurrence of the delimiter

            while (ind != std::string::npos) // Loop while we keep finding the delimiter
            {
                v.push_back(res.substr(start, ind - start)); // Extract substring before delimiter and store
                start = ind + splt.length();                 // Move past the delimiter
                ind = res.find(splt, start);                 // Find the next occurrence of the delimiter
            }

            // Add the last part of the string after the last delimiter
            v.push_back(res.substr(start));
        }

        return v; // Return the vector containing the split parts
    }

    std::string Join(const std::string &str, const std::vector<std::string> &vect) noexcept
    {
        std::string res = vect[0];
        for (size_t i = 1; i < vect.size(); i++)
        {
            res += str + vect[i];
        }
        return res;
    }

    std::string ExpandTabs(const std::string &str, int tabsize) noexcept
    {
        std::string res;
        size_t curr = 0;
        if (tabsize == 0)
        {
            for (char c : str)
            {
                if (c != '\t')
                {
                    res += c;
                }
            }
            return res;
        }
        for (char c : str)
        {
            if (c == '\t')
            {
                size_t numspaces = tabsize - (curr % tabsize);
                res.append(numspaces, ' ');
                curr += numspaces;
            }
            else
            {
                res += c;
                curr += 1;
            }
        }
        return res;
    }

    int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept
    {
        // Replace code here
        std::string leftTemp = left;
        std::string rightTemp = right;
        // must be lowercase if ignorecase is true
        if (ignorecase)
        {
            leftTemp = StringUtils::Lower(left);
            rightTemp = StringUtils::Lower(right);
        }
        // now get the length actual sizes of the strings
        size_t leftSize = leftTemp.size();
        size_t rightSize = rightTemp.size();
        // create 2D vector to hold the distances of the strings
        std::vector<std::vector<int>> distance(leftSize + 1, std::vector<int>(rightSize + 1));
        // now this should look like a table

        for (size_t i = 0; i <= leftSize; i++)
        {
            distance[i][0] = i;
        }
        // this is for the inital row
        for (size_t j = 0; j <= rightSize; j++)
        {
            distance[0][j] = j;
        }
        // fill rest of the table
        for (size_t i = 1; i <= leftSize; i++)
        {
            for (size_t j = 1; j <= rightSize; j++)
            {
                int temp = 1;
                if (leftTemp[i - 1] == rightTemp[j - 1])
                {
                    temp = 0;
                }

                int insertion = distance[i][j - 1] + 1;
                int deletion = distance[i - 1][j] + 1;
                int substitution = distance[i - 1][j - 1] + temp;
                // these are all the costs of the operations

                if (insertion <= deletion && insertion <= substitution)
                {
                    distance[i][j] = insertion;
                }
                else if (deletion <= insertion && deletion <= substitution)
                {
                    distance[i][j] = deletion;
                }
                else
                {
                    distance[i][j] = substitution;
                }
                // this is the minimum of the three
            }
        }
        // now we have the table filled out, so we can return the last element
        // this is the distance between the two strings
        return distance[leftSize][rightSize];
    }
};