//main.cpp
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <iostream>
#include <thread>
#include <filesystem>
namespace fs = std::filesystem;

#include "tinyfiledialogs.h"

#include "common.hpp"
#include "assets.hpp"

#include <algorithm>

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_thread.h"

bool loading = true;
bool attempt_exit;
bool true_exit;

void load_assets(DirtAssetManager * assets)
{
    if ( DirectoryExists("src") )
    {
        //Load Sprites
        assets->load_directory("src/");
        assets->target_file = "";
        WaitTime(MINIMUM_LOAD_TIME);
    }
    else
    {
        tinyfd_messageBox("dirt - Critical Error","No source folder found.\nExiting.","ok","error",1);
        errorlog += "no src folder";
        true_exit = true;
    }
    loading = false;
}

void intro(DirtAssetManager &assets)
{
    std::string logos[3] = {"raylib","soloud","dirtflat"};
    double time = 0;
    double alpha = 0;

    bool interrupt = false;

    bool finished = false;
    double fintime = PI*3;

    while ( !finished && !attempt_exit && !true_exit )
    {
        Texture2D texture = assets.get_tex(logos[(int) (time * PIDIV)]);

        if ( interrupt )
        {
            if (alpha > 0) time -= GetFrameTime()*1.5;
            else finished = true;
        }
        else 
        {
            time += GetFrameTime()*0.75;
        }

        alpha = std::clamp( abs(sin(time)*500)-150, 0.0 , 255.0);

        BeginDrawing();
            ClearBackground(BLACK2);
            DrawTexture( texture,160 - texture.width*0.5 ,120 - texture.height*0.5,{255,255,255,(unsigned char)(alpha)});
        EndDrawing();

        if ( !GetCharPressed() == 0 ) interrupt = true;
        if ( WindowShouldClose() ) attempt_exit = true;
        if ( time > fintime ) finished = true;
    }
    if (!attempt_exit && !true_exit) WaitTime(0.5);
    ClearWindowState(FLAG_WINDOW_UNDECORATED);
}   

int main ( int argc, char **argv )
{
    InitWindow(320,240," ");
    SetWindowState(FLAG_WINDOW_UNDECORATED);
    
    SoLoud::Soloud soloud; // SoLoud engine
    SoLoud::Wav wave;      // One wave file

    DirtAssetManager assets;
    assets.soloud = &soloud;

    SetExitKey(KEY_NULL); // So pressing escape won't attempt to close the window
    
    //PROCESS ARGS HERE

    BeginDrawing();
        ClearBackground(BLACK2);
        DrawText("loading",10,10,10,GRAY);
    EndDrawing();
    soloud.init();

    std::thread loading_thread(load_assets,&assets);

    int step;
    std::string loading_text = "loading...";
    
    while ( loading )
    {
        step = int(GetTime()*3) % 4;
        BeginDrawing();
            ClearBackground(BLACK2);
            DrawText((loading_text.substr(0,7+step) + ((assets.target_file != "") ? ("\ntarget: " + assets.target_file) : "")).c_str(),10,10,10,RAYWHITE);
        EndDrawing();
    }

    loading_thread.join();

    assets.push_textures();
    assets.get_files_status();

    intro(assets);
    
    
    wave.load("src/audio/less bass more mid high snare.ogg");
    soloud.play(wave);

    while ( !true_exit )
    {
        BeginDrawing();
            ClearBackground(BLACK2);
            DrawTexture(assets.get_tex("ass"),20,20,WHITE);
        EndDrawing();

        if ( attempt_exit ) true_exit = tinyfd_messageBox("dirt","Are you sure you want to exit?","yesno","question",1);
        attempt_exit = WindowShouldClose();

        Vector2 mouse_position = GetMousePosition();

        if ( soloud.getActiveVoiceCount() > 0) SoLoud::Thread::sleep(100);
    }

    CloseWindow();
    assets.cleanup();
    soloud.deinit();
    logprint("Exiting.");
    prompt_savelog();
    return 0;
}