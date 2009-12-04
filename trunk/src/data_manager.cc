/*
 *  Copyright (C) 2005 IXA Research Group / IXA Ikerkuntza Taldea.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#include "data_manager.h"
#include "matxin_string_utils.h"

chunk text2chunk(wstring text)
{
  chunk output;

  size_t separator = text.find(L"/");
  if (separator == wstring::npos)
    return output;

  output.condition = text.substr(0, separator);
  output.attrib = text.substr(separator + 1);

  return output;
}


bool apply_condition(wstring attributes, wstring condition)
{
  if (condition == L"true")
  {
    return true;
  }
  else if (condition.find(L"(") != string::npos)
  {
    size_t open_position = condition.find(L"(");
    size_t close_position = condition.find(L")", open_position + 1);

    while (condition.find(L"(", open_position+1) < close_position)
    {
      open_position = condition.find(L"(", open_position + 1);
      close_position = condition.find(L")", open_position + 1);

      if (close_position == wstring::npos)
        return false;
    }
    open_position = condition.find(L"(");

    wstring condition_2 = condition.substr(open_position + 1,
                                           close_position - open_position - 1);
    bool eval = apply_condition(attributes, condition_2);

    if (open_position != 0)
    {
      wstring condition_1 = condition.substr(0, open_position);
      int and_position = condition_1.rfind(L"&&");
      int or_position = condition_1.rfind(L"||");

      if (and_position != int(wstring::npos) && and_position > or_position)
      {
        condition_1 = condition.substr(0, and_position);
        eval = eval & apply_condition(attributes, condition_1);
      }
      else if (or_position != int(wstring::npos))
      {
        condition_1 = condition.substr(0, or_position);
        eval = eval | apply_condition(attributes, condition_1);
      }
    }

    if (close_position != condition.size())
    {
      wstring condition_3 = condition.substr(close_position + 1);
      int and_position = condition_3.find(L"&&");
      int or_position = condition_3.find(L"||");

      if (and_position != int(wstring::npos) && and_position < or_position)
      {
        condition_3 = condition_3.substr(0, and_position);
        eval = eval & apply_condition(attributes, condition_3);
      }
      else if (or_position != int(wstring::npos))
      {
        condition_3 = condition_3.substr(0, or_position);
        eval = eval | apply_condition(attributes, condition_3);
      }
    }

    return eval;
  }
  else if (condition.find(L"||") != wstring::npos)
  {
    int operator_position = condition.find(L"||");

    return (apply_condition(attributes, condition.substr(0, operator_position)) || 
            apply_condition(attributes, condition.substr(operator_position + 2)));
  }
  else if (condition.find(L"&&") != wstring::npos)
  {
    int operator_position = condition.find(L"&&");

    return (apply_condition(attributes, condition.substr(0, operator_position)) &&
            apply_condition(attributes, condition.substr(operator_position + 2)));
  }
  else
  {
    if (condition.find(L"!=") != wstring::npos)
    {
      int operator_position = condition.find(L"!=");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(blank_position + 1,
                                           operator_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(int(wstring::npos)))
        blank_position = condition.size();

      if (condition[operator_position + 2] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 2,
                                       blank_position - operator_position - 2);

      return (text_attrib(attributes, attribute) != value);
    }
    else if (condition.find(L"=~") != wstring::npos)
    {
      int operator_position = condition.find(L"=~");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(blank_position + 1,
                                           operator_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+2] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 2,
                                       blank_position-operator_position - 2);

      return (text_attrib(attributes, attribute).find(value) != wstring::npos);
    }
    else if (condition.find(L"=") != wstring::npos)
    {
      int operator_position = condition.find(L"=");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(blank_position + 1,
                                           operator_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 1,
                                       blank_position-operator_position - 1);

      return (text_attrib(attributes, attribute) == value);
    }
    else if (condition.find(L">") != wstring::npos)
    {
      int operator_position = condition.find(L">");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(blank_position + 1,
                                           operator_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        wstring value = condition.substr(operator_position + 2,
                                         blank_position-operator_position - 3);
        return text_attrib(attributes, attribute) > value;
      }
      else
      {
        wstring value = condition.substr(operator_position + 1,
                                         blank_position - operator_position - 1);
        return watoi(text_attrib(attributes, attribute).c_str()) > watoi(value.c_str());
      }
    }
    else if (condition.find(L"<") != string::npos)
    {
      int operator_position = condition.find(L"<");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(blank_position + 1,
                                           operator_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        wstring value = condition.substr(operator_position + 2,
                                         blank_position - operator_position - 3);
        return text_attrib(attributes, attribute) < value;
      }
      else
      {
        wstring value = condition.substr(operator_position + 1,
                                         blank_position - operator_position - 1);
        return watoi(text_attrib(attributes, attribute).c_str()) < watoi(value.c_str());
      }
    }
  }

  return true;
}


bool apply_condition(wstring parent_attributes, wstring child_attributes, wstring condition)
{
  if (condition.find(L"(") != wstring::npos)
  {
    size_t open_position = condition.find(L"(");
    size_t close_position = condition.find(L")", open_position + 1);

    while (condition.find(L"(", open_position + 1) < close_position)
    {

      open_position = condition.find(L"(", open_position + 1);
      close_position = condition.find(L")", open_position + 1);

      if (close_position == wstring::npos)
        return false;
    }
    open_position = condition.find(L"(");

    wstring condition_2 = condition.substr(open_position + 1,
                                           close_position - open_position - 1);
    bool eval = apply_condition(parent_attributes, child_attributes, condition_2);

    if (open_position != 0)
    {
      wstring condition_1 = condition.substr(0, open_position);
      int and_position = condition_1.rfind(L"&&");
      int or_position = condition_1.rfind(L"||");

      if (and_position != int(wstring::npos) && and_position > or_position)
      {
        condition_1 = condition.substr(0, and_position);
        eval = eval & apply_condition(parent_attributes, child_attributes, condition_1);
      }
      else if (or_position != int(wstring::npos))
      {
        condition_1 = condition.substr(0, or_position);
        eval = eval | apply_condition(parent_attributes, child_attributes, condition_1);
      }
    }

    if (close_position != condition.size())
    {
      wstring condition_3 = condition.substr(close_position + 1);
      int and_position = condition_3.find(L"&&");
      int or_position = condition_3.find(L"||");

      if (and_position != int(wstring::npos) && and_position < or_position)
      {
        condition_3 = condition_3.substr(and_position + 2);
        eval = eval & apply_condition(parent_attributes, child_attributes, condition_3);
      }
      else if (or_position != int(wstring::npos))
      {
        condition_3 = condition_3.substr(or_position + 2);
        eval = eval | apply_condition(parent_attributes, child_attributes, condition_3);
      }
    }

    return eval;
  }
  else if (condition.find(L"||") != wstring::npos)
  {
    int operator_position = condition.find(L"||");

    return (apply_condition(parent_attributes, child_attributes, condition.substr(0, operator_position)) ||
            apply_condition(parent_attributes, child_attributes, condition.substr(operator_position+2)));
  }
  else if (condition.find(L"&&") != wstring::npos)
  {
    int operator_position = condition.find(L"&&");

    return (apply_condition(parent_attributes, child_attributes, condition.substr(0, operator_position)) &&
	          apply_condition(parent_attributes, child_attributes, condition.substr(operator_position+2)));
  }
  else 
  {
    if (condition.find(L"!=") != wstring::npos)
    {
      int operator_position = condition.find(L"!=");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;
      int dot_position = condition.rfind(L".", operator_position);
      if (dot_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(dot_position + 1, operator_position-dot_position - 1);
      wstring chunk = condition.substr(blank_position + 1, dot_position-blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos)) blank_position = condition.size();

      if (condition[operator_position + 2] == L'\'' and
          condition[blank_position - 1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 2,
                                       blank_position-operator_position - 2);

      if (chunk == L"parent")
        return (text_attrib(parent_attributes, attribute) != value);
      else
        return (text_attrib(child_attributes, attribute) != value);
    }
    else if (condition.find(L"=~") != wstring::npos)
    {
      int operator_position = condition.find(L"=~");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;
      int dot_position = condition.rfind(L".", operator_position);
      if (dot_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(dot_position + 1,
                                           operator_position-dot_position - 1);
      wstring chunk = condition.substr(blank_position + 1,
                                       dot_position-blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+2] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 2,
                                       blank_position-operator_position - 2);

      if (chunk == L"parent")
        return (text_attrib(parent_attributes, attribute).find(value) != wstring::npos);
      else
        return (text_attrib(child_attributes, attribute).find(value) != wstring::npos);
    }
    else if (condition.find(L"=") != wstring::npos)
    {
      int operator_position = condition.find(L"=");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;
      int dot_position = condition.rfind(L".", operator_position);
      if (dot_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(dot_position + 1,
                                           operator_position - dot_position - 1);
      wstring chunk = condition.substr(blank_position + 1,
                                       dot_position - blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        blank_position--;
        operator_position++;
      }
      wstring value = condition.substr(operator_position + 1,
                                       blank_position-operator_position - 1);

      if (chunk == L"parent")
        return (text_attrib(parent_attributes, attribute) == value);
      else
        return (text_attrib(child_attributes, attribute) == value);
    }
    else if (condition.find(L">") != wstring::npos)
    {
      int operator_position = condition.find(L">");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;
      int dot_position = condition.rfind(L".", operator_position);
      if (dot_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(dot_position + 1,
                                           operator_position-dot_position - 1);
      wstring chunk = condition.substr(blank_position + 1,
                                       dot_position-blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        wstring value = condition.substr(operator_position + 2,
                                         blank_position-operator_position - 3);

        if (chunk == L"parent")
          return text_attrib(parent_attributes, attribute) > value;
        else
          return text_attrib(child_attributes, attribute) > value;
      }
      else
      {
        wstring value = condition.substr(operator_position + 1,
                                         blank_position - operator_position - 1);

        if (chunk == L"parent")
          return watoi(text_attrib(parent_attributes, attribute).c_str()) > watoi(value.c_str());
        else
          return watoi(text_attrib(child_attributes, attribute).c_str()) > watoi(value.c_str());
      }
    }
    else if (condition.find(L"<") != wstring::npos)
    {
      int operator_position = condition.find(L"<");
      int blank_position = condition.rfind(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = -1;
      int dot_position = condition.rfind(L".", operator_position);
      if (dot_position == int(wstring::npos))
        blank_position = -1;

      wstring attribute = condition.substr(dot_position + 1,
                                           operator_position-dot_position - 1);
      wstring chunk = condition.substr(blank_position + 1,
                                       dot_position-blank_position - 1);

      blank_position = condition.find(L" ", operator_position);
      if (blank_position == int(wstring::npos))
        blank_position = condition.size();

      if (condition[operator_position+1] == L'\'' and
          condition[blank_position-1] == L'\'')
      {
        wstring value = condition.substr(operator_position + 2,
                                         blank_position - operator_position - 3);

        if (chunk == L"parent")
          return text_attrib(parent_attributes, attribute) < value;
        else
          return text_attrib(child_attributes, attribute) < value;
      }
      else
      {
        wstring value = condition.substr(operator_position + 1,
                                         blank_position - operator_position - 1);

        if (chunk == L"parent")
          return watoi(text_attrib(parent_attributes, attribute).c_str()) < watoi(value.c_str());
        else
          return watoi(text_attrib(child_attributes, attribute).c_str()) < watoi(value.c_str());
      }
    }
  }

  return true;
}


static struct chunkMovements
{
  bool isInit;
  vector<movement> movements;
} chunkAttribMovement;


void init_chunkMovement(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    size_t sep1 = lerro.find(L'\t');
    size_t sep2 = lerro.find(L'\t', sep1 + 1);
    size_t sep3 = lerro.find(L'\t', sep2 + 1);
    if (sep1 == wstring::npos || sep2 == wstring::npos || sep3 == wstring::npos)
      continue;

    movement current;
    wstring from = lerro.substr(0, sep1);
    current.from = text2chunk(from);

    wstring to = lerro.substr(sep1 + 1, sep2 - sep1 - 1);
    current.to = text2chunk(to);

    current.direction = lerro.substr(sep2 + 1, sep3 - sep2 - 1);
    current.write_type = lerro.substr(sep3 + 1);

    chunkAttribMovement.movements.push_back(current);
  }

  fitx.close();
  chunkAttribMovement.isInit = true;
}


vector<movement> get_chunk_movements(wstring fromAttributes,
                                     wstring toAttributes, wstring direction)
{
  vector<movement> output;

  for (size_t i = 0; i < chunkAttribMovement.movements.size(); i++)
  {
    if (apply_condition(fromAttributes, chunkAttribMovement.movements[i].from.condition) &&
        apply_condition(toAttributes, chunkAttribMovement.movements[i].to.condition) &&
        chunkAttribMovement.movements[i].direction == direction)
    {
      output.push_back(chunkAttribMovement.movements[i]);
    }
  }

  return output;
}


static struct nodeMovements
{
  bool isInit;
  vector<movement> movements;
} nodeAttribMovement;


void init_nodeMovement(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    size_t sep1 = lerro.find(L'\t');
    size_t sep2 = lerro.find(L'\t', sep1 + 1);
    if (sep1 == wstring::npos || sep2 == wstring::npos)
      continue;

    movement current;
    wstring from = lerro.substr(0, sep1);
    current.from = text2chunk(from);

    wstring to = lerro.substr(sep1 + 1, sep2 - sep1 - 1);
    current.to = text2chunk(to);

    current.write_type = lerro.substr(sep2+1);

    nodeAttribMovement.movements.push_back(current);
  }

  fitx.close();
  nodeAttribMovement.isInit = true;
}


vector<movement> get_node_movements_byfrom(wstring attributes)
{
  vector<movement> output;

  for (size_t i = 0; i < nodeAttribMovement.movements.size(); i++)
  {

    if (apply_condition(attributes, nodeAttribMovement.movements[i].from.condition))
    {
      output.push_back(nodeAttribMovement.movements[i]);
    }
  }

  return output;
}


vector<movement> get_node_movements_byto(wstring attributes)
{
  vector<movement> output;

  for (size_t i = 0; i < nodeAttribMovement.movements.size(); i++)
  {
    if (apply_condition(attributes, nodeAttribMovement.movements[i].to.condition))
    {
      output.push_back(nodeAttribMovement.movements[i]);
    }
  }

  return output;
}


struct lexInfo
{
  wstring name;
  map<wstring,wstring> info;
};


static vector<lexInfo> lexical_information;


void init_lexInfo(wstring name, string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  lexInfo lex;
  lex.name = name;
  wstring lerro;
  while (getline(fitx, lerro))
  {
    // Remove comments
    if (lerro.find(L'#') != wstring::npos)
      lerro = lerro.substr(0, lerro.find(L'#'));

    // Remove whitespace and so...
    for (int i = 0; i < int(lerro.size()); i++)
    {
      if (lerro[i] == L' ' and (lerro[i+1] == L' ' or lerro[i+1] == L'\t'))
        lerro[i] = L'\t';
      if ((lerro[i] == L' ' or lerro[i] == L'\t') and
          (i == 0 or lerro[i-1] == L'\t'))
      {
        lerro.erase(i,1);
        i--;
      }
    }
    if (lerro[lerro.size()-1] == L' ' or lerro[lerro.size()-1] == L'\t')
      lerro.erase(lerro.size()-1,1);

    size_t pos = lerro.find(L"\t");
    if (pos == wstring::npos)
      continue;

    wstring key = lerro.substr(0,pos);
    wstring value = lerro.substr(pos+1);

    lex.info[key] = value;
  }

  fitx.close();
  lexical_information.push_back(lex);
}


wstring get_lexInfo(wstring name, wstring key)
{
  for (size_t i = 0; i < lexical_information.size(); i++)
  {
    if (lexical_information[i].name == name)
      return lexical_information[i].info[key];
  }
  return L"";
}


struct pattern
{
  string condition;
  string patroi;    // izena aldatu...
};


struct replacement
{
  string left_context;
  string right_context;
  string attrib;
  string value;
};


static struct
{
//  vector<pattern> patterns;
  bool traza;
  vector<replacement> replacements;
} verbTransference;


void init_verbTrasference(string fileName, bool traza)
{
  ifstream fitx;
  fitx.open(fileName.c_str());

  verbTransference.traza = traza;

  string lerro;
  while (getline(fitx, lerro))
  {
    size_t pos1 = lerro.find("\t");
    if (pos1 == string::npos)
      continue;
    size_t pos2 = lerro.find("\t", pos1+1);
    if (pos2 == string::npos)
      continue;
    size_t pos3 = lerro.find("\t", pos2+1);
    if (pos3 == string::npos)
      continue;

    replacement lag;
    lag.left_context = lerro.substr(0, pos1);
    lag.attrib = lerro.substr(pos1 + 1, pos2 - pos1 - 1);
    lag.right_context = lerro.substr(pos2 + 1, pos3 - pos2 - 1);
    lag.value = lerro.substr(pos3 + 1);

    verbTransference.replacements.push_back(lag);
  }

  fitx.close();
}

wstring apply_verbTransference(wstring AS_source)
{
  string output = UtfConverter::toUtf8(AS_source);

  for (size_t i = 0; i<verbTransference.replacements.size(); i++)
  {
    string pattern = "(" + verbTransference.replacements[i].left_context + ")" +
                     verbTransference.replacements[i].attrib +
                     verbTransference.replacements[i].right_context;
    Reg_Ex regex = Reg_Ex(pattern.c_str());

    pattern = "(" + verbTransference.replacements[i].left_context +
              verbTransference.replacements[i].attrib + ")" +
              verbTransference.replacements[i].right_context;
    Reg_Ex regex_2 = Reg_Ex(pattern.c_str());

    if (regex.Search(output.c_str()) && regex_2.Search(output.c_str()))
    {
      string left_context = regex.Match(1);
      string attrib = regex_2.Match(1);
      if (verbTransference.traza)
        cerr << output << endl << endl << pattern << endl;

      size_t attrib_pos = output.find(attrib);
      // ez da egiaztatzen zerbait aurkitu duela adierazpen erregularraren
      // emaitza denez segurutzat jotzen da.
      string value = left_context + verbTransference.replacements[i].value;
      output.replace(attrib_pos, attrib.size(), value);
    }
  }

  if (verbTransference.traza)
    cerr << output << endl << endl << endl;

  return UtfConverter::fromUtf8(output);
}


struct chunk_order
{
  wstring parent_condition;
  wstring child_condition;
  wstring relative_order;
  wstring order;
};


static vector<chunk_order> order_inter;


void init_chunk_order(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    // Remove comments
    if (lerro.find(L'#') != wstring::npos)
      lerro = lerro.substr(0, lerro.find(L'#'));

    // Remove whitespace and so...
    for (int i = 0; i<int(lerro.size()); i++)
    {
      if (lerro[i] == L' ' and (lerro[i+1] == L' ' or lerro[i+1] == L'\t'))
        lerro[i] = L'\t';
      if ((lerro[i] == L' ' or lerro[i] == L'\t') and
          (i == 0 or lerro[i-1] == L'\t'))
      {
        lerro.erase(i,1);
        i--;
      }
    }
    if (lerro[lerro.size()-1] == L' ' or lerro[lerro.size()-1] == L'\t')
      lerro.erase(lerro.size()-1,1);

    size_t sep1 = lerro.find(L"\t");
    size_t sep2 = lerro.find(L"\t", sep1 + 1);
    size_t sep3 = lerro.find(L"\t", sep2 + 1);
    if (sep1 == wstring::npos || sep2 == wstring::npos || sep3 == wstring::npos)
      continue;

    chunk_order current;
    current.parent_condition = lerro.substr(0, sep1);
    current.child_condition = lerro.substr(sep1 + 1, sep2 - sep1 - 1);
    current.relative_order = lerro.substr(sep2 + 1, sep3 - sep2 - 1);
    current.order = lerro.substr(sep3 + 1, lerro.size() - sep3 - 1);

    order_inter.push_back(current);
  }

  fitx.close();
}


wstring get_chunk_order(wstring parent_attribs, wstring child_attribs,
                        int relative_order)
{
  for (size_t i = 0; i < order_inter.size(); i++)
  {

    if (apply_condition(parent_attribs, order_inter[i].parent_condition) and apply_condition(child_attribs, order_inter[i].child_condition) and
	(order_inter[i].relative_order == L".*?" or 
	 order_inter[i].relative_order[0] == L'=' and relative_order == watoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str()) or
	 order_inter[i].relative_order[0] == L'>' and relative_order > watoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str()) or
	 order_inter[i].relative_order[0] == L'<' and relative_order < watoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str())
	 )) 
    {
      return order_inter[i].order;
    }
  }

  return L"";
}


/* Prepositions transference */

