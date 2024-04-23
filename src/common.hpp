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
bool user_wants_to_abort;

//Colors
#define TRUEGREEN (Color) {0,255,0,255}
#define TRUERED (Color) {255,0,0,255}
#define BLACK2 (Color) {10,10,10,255}
//sdaskdsao
#define endl "\n"
const double PIDIV = 1/PI;
//stuff
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

void logprint(const char *text)
{
    std::cout << text;
    errorlog += get_time();
    errorlog += " | ";
    errorlog += text;
}

const char * fix_backslash(const char * cstring)
{
    std::string string = cstring;
    string = replace_all(string,"\\","/");
    return string.c_str();
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
            char * file_name = tinyfd_saveFileDialog("Save log.","dirtlog",1,filterPatterns,"Text files");
            if ( file_name != "")
            {
                SaveFileText(file_name,errorlog.data());
            }   
        }

}