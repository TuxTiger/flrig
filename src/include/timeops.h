// ----------------------------------------------------------------------------
// Copyright (C) 2017
//              David Freese, W1HKJ
//
// This file is part of flrig
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------


#ifndef TIMEOPS_H_
#define TIMEOPS_H_

#include "config.h"
#include <time.h>
#include <sys/time.h>

#ifdef __MINGW32__
#  include <pthread.h>
#endif

struct timespec operator+(const struct timespec &t0, const double &t);
struct timespec operator-(const struct timespec &t0, const struct timespec &t1);
struct timespec& operator-=(struct timespec &t0, const struct timespec &t1);
bool operator>(const struct timespec &t0, const struct timespec &t1);
bool operator==(const struct timespec &t0, const struct timespec &t1);

struct timeval operator+(const struct timeval &t0, const double &t);
struct timeval operator-(const struct timeval &t0, const struct timeval &t1);
struct timeval& operator-=(struct timeval &t0, const struct timeval &t1);
bool operator>(const struct timeval &t0, const struct timeval &t1);
bool operator==(const struct timeval &t0, const struct timeval &t1);

#ifndef GMTIME_R
extern struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
extern struct tm *localtime_r(const time_t *_Time,struct tm *_Tm);
#endif

#endif // TIMEOPS_H_