struct preposition
{
  wstring cas;
  wstring condition;
  wstring maintain;
};

map<wstring, vector<preposition> > prepositions;

void init_preposition_transference(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    // Remove comments
    if (lerro.find(L'#') != wstring::npos)
      lerro = lerro.substr(0, lerro.find(L'#'));

    // Remove whitespace and so...
    for (int i = 0; i < int(lerro.size()); i++)
    {
      if (lerro[i] == L' ' and (lerro[i+1] == L' ' or lerro[i+1] == L'\t'))
        lerro[i] = L'\t';
      if ((lerro[i] == L' ' or lerro[i] == L'\t') and
          (i == 0 or lerro[i-1] == L'\t'))
      {
        lerro.erase(i,1);
        i--;
      }
    }
    if (!lerro.empty() and (lerro[lerro.size() - 1] == L' ' or lerro[lerro.size() - 1] == L'\t'))
      lerro.erase(lerro.size() - 1, 1);

    size_t sep1 = lerro.find(L"\t");
    size_t sep2 = lerro.find(L"\t", sep1 + 1);
    size_t sep3 = lerro.find(L"\t", sep2 + 1);
    if (sep1 == wstring::npos || sep2 == wstring::npos || sep3 == wstring::npos)
      continue;

    preposition current;
    wstring prep = lerro.substr(0, sep1);

    current.cas = lerro.substr(sep1 + 1, sep2 - sep1 - 1);
    current.condition = lerro.substr(sep2 + 1, sep3 - sep2 - 1);
    current.maintain = lerro.substr(sep3 + 1, lerro.size() - sep3 - 1);

    prepositions[prep].push_back(current);
  }

  fitx.close();

}


