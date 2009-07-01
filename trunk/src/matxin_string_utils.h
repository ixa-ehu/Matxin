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

#ifndef MATXIN_STRING_UTILS
#define MATXIN_STRING_UTILS

#include <wchar.h>

#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * zenbaki positiboak testutik int-era pasatzeko
 */
int watoi(const wchar_t *buffer);

void Tokenize(const wstring& str,
              vector<wstring>& tokens,
              const wstring& delimiters = L" ");

void Tokenize2(const wstring& str,
               wstring& tokens,
               const wstring& delimiters = L" ");

vector<wstring> split_multiattrib(wstring str);

#ifdef __cplusplus
}
#endif

#endif
