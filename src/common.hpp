//common.hpp
#pragma once
//includes
#include <raylib.h>

#include "tinyfiledialogs.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <iostream>
#include <iterator>
#include <locale>

//Exit Values
extern bool attempt_exit;
extern bool true_exit;
extern bool forced_exit;

//Colors
#define TRUEGREEN (Color) {0,255,0,255}
#define TRUERED (Color) {255,0,0,255}
#define BLACK2 (Color) {10,10,10,255}
//sdaskdsao
#define endl "\n"
const double PIDIV = 1/PI;
//stuff
void FocusWindow()
{
    RestoreWindow();
    SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
    SetWindowState(FLAG_WINDOW_TOPMOST);
}

void StopFocusWindow()
{
    ClearWindowState(FLAG_WINDOW_ALWAYS_RUN);
    ClearWindowState(FLAG_WINDOW_TOPMOST);
}

bool is_even (int num)
{
    return !(num & 1);
}

std::string get_time()
{
    std::time_t t = std::time(nullptr);
    char out[100];
    std::strftime(out, 100, "%Y-%m-%d [%I-%M-%S %p]", std::localtime(&t));
    return std::string(out);
}

std::string erase_substring(std::string string, std::string substring)
{
    return string.erase(string.find(substring),substring.length());
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

//Error Handling & Warnings

std::string errorlog;

void logerror(const char *text)
{
    std::cerr << text;
    errorlog += get_time();
    errorlog += " [ERROR] ";
    errorlog += " | ";
    errorlog += text;
}

void loginfo(const char *text)
{
    std::cout << text;
    errorlog += get_time();
    errorlog += " [INFO] ";
    errorlog += " | ";
    errorlog += text;
}

void logprint(const char *text)
{
    std::cout << text;
    errorlog += get_time();
    errorlog += " | ";
    errorlog += text;
}

std::string fix_backslash(const char * cstring)
{
    std::string string = cstring;
    string = replace_all(string,"\\","/");
    return string;
}

void do_warning(const char *message)
{
    tinyfd_messageBox("caution!",message,"ok","warning",1);
}

void do_error(const char *message,bool log = true)
{
    if ( !true_exit )
    {
        true_exit = tinyfd_messageBox("oops!",TextFormat("%s\nAbort?",message),"yesno","error",1);
    }

    if ( log )
    {
        logprint(TextFormat("%s\n",message));
    }
}

int errorcheck(int error, const char *message)
{
    if ( error )
    {
        do_error(message);
        return 1;
    }
    else
    {
        return 0;
    }
}

//Other stuff

void prompt_savelog()
{
        //save log prompt
        int save = tinyfd_messageBox("dirt","Would you like to save a log?","yesno","question",1);
        if ( save )
        {
            const char *filterPatterns[] = { "*.txt" };
            FocusWindow();
            char * file_name = tinyfd_saveFileDialog("Save log.","dirtlog",1,filterPatterns,"Text files");
            if ( file_name != "")
            {
                SaveFileText(file_name,errorlog.data());
            }   
            StopFocusWindow();
        }

}