vector<wstring>
  preposition_transference(wstring parent_attributes, wstring child_attributes,
                           wstring sentenceref, int sentencealloc, config &cfg)
{
  vector<wstring> maintain_cases;

  wstring prep = text_attrib(child_attributes, L"prep");
  int alloc = watoi(text_attrib(child_attributes, L"alloc").c_str()) - sentencealloc;

  if (prep == L"")
    prep = L"-";

  if (prepositions[prep].size() == 0)
  {
    maintain_cases.push_back(L"[ZERO]");
    wcerr << L"WARNING: unknown preposition " << prep << endl;
    return maintain_cases;
  }

  vector<preposition> current_prep = prepositions[prep];

  if (cfg.DoPrepTrace)
    wcerr << sentenceref << L":" << alloc << L":" << prep;
  for (size_t i = 0; i < current_prep.size(); i++)
  {
    if (cfg.DoPrepTrace)
      wcerr << L"\t" << current_prep[i].cas << L"::"
            << current_prep[i].condition << L"::" << current_prep[i].maintain
            << endl;
    if (current_prep[i].maintain == L"+")
      maintain_cases.push_back(current_prep[i].cas);

    if (cfg.UsePrepRules and current_prep[i].condition != L"-" and
        apply_condition(parent_attributes, child_attributes, current_prep[i].condition))
    {
      vector<wstring> translation_case;

      translation_case.push_back(current_prep[i].cas);

      if (cfg.DoPrepTrace)
        wcerr << endl << L"ERREGELEN BIDEZ ITZULI DA (" << sentenceref
              << L":" << alloc << L":" << prep << L"): " << current_prep[i].cas
              << endl << endl;
      return translation_case;
    }
  }

  if (maintain_cases.size() == 0)
  {
    maintain_cases.push_back(L"[ZERO]");
    wcerr << L"WARNING: unknown preposition " << prep << endl;
  }

  if (cfg.DoPrepTrace)
  {
    wcerr << endl << L"HIZTEGIKO ERANTZUNA(" << sentenceref << L":" << alloc
          << L":" << prep << L"): ";
    for (size_t j = 0; j < maintain_cases.size(); j++)
      wcerr << maintain_cases[j] << L" ";
    cerr << endl << endl;
  }

  return maintain_cases;
}


