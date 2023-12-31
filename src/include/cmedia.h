// ----------------------------------------------------------------------------
// Copyright (C) 2020
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef CMEDIA_H
#define CMEDIA_H

#include <string>

/*
#ifdef __WIN32__
#  include <winsock2.h>
//#  include "par_nt.h"
#endif
*/

extern bool set_cmedia(int);
extern int  get_cmedia();
extern int  open_cmedia(std::string);
extern void close_cmedia();
extern void init_hids();
extern void test_hid_ptt();

#endif
