/*
 * Copyright (c) 2019 Marco Pulimeno, University of Salento
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * provided that the above copyright notice(s) and this permission notice
 * appear in all copies of the Software and that both the above copyright
 * notice(s) and this permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
 * LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization of the
 * copyright holder.
 */

#ifndef TEXTCOLOR_MACROS_H
#define TEXTCOLOR_MACROS_H

#define FG_RED(s)           "\e[38;5;1m"s"\e[0m"
#define FG_GREEN(s)         "\e[38;5;2m"s"\e[0m"
#define FG_YELLOW(s)        "\e[38;5;3m"s"\e[0m"
#define FG_BLUE(s)          "\e[38;5;4m"s"\e[0m"
#define FG_MAGENTA(s)       "\e[38;5;5m"s"\e[0m"
#define FG_CYAN(s)          "\e[38;5;6m"s"\e[0m"
#define FG_LIGHTGRAY(s)     "\e[38;5;7m"s"\e[0m"
#define FG_GRAY(s)          "\e[38;5;8m"s"\e[0m"
#define FG_LIGHTRED(s)      "\e[38;5;9m"s"\e[0m"
#define FG_LIGHTGREEN(s)    "\e[38;5;10m"s"\e[0m"
#define FG_LIGHTYELLOW(s)   "\e[38;5;11m"s"\e[0m"
#define FG_LIGHTBLUE(s)     "\e[38;5;12m"s"\e[0m"
#define FG_LIGHTMAGENTA(s)  "\e[38;5;13m"s"\e[0m"
#define FG_LIGHTCYAN(s)     "\e[38;5;14m"s"\e[0m"
#define FG_WHITE(s)         "\e[38;5;15m"s"\e[0m"

#endif //TEXTCOLOR_MACROS_H