/* Lexical selection */

struct disambiguation_rule
{
  wstring condition;
  wstring default_value;
};

map<wstring, map<wstring, disambiguation_rule > > disambiguation_rules;


void init_lexical_selection(string filename)
{
  wifstream file;
  wstring trgt_lemma;
  wstring lerro;

  file.open(filename.c_str());

  while (getline(file, lerro))
  {
    // Remove comments
    if (lerro.find(L'#') != wstring::npos)
      lerro = lerro.substr(0, lerro.find(L'#'));

    // Strip whitespaces
    for (int i = 0; i < int(lerro.size()); i++)
    {
      if (lerro[i] == L' ' and (lerro[i+1] == L' ' or lerro[i+1] == L'\t'))
        lerro[i] = L'\t';

      if ((lerro[i] == L' ' or lerro[i] == L'\t') and
          (i == 0 or lerro[i-1] == L'\t'))
      {
        lerro.erase(i, 1);
        i--;
      }
    }
    if (lerro[lerro.size()-1] == L' ' or lerro[lerro.size()-1] == L'\t')
      lerro.erase(lerro.size()-1, 1);

    size_t sep1 = lerro.find(L"\t");
    size_t sep2 = lerro.find(L"\t", sep1 + 1);
    size_t sep3 = lerro.find(L"\t", sep2 + 1);
    if (sep1 == wstring::npos or sep2 == wstring::npos or sep3 == wstring::npos)
      continue;

    disambiguation_rule current;
    wstring src_lemma = lerro.substr(0, sep1);
    if (src_lemma[0] == L'\'' && src_lemma[src_lemma.size()-1] == L'\'') src_lemma = src_lemma.substr(1,src_lemma.size()-2);

    trgt_lemma = lerro.substr(sep1 + 1, sep2 - sep1 - 1);
    current.condition = lerro.substr(sep2 + 1, sep3 - sep2 - 1);
    current.default_value = lerro.substr(sep3 + 1, lerro.size() - sep3 - 1);

    if (trgt_lemma[0] == L'\'' && trgt_lemma[trgt_lemma.size()-1] == L'\'') trgt_lemma = trgt_lemma.substr(1,trgt_lemma.size()-2);

    disambiguation_rules[src_lemma][trgt_lemma] = current;
  }

  file.close();

}


