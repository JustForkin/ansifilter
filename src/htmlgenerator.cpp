/***************************************************************************
                     htmlgenerator.cpp  -  description
                             -------------------

    copyright            : (C) 2007-2015 by Andre Simon
    email                : andre.simon1@gmx.de
 ***************************************************************************/

/*
This file is part of ANSIFilter.

ANSIFilter is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ANSIFilter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ANSIFilter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "htmlgenerator.h"
#include "version.h"

namespace ansifilter
{

HtmlGenerator::HtmlGenerator ():
    CodeGenerator(HTML),
    fileSuffix(".html")
{
    newLineTag="\n";
    styleCommentOpen="/*";
    styleCommentClose="*/";
    spacer=" ";
}

string HtmlGenerator::getOpenTag()
{
    ostringstream fmtStream;

    if (elementStyle.isBold()) {
        fmtStream<< "font-weight:bold;";
    }
    if (elementStyle.isItalic()) {
        fmtStream<< "font-style:italic;";
    }
    if (elementStyle.isBlink()) {
        fmtStream<< "text-decoration:blink;";
    }
    if (elementStyle.isUnderline()) {
        fmtStream<< "text-decoration:underline;";
    }
    if (elementStyle.isConceal()) {
        fmtStream<< "display:none;";
    }

    if (elementStyle.isFgColorSet()) {
        fmtStream << "color:#"
                  << elementStyle.getFgColour().getRed(HTML)
                  << elementStyle.getFgColour().getGreen(HTML)
                  << elementStyle.getFgColour().getBlue(HTML)
                  << ";";
    }

    if (elementStyle.isBgColorSet()) {
        fmtStream <<"background-color:#"
                  << elementStyle.getBgColour().getRed(HTML)
                  << elementStyle.getBgColour().getGreen(HTML)
                  << elementStyle.getBgColour().getBlue(HTML)
                  <<";";
    }

    string fmt  = fmtStream.str();
    tagIsOpen = fmt.size()>0;
    if (tagIsOpen) {
        ostringstream spanTag;
        spanTag<< "<span style=\""<<fmt<<"\">";
        return spanTag.str();
    }
    return "";
}

string HtmlGenerator::getCloseTag()
{
    string retVal = tagIsOpen ? "</span>"  : "";
    tagIsOpen = false;
    return retVal;
}

string HtmlGenerator::getGeneratorComment()
{
    ostringstream s;
    s <<"\n</body>\n</html>\n<!--HTML generated by ansifilter "
      << ANSIFILTER_VERSION << ", " <<  ANSIFILTER_URL <<"-->\n";

    return s.str();
}

string HtmlGenerator::getHeader()
{
    ostringstream os;
    os << "<!DOCTYPE html>"
       << "\n<html>\n<head>\n";
    if (encodingDefined()) {
        os << "<meta charset=\""
           << encoding
           << "\">\n";
    }
   
   os << "<style type=\"text/css\">\n";
   os << "pre {\n";
   os << "  font-family:"<< font << ";\n";
   os << "  font-size:"<< fontSize << ";\n";
   
   if (parseCP437) {
     os << "  color: #e5e5e5;\n";
   }
   os << "}\n\n";
   
   if (parseCP437||parseAsciiBin) {
     
     os << "body {  background-color: black; } \n";
   }
   os << "</style>\n";
    
   
    if (!styleSheetPath.empty()) {
        os << "<link rel=\"stylesheet\" type=\"text/css\" href=\""
           << styleSheetPath << "\">\n";
    }
    os << "<title>" << docTitle << "</title>\n";
    os << "</head>\n<body>\n<pre>";

    return os.str();
}

string HtmlGenerator::getFooter()
{
    string footer;
    footer += getCloseTag();
    footer += "</pre>" + getGeneratorComment();
    return footer;
}

void HtmlGenerator::printBody()
{
    processInput();
}

