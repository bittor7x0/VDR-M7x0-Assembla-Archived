/*
Copyright (C) 2004-2010 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#ifndef __VAR_PARSER_INC__
#define __VAR_PARSER_INC__

#include <string>
#include <vector>
#include <vdr/config.h>
#include "epgsearchtools.h"

using std::string;
using std::vector;

typedef enum
{
    condEq = 0,
    condNeq
} condOperator;

class cVarParser
{
 public:
    string varName;
    string condEqLeft;
    string condEqRight;
    condOperator condOp;
    string condvarTrue;
    string condvarFalse;
    string compExpr;

    cCommand* cmd;
    string cmdArgs;

    string connectAddr;
    int connectPort;    

 cVarParser() : cmd(NULL), connectPort(-1) {}
    bool Parse(const string& input);
    bool ParseExp(const string& input);
    bool IsCondExpr();
    bool IsShellCmd();
    bool IsConnectCmd();
 private:
    bool ParseAssign(const string& input);
    bool ParseShellCmd(const string& input);
    bool ParseConnectCmd(const string& input);
    bool ParseCondExp(const string& input);
    bool ParseEquality(const string& input);
    bool ParseVar(const string& input);
};

#endif