vector<wstring>
  lexical_selection(wstring parent_attributes, wstring common_attribs,
                  vector<wstring> child_attributes, config &cfg)
{
  wstring src_lemma;
  wstring trgt_lemma;
  wstring attributes;
  vector<wstring> default_case;

  src_lemma = text_attrib(common_attribs, L"slem");

  // Save the first value just in case there's no default set in the rules
  if (child_attributes.size() > 0)
    default_case.push_back(child_attributes[0]);

  for (size_t i = 0; i < child_attributes.size(); i++)
  {
    attributes = common_attribs + L" " + child_attributes[i];

    trgt_lemma = text_attrib(attributes, L"lem");
    disambiguation_rule current_rule = disambiguation_rules[src_lemma][trgt_lemma];

    if (current_rule.default_value == L"+")
      default_case.insert(default_case.begin(), child_attributes[i]);

    if (cfg.UseLexRules && current_rule.condition != L"" and
        apply_condition(parent_attributes, attributes,
                        current_rule.condition))
    {
      vector<wstring> translation_case;

      translation_case.push_back(child_attributes[i]);

      return translation_case;
    }

  }

  return default_case;
}



struct subcategorisation
{
  wstring trans;
  wstring subj_case;
  wstring cases;
};

map<wstring, vector<subcategorisation> > subcategorisations;


void init_verb_subcategorisation(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    // Remove whitespace and so...
    for (int i = 0; i < int(lerro.size()); i++)
      {
	if (lerro[i] == L' ' and (lerro[i+1] == L' ' or lerro[i+1] == L'\t'))
	  lerro[i] = L'\t';
	if ((lerro[i] == L' ' or lerro[i] == L'\t') and
	    (i == 0 or lerro[i-1] == L'\t'))
	  {
	    lerro.erase(i,1);
	    i--;
	  }
      }
    if (0 < lerro.size() and (lerro[lerro.size()-1] == L' ' or
			      lerro[lerro.size()-1] == L'\t'))
      lerro.erase(lerro.size() - 1, 1);

    size_t sep1 = lerro.find(L"\t");
    if (sep1 == wstring::npos)
      continue;

    wstring verb_lem = lerro.substr(0, sep1);
    lerro = lerro.substr(sep1 + 1);

    while (lerro.find(L"#") != wstring::npos)
    {
      wstring subcat = lerro.substr(0, lerro.find(L"#"));
      lerro = lerro.substr(lerro.find(L"#") + 1);

      size_t sep2 = subcat.find(L'/');
      size_t sep3 = subcat.find(L'/', sep2 + 1);
      if (sep2 == wstring::npos || sep3 == wstring::npos)
        continue;

      subcategorisation current;
      current.trans = subcat.substr(0, sep2);
      current.subj_case = subcat.substr(sep2 + 1, sep3 - sep2 - 1);
      current.cases = subcat.substr(sep3 + 1);

      subcategorisations[verb_lem].push_back(current);
    }
  }

  fitx.close();
}


