#include "matxin_string_utils.h"
#include <wctype.h>
#include <assert.h>


int watoi(const wchar_t *buffer)
{
    return wcstol(buffer, NULL, 10);
}


void Tokenize(const wstring& str,
              vector<wstring>& tokens,
              const wstring& delimiters)
{
    // Skip delimiters at beginning
    size_t lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter"
    size_t pos = str.find_first_of(delimiters, lastPos);

    while (pos != wstring::npos || lastPos != wstring::npos)
    {
        // Found a token, add it to the vector
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


void Tokenize2(const wstring& str,
               wstring& tokens,
               const wstring& delimiters)
{
    // Skip delimiters at beginning
    size_t lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter"
    size_t pos = str.find_first_of(delimiters, lastPos);

    while (pos != wstring::npos || lastPos != wstring::npos)
    {
        // Found a token, add it to the resulting string
        tokens += str.substr(lastPos, pos - lastPos);
        // Skip delimiters
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


wstring v2s(vector<wstring> vector, wstring delimiter)
{
  wstring result = L"";

  for (size_t i = 0; i < vector.size(); i++)
    result += delimiter + vector[i];

  return result;
}


vector<wstring> split_multiattrib(wstring str)
{
  vector<wstring> tokens;
  vector<wstring> result;
  wstring resultStr;

  Tokenize(str, tokens);
  for (size_t i = 0; i < tokens.size(); i++)
  {
    vector<wstring> attribs;
    vector<wstring> valueparts;
    wstring values;

    Tokenize(tokens[i], attribs, L"=");
    Tokenize2(attribs[1], values, L"'");
    Tokenize(values, valueparts, L"\\");

    for (size_t j = 0; j < valueparts.size(); j++)
    {
      resultStr = attribs[0] + L"='" + valueparts[j] + L"' ";
      if (result.size() > j)
        result[j].append(resultStr);
      else {
        result.resize(valueparts.size());
        result[j] = resultStr;
      }
    }
  }

  return result;
}