string HtmlGenerator::maskCharacter(unsigned char c)
{
    switch (c) {
    case '<' :
        return "&lt;";
        break;
    case '>' :
        return "&gt;";
        break;
    case '&' :
        return "&amp;";
        break;
    case '\"' :
        return "&quot;";
        break;
    case '\'' :
      return "&apos;";
      break;
    case '\t' : // see deletion of nonprintable chars below
        return "\t";
        break;

    case '@' :
        return "&#64;";
        break;
        
    default :
        if (c>0x1f ) { // printable?
            return string( 1, c );
        } else {
            return "";
        }
    }
}

string HtmlGenerator::maskCP437Character(unsigned char c)
{    
  switch (c) {
    case 0 :
      return " ";
      break;
      
    case '<' :
      return "&lt;";
      break;
    case '>' :
      return "&gt;";
      break;
    case '&' :
      return "&amp;";
      break;
    case '\"' :
      return "&quot;";
      break;
    case '\'' :
      return "&apos;";
      break;
    case '\t' : // see deletion of nonprintable chars below
      return "\t";
      break;
      
    case 0x01:  
      return "&#x263a;";
      break;
    case 0x02:  
      return "&#x263b;";
      break;
    case 0x03:  
      return "&#x2665;";
      break;
    case 0x04:  
      return "&#x2666;";
      break;
    case 0x05:  
      return "&#x2663;";
      break;
    case 0x06:  
      return "&#x2660;";
      break;
    case 0x08:  
      return "&#x25d8;";
      break;  
     
    case 0x0a:  
      return "&#x25d9;";
      break;
    case 0x0b:  
      return "&#x2642;";
      break;
    case 0x0c:  
      return "&#x2640;";
      break;
    
    case 0x10:  
      return "&#x25BA;";
      break;  
    case 0x11:  
      return "&#x25C4;";
      break;
    case 0x12:  
      return "&#x2195;";
      break;  
    case 0x13:  
      return "&#x203C;";
      break;  
    case 0x14:  
      return "&#x00b6;";
      break;  
    case 0x15:  
      return "&#x00a7;";
      break;  
    case 0x16:  
      return "&#x25ac;";
      break;  
    case 0x17:  
      return "&#x21A8;";
      break;  
    case 0x18:  
      return "&#x2191;";
      break;  
    case 0x19:  
      return "&#x2193;";
      break; 
    case 0x1a:  
      return "&#x2192;";
      break; 
    case 0x1b:  
      return "&#x2190;";
      break; 
    case 0x1c:  
      return "&#x221F;";
      break; 
    case 0x1d:  
      return "&#x2194;";
      break; 
    case 0x1e:  
      return "&#x25B2;";
      break; 
    case 0x1f:  
      return "&#x25BC;";
      break; 
      
    case 0x80:  
      return "&#x00c7;";
      break;  
    case 0x81:  
      return "&#x00fc;";
      break;  
    case 0x82:  
      return "&#x00e9;";
      break;  
    case 0x83:  
      return "&#x00e2;";
      break;  
    case 0x84:  
      return "&#x00e4;";
      break;  
    case 0x85:  
      return "&#x00e0;";
      break;  
    case 0x86:  
      return "&#x00e5;";
      break;  
    case 0x87:  
      return "&#x00e7;";
      break;  
    case 0x88:  
      return "&#x00ea;";
      break;  
    case 0x89:  
      return "&#x00eb;";
      break;  
    case 0x8a:  
      return "&#x00e8;";
      break;  
    case 0x8b:  
      return "&#x00ef;";
      break;  
    case 0x8c:  
      return "&#x00ee;";
      break;  
    case 0x8d:  
      return "&#x00ec;";
      break;  
    case 0x8e:  
      return "&#x00c4;";
      break;  
    case 0x8f:  
      return "&#x00c5;";
      break;  
    
    case 0x90:  
      return "&#x00c9;";
      break;  
    case 0x91:  
      return "&#x00e6;";
      break;  
    case 0x92:  
      return "&#x00c6;";
      break;  
    case 0x93:  
      return "&#x00f4;";
      break;  
    case 0x94:  
      return "&#x00f6;";
      break;  
    case 0x95:  
      return "&#x00f2;";
      break;  
    case 0x96:  
      return "&#x00fb;";
      break;  
    case 0x97:  
      return "&#x00f9;";
      break;  
    case 0x98:  
      return "&#x00ff;";
      break;  
    case 0x99:  
      return "&#x00d6;";
      break;  
    case 0x9a:  
      return "&#x00dc;";
      break;  
    case 0x9b:  
      return "&#x00a2;";
      break;  
    case 0x9c:  
      return "&#x00a3;";
      break;  
    case 0x9d:  
      return "&#x00a5;";
      break;  
    case 0x9e:  
      return "&#x20a7;";
      break;  
    case 0x9f:  
      return "&#x0192;";
      break;  
      
    
    case 0xa0:  
      return "&#x00e1;";
      break;
    case 0xa1:  
      return "&#x00ed;";
      break;
    case 0xa2:  
      return "&#x00f3;";
      break;
    case 0xa3:  
      return "&#x00fa;";
      break;
    case 0xa4:  
      return "&#x00f1;";
      break;
    case 0xa5:  
      return "&#x00d1;";
      break;
    case 0xa6:  
      return "&#x00aa;";
      break;
    case 0xa7:  
      return "&#x00ba;";
      break;
    case 0xa8:  
      return "&#x00bf;";
      break;
    case 0xa9:  
      return "&#x2310;";
      break;
    case 0xaa:  
      return "&#x00ac;";
      break;
    case 0xab:  
      return "&#x00bd;";
      break;
    case 0xac:  
      return "&#x00bc;";
      break;
    case 0xad:  
      return "&#x00a1;";
      break;
    case 0xae:  
      return "&#x00ab;";
      break;
    case 0xaf:  
      return "&#x00bb;";
      break;
      
      //shades
    case 0xb0:   
      return "&#9617;";
      break;
    case 0xb1:   
      return "&#9618;";
      break;
    case 0xb2:   
      return "&#9619;";
      break;
      
      //box drawings
    case 0xb3:   
      return "&#9474;";
      break;
    case 0xb4:   
      return "&#9508;";
      break;
    case 0xb5:   
      return "&#9569;";
      break;
    case 0xb6:   
      return "&#9570;";
      break;
    case 0xb7:   
      return "&#9558;";
      break;
    case 0xb8:   
      return "&#9557;";
      break;
    case 0xb9:   
      return "&#9571;";
      break;
    case 0xba:   
      return "&#9553;";
      break;
    case 0xbb:   
      return "&#9559;";
      break;
    case 0xbc:   
      return "&#9565;";
      break;
    case 0xbd:   
      return "&#9564;";
      break;
    case 0xbe:   
      return "&#9563;";
      break;
    case 0xbf:   
      return "&#9488;";
      break;
      
    case 0xc0:   
      return "&#9492;";
      break;
    case 0xc1:   
      return "&#9524;";
      break;
    case 0xc2:   
      return "&#9516;";
      break;
    case 0xc3:   
      return "&#9500;";
      break;
    case 0xc4:   
      return "&#9472;";
      break;
    case 0xc5:   
      return "&#9532;";
      break;
    case 0xc6:   
      return "&#9566;";
      break;
    case 0xc7:   
      return "&#9567;";
      break;
    case 0xc8:   
      return "&#9562;";
      break;
    case 0xc9:   
      return "&#9556;";
      break;
    case 0xca:   
      return "&#9577;";
      break;
    case 0xcb:   
      return "&#9574;";
      break;
    case 0xcc:   
      return "&#9568;";
      break;
    case 0xcd:   
      return "&#9552;";
      break;
    case 0xce:   
      return "&#9580;";
      break;
    case 0xcf:   
      return "&#9575;";
      break;
  
    case 0xd0:   
      return "&#9576;";
      break;
    case 0xd1:   
      return "&#9572;";
      break;
    case 0xd2:   
      return "&#9573;";
      break;
    case 0xd3:   
      return "&#9561;";
      break;
    case 0xd4:   
      return "&#9560;";
      break;
    case 0xd5:   
      return "&#9554;";
      break;
    case 0xd6:   
      return "&#9555;";
      break;
    case 0xd7:   
      return "&#9579;";
      break;
    case 0xd8:   
      return "&#9578;";
      break;
    case 0xd9:   
      return "&#9496;";
      break;
    case 0xda:   
      return "&#9484;";
      break;
          
      //https://de.wikipedia.org/wiki/Unicodeblock_Blockelemente
    case 0xdb:
      return "&#9608;";
      break;
    case 0xdc:
      return "&#9604;";
      break;
    case 0xdd:
      return "&#9612;";
      break;
    case 0xde:
      return "&#9616;";
      break;      
    case 0xdf:
      return "&#9600;";
      break;
   
    case 0xe0:
      return "&#x03b1;";
      break;
    case 0xe1:
      return "&#x00df;";
      break;
    case 0xe2:
      return "&#x0393;";
      break;
    case 0xe3:
      return "&#x03c0;";
      break;
    case 0xe4:
      return "&#x03a3;";
      break;
    case 0xe5:
      return "&#x03c3;";
      break;
    case 0xe6:
      return "&#x00b5;";
      break;
    case 0xe7:
      return "&#x03c4;";
      break;
    case 0xe8:
      return "&#x03a6;";
      break;
    case 0xe9:
      return "&#x0398;";
      break;
    case 0xea:
      return "&#x03a9;";
      break;
    case 0xeb:
      return "&#x03b4;";
      break;
      
    case 0xec:
      return "&#x221e;";
      break;
    case 0xed:
      return "&#x03c6;";
      break;
    case 0xee:
      return "&#x03b5;";
      break;
    case 0xef:
      return "&#x2229;";
      break;
      
    case 0xf0:
      return "&#x2261;";
      break;
    
    case 0xf1:
      return "&#x00b1;";
      break;
    case 0xf2:
      return "&#x2265;";
      break;
    case 0xf3:
      return "&#x2264;";
      break;
    case 0xf4:
      return "&#x2320;";
      break;
    case 0xf5:
      return "&#x2321;";
      break;
    case 0xf6:
      return "&#x00f7;";
      break;
    case 0xf7:
      return "&#x2248;";
      break;
    case 0xf8:
      return "&#x00b0;";
      break;
      
    case 0xf9:
      return "&#x2219;";
      break;
    case 0xfa:
      return "&#x00b7;";
      break;
    case 0xfb:
      return "&#x221a;";
      break;
    case 0xfc:
      return "&#x207F;";
      break;
    case 0xfd:
      return "&#x20b2;";
      break;
    case 0xfe:
      return "&#x25a0;";
      break;  
    case 0xff:
      return "&nbsp;";
      break;  
      
    default :
        if (c>0x1f && c<0x7f) { // printable?          
            return string( 1, c );
        } else {
          // std::cerr<<"Unknown: "<<(int)c<<"\n";
          return "";
        }
  }
};

void HtmlGenerator::insertLineNumber ()
{
  if ( showLineNumbers && !parseCP437) {

        ostringstream lnum;
        lnum << setw ( 5 ) << right;
        if( numberCurrentLine ) {
            *out << getCloseTag();
            lnum << lineNumber;
            *out << "<span";

            if (addAnchors) {
                *out << " id=\"l_" << lineNumber<< "\" ";
            }
            *out << " style=\"color:gray;\">";

            *out <<lnum.str() <<"</span> ";
            *out << getOpenTag();
        } else {
            *out << lnum.str(); //for indentation
        }
    }

}

}