wstring get_case (wstring input)
{
  if (input == L"[ZERO]")
    return L"";

  if (input.rfind(L"/") != wstring::npos)
    return input.substr(input.rfind(L"/") + 1);

  if (input.rfind(L"++") != wstring::npos)
    return input.substr(input.rfind(L"++") + 2);

  if (input[0] == L'[' && input[input.size() - 1] == L']')
    return input.substr(1, input.size() - 2);

  return input;
}


int match_subcategorisation(vector<vector<wstring> > &cases,
                            wstring subcat_pattern)
{
  if (cases.size() == 0)
    return 0;

  wstring current_case = L"";
  vector<wstring> current_component = cases[0];

  vector<vector<wstring> > best_cases = cases;
  int best_fixed = 0;
  bool current_fixed;

  for (size_t i = 0; i<current_component.size(); i++)
  {
    vector<vector<wstring> > extra_components = cases;
    extra_components.erase(extra_components.begin());
    int fixed_cases = 0;

    if ((current_case = get_case(current_component[i])) != L"")
      {
	size_t position = subcat_pattern.find(current_case);
	current_fixed = false;
	
	if (position != wstring::npos &&
	    (position == 0 || subcat_pattern[position-1] == L'-') and
	    (position+current_case.size() == subcat_pattern.size() or
	     subcat_pattern[position+current_case.size()] == L'-'))
	  {
	    current_fixed = true;
	    fixed_cases++;
	    subcat_pattern.erase(position, current_case.size());
	    
	    if (subcat_pattern.find(L"--") != string::npos)
	      subcat_pattern.erase(subcat_pattern.find(L"--"), 1);
	    if (subcat_pattern[0] == L'-')
	      subcat_pattern.erase(0, 1);
	    if (subcat_pattern[subcat_pattern.size() - 1] == L'-')
	      subcat_pattern.erase(subcat_pattern.size() - 1, 1);
	  }
      }

    if (subcat_pattern.size() > 0)
      fixed_cases += match_subcategorisation(extra_components, subcat_pattern);

    if (fixed_cases > best_fixed)
    {
      best_fixed = fixed_cases;
      best_cases.clear();
      if (current_fixed)
      {
        vector<wstring> current_case_array;
        current_case_array.push_back(current_component[i]);
        best_cases.push_back(current_case_array);
      }
      else
      {
        best_cases.push_back(current_component);
      }

      for (size_t j = 0; j < extra_components.size(); j++)
        best_cases.push_back(extra_components[j]);
    }
  }

  cases = best_cases;
  return best_fixed;
}


