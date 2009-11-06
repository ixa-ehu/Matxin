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

#include <string>
#include <iostream>

#include <libxml/xmlreader.h>
#include <libxml/encoding.h>

#include <lttoolbox/xml_parse_util.h>

#include "matxin_string_utils.h"

using namespace std;

string xmlc2s(xmlChar const * entrada);

wstring write_xml(wstring input);

wstring getTagName(xmlTextReaderPtr reader);

int nextTag(xmlTextReaderPtr reader);

wstring attrib(xmlTextReaderPtr reader, string const &nombre);

wstring allAttrib(xmlTextReaderPtr reader);

wstring allAttrib_except(xmlTextReaderPtr reader, wstring attrib_no);

wstring text_attrib(wstring attributes, wstring const &nombre);

wstring text_whole_attrib(wstring attributes, wstring const &nombre);

wstring text_allAttrib_except(wstring attributes, const wstring &nombre);