wstring
  verb_subcategorisation(wstring verb_lemma, vector<vector<wstring> > &cases,
                         vector<wstring> &attributes, vector<wstring> &subj_cases,
                         wstring subj_attributes, wstring sentenceref,
                         int sentencealloc, config &cfg)
{
  vector<subcategorisation> subcat = subcategorisations[verb_lemma];

  if (cfg.DoPrepTrace)
  {
    wcerr << L"AZPIKATEGORIZAZIOA APLIKATZEN: " << verb_lemma << L" (" << subcat.size() << L" azpikategorizazio desberdin aurkitu dira)" << endl;
    wstring prep = text_attrib(subj_attributes, L"prep");
    if (prep == L"")
      prep = L"-";
    int alloc = watoi(text_attrib(subj_attributes, L"alloc").c_str()) - sentencealloc;
    wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t";
    for (size_t j = 0; j < subj_cases.size(); j++)
      wcerr << subj_cases[j] << L" ";
    cerr << endl << endl;

    for (size_t i = 0; i < cases.size(); i++)
    {
      prep = text_attrib(attributes[i], L"prep");
      if (prep == L"")
        prep = L"-";
      alloc = watoi(text_attrib(attributes[i], L"alloc").c_str()) - sentencealloc;
      wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t";
      for (size_t j = 0; j < cases[i].size(); j++)
        wcerr << cases[i][j] << L" ";
      cerr << endl;
    }
    cerr << endl;
  }

  bool matches_subj, best_matches_subj;
  int best_fixed = -1;
  int best_length = 0;
  int cases_size = 0;;
  wstring best_trans = L"DU"; if (subcat.size() > 0) best_trans = subcat[0].trans;
  wstring best_subj_case;
  vector<vector<wstring> > best_cases = cases;
  matches_subj = best_matches_subj = false;

  for (size_t i = 0; i < cases.size(); i++)
  {
    for (size_t j = 0; j<cases[i].size(); j++)
    {
      if (get_case(cases[i][j]) != L"")
      {
        cases_size++;
        break;
      }
    }
  }

  for (size_t i = 0; i < subcat.size(); i++)
  {
    vector<vector<wstring> > final_cases = cases;
    wstring subcat_pattern = subcat[i].cases;

    matches_subj = (subj_cases.size() == 0);
    for (size_t j = 0; j < subj_cases.size(); j++)
      if (get_case(subj_cases[j]) == subcat[i].subj_case)
        matches_subj = true;

    int fixed_cases = match_subcategorisation(final_cases, subcat_pattern);

    int subcat_length = 0;
    if (subcat_pattern.size() != 0)
      subcat_length++;
    for (size_t j = 0; j < subcat_pattern.size(); j++)
    {
      if (subcat_pattern[j] == L'-')
        subcat_length++;
    }

    if (cfg.DoPrepTrace)
      wcerr << subcat_pattern << L"/" << subcat[i].subj_case << L"/"
            << subcat[i].trans << L" || " << fixed_cases  << L"/"
            << cases_size << L"/" << subcat_length << endl;

    if (fixed_cases == cases_size && (matches_subj))
    {
      cases = final_cases;

      if (cfg.DoPrepTrace)
      {
        wstring prep = text_attrib(subj_attributes, L"prep");
        if (prep == L"")
          prep = L"-";
        int alloc = watoi(text_attrib(subj_attributes, L"alloc").c_str()) - sentencealloc;
        wcerr << endl << L"AZPIKATEGORIZAZIOAREN EMAITZA: '" << verb_lemma
              << L"' aditzaren transitibitatea: " << subcat[i].trans << endl;
        wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t["
              << subcat[i].subj_case << L"]" << endl << endl;
        for (size_t k = 0; k < cases.size(); k++)
        {
          prep = text_attrib(attributes[k], L"prep");
          if (prep == L"")
            prep = L"-";
          alloc = watoi(text_attrib(attributes[k], L"alloc").c_str()) - sentencealloc;
          wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t";
          for (size_t j = 0; j < cases[k].size(); j++)
            wcerr << cases[k][j] << L" ";
          cerr << endl;
        }
        cerr << endl;
      }

      subj_cases.clear();
      subj_cases.push_back(L"[" + subcat[i].subj_case + L"]");

      return subcat[i].trans;
    }
    else if ((matches_subj or !best_matches_subj) and
             (fixed_cases > best_fixed ||
             (fixed_cases == best_fixed && best_length > subcat_length)))
    {
      if (matches_subj)
        best_subj_case = subcat[i].subj_case;
      best_fixed = fixed_cases;
      best_length = subcat_length;
      best_cases = final_cases;
      best_trans = subcat[i].trans;
    }
  }

  cases = best_cases;
  if (best_subj_case != L"")
  {
    subj_cases.clear();
    subj_cases.push_back(L"[" + best_subj_case + L"]");
  }

  if (cfg.DoPrepTrace)
  {
    wstring prep = text_attrib(subj_attributes, L"prep");
    if (prep == L"")
      prep = L"-";
    int alloc = watoi(text_attrib(subj_attributes, L"alloc").c_str()) - sentencealloc;
    wcerr << endl << L"AZPIKATEGORIZAZIOAREN EMAITZA: '" << verb_lemma
          << L"' aditzaren transitibitatea: " << best_trans << endl;
    wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t"
          << best_subj_case << endl << endl;
    for (size_t i = 0; i < cases.size(); i++)
    {
      prep = text_attrib(attributes[i], L"prep");
      if (prep == L"")
        prep = L"-";
      alloc = watoi(text_attrib(attributes[i], L"alloc").c_str()) - sentencealloc;
      wcerr << sentenceref << L":" << alloc << L":" << prep << L"\t";
      for (size_t j = 0; j < cases[i].size(); j++)
        wcerr << cases[i][j] << L" ";
      cerr << endl;
    }
    cerr << endl;
  }

  return best_trans;
}


map <wstring, map<wstring, vector<wstring> > > verb_noun_subcat;


void init_verb_noun_subcategorisation(string fitxName)
{
  wifstream fitx;
  fitx.open(fitxName.c_str());

  wstring lerro;
  while (getline(fitx, lerro))
  {
    // Remove whitespace and so...
    for (int i = 0; i < int(lerro.size()); i++)
    {
      if (lerro[i] == L' ' and (lerro[i + 1] == L' ' or lerro[i + 1] == L'\t'))
        lerro[i] = L'\t';
      if ((lerro[i] == L' ' or lerro[i-1] == L'\t') and
          (i == 0 or lerro[i] == L'\t'))
      {
        lerro.erase(i, 1);
        i--;
      }
    }
    if (lerro[lerro.size() - 1] == L' ' or lerro[lerro.size() - 1] == L'\t')
      lerro.erase(lerro.size() - 1, 1);

    size_t sep1 = lerro.find(L"\t");
    if (sep1 == wstring::npos)
      continue;

    wstring verb_lem = lerro.substr(0, sep1);

    lerro = lerro.substr(sep1 + 1);
    while (lerro.find(L"#") != string::npos)
    {
      wstring current = lerro.substr(0, lerro.find(L"#"));
      lerro = lerro.substr(lerro.find(L"#") + 1);

      size_t sep2 = current.find(L"/");
      if (sep2 == wstring::npos)
        continue;

      wstring chunk_head = current.substr(0, sep2);

      vector<wstring> cases;
      current = current.substr(sep2 + 1) + L"/";
      while (current.find(L"/") != wstring::npos)
      {
        wstring cas = current.substr(0, current.find(L"/"));
        cases.push_back(cas);
        current = current.substr(current.find(L"/") + 1);
      }

      verb_noun_subcat[verb_lem][chunk_head] = cases;
    }
  }

  fitx.close();
}


wstring to_upper(wstring input)
{
  wstring output;

  for (size_t i = 0; i < input.size(); i++)
  {
    output += toupper(input[i]);
  }

  return output;
}


vector<wstring>
  verb_noun_subcategorisation(wstring verb_lemma, wstring chunk_head,
                              vector<wstring> &cases, wstring &attributes,
                              wstring sentenceref, int sentencealloc, config &cfg)
{
  vector<wstring> subcat = verb_noun_subcat[verb_lemma][chunk_head];

  int alloc = watoi(text_attrib(attributes, L"alloc").c_str()) - sentencealloc;
  wstring prep = text_attrib(attributes, L"prep");
  if (prep == L"")
    prep = L"-";
  if (cfg.DoPrepTrace)
  {
    wcerr << sentenceref << L":" << alloc << L":" << prep
          << L"\tTRIPLETEN APLIKAZIOA: " << verb_lemma << L","
          << chunk_head << L"(";
    for (size_t i = 0; i < subcat.size(); i++)
    {
      wcerr << L" " << subcat[i];
    }
    wcerr << L") [";
    for (size_t i = 0; i < cases.size(); i++)
    {
      wcerr << L" " << cases[i];
    }
    wcerr << L"]" << endl;
  }

  for (size_t i = 0; i < subcat.size(); i++)
  {
    vector<wstring> output;
    wstring subcat_cases = subcat[i];

    while (subcat_cases.size() > 0)
    {
      wstring cas;
      if (subcat_cases.find(L"=") != wstring::npos)
      {
        cas = subcat_cases.substr(0, subcat_cases.find(L"="));
        subcat_cases = subcat_cases.substr(subcat_cases.find(L"=") + 1);
      }
      else
      {
        cas = subcat_cases;
        subcat_cases = L"";
      }

      for (size_t j = 0; j < cases.size(); j++)
      {
        if (to_upper(cas) == get_case(cases[j]))
        {
          output.push_back(cases[j]);
        }
      }
    }

    if (output.size() > 0)
    {
      if (cfg.DoPrepTrace)
      {
        wcerr << sentenceref << L":" << alloc << L":" << prep
              << L"\tTRIPLETEKIN ONDOREN KASU POSIBLEAK: ";
        for (size_t i = 0; i < output.size(); i++)
        {
          wcerr << output[i] << L" ";
        }
        cerr << endl << endl;
      }

      return output;
    }
  }

  if (cfg.DoPrepTrace)
  {
    wcerr << sentenceref << L":" << alloc << L":" << prep
          << L"\tTRIPLETEKIN ONDOREN KASU POSIBLEAK: ";
    for (size_t i = 0; i < cases.size(); i++)
    {
      wcerr << cases[i] << L" ";
    }
    cerr << endl << endl;
  }

  return cases;
}


struct node_order
{
  string chunk_type;
  string order;
};


static vector<node_order> order_intra;


void init_node_order(string fitxName)
{
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro))
  {
    size_t sep1 = lerro.find("\t");
    if (sep1 == string::npos)
      continue;

    node_order current;
    current.chunk_type = lerro.substr(0, sep1);
    current.order = lerro.substr(sep1 + 1, lerro.size() - sep1);

    order_intra.push_back(current);
  }

  fitx.close();
}


wstring get_node_order(wstring chunk_type, wstring nodes)
{
  string node_order;
  vector<int> final_order;

  for (size_t i = 0; i < order_intra.size(); i++)
  {
    if (order_intra[i].chunk_type == UtfConverter::toUtf8(chunk_type))
    {
      node_order = order_intra[i].order;
    }
  }

  wstring current_pos;
  int current_order = 0;
  size_t delimiter = nodes.find(L')');
  while (delimiter != string::npos)
  {
    current_pos = nodes.substr(0, delimiter + 1);
    nodes = nodes.substr(delimiter + 1);
    final_order.push_back(current_order);

    ostringstream order_string;
    order_string << "<" << current_order << ">";
    if (node_order.rfind(UtfConverter::toUtf8(current_pos)) != string::npos)
      node_order.replace(node_order.rfind(UtfConverter::toUtf8(current_pos)),
                                          current_pos.size(), order_string.str());
    else
      node_order += order_string.str();

    current_order++;
    delimiter = nodes.find(L')');
  }


  delimiter = node_order.find('(');
  while (delimiter != string::npos)
  {
    size_t size = node_order.find(')') - delimiter;
    node_order.erase(delimiter, size + 1);
    delimiter = node_order.find('(');
  }


  size_t start = node_order.find('<');
  size_t end = node_order.find('>');
  int current_position = 0;
  while (start != string::npos && end != string::npos)
  {
    int old_position  = atoi(node_order.substr(start + 1, end - start - 1).c_str());
    final_order[old_position] = current_position;
    start = node_order.find('<',end + 1);
    end = node_order.find('>',end + 1);
    current_position++;
  }

  wostringstream order;
  for (size_t i = 0; i < final_order.size(); i++)
  {
    if (i == 0)
      order << final_order[i];
    else
      order << L"-" << final_order[i];
  }

  return order.str();
}


struct generation_order
{
  string regex;
  string order;
};


static vector<generation_order> orders;


void init_generation_order(string fitxName)
{
  ifstream fitx;
  fitx.open(fitxName.c_str());

  generation_order gen_order;
  string lerro;
  while (getline(fitx, lerro))
  {
    size_t pos_comment = lerro.find("#");
    if (pos_comment != string::npos)
      continue;

    size_t pos_separator = lerro.rfind("\t");
    if (pos_separator == string::npos)
      continue;

    gen_order.regex = lerro.substr(0, pos_separator);

    if (gen_order.regex.find("\t") != string::npos)
      gen_order.regex = gen_order.regex.substr(0, gen_order.regex.find("\t"));
    for (size_t i = 0; i < gen_order.regex.size(); i++)
    {
      if (gen_order.regex[i] == '[' or gen_order.regex[i] == ']')
      {
        gen_order.regex.insert(i,"\\");
        i++;
      }
    }

    gen_order.order = lerro.substr(pos_separator + 1);
    orders.push_back(gen_order);
  }

  fitx.close();
}

wstring
  get_generation_order(wstring prefix, wstring lemmaMorf, wstring number,
                       wstring cas, wstring head_sem)
{
  string generation = UtfConverter::toUtf8(prefix + lemmaMorf + L">" + number +
                      L">" + cas + L">" + head_sem);

  for (size_t i = 0; i < orders.size(); i++)
  {
    Reg_Ex regex = Reg_Ex(orders[i].regex.c_str());
    if (regex.Search(generation.c_str()))
    {
      return UtfConverter::fromUtf8(orders[i].order);
    }
  }
  return L"";
}

