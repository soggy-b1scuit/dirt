//main.cpp
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <iostream>
#include <fstream>
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

int16_t * read_wav_data(fs::path path,uint32_t& ass) // only works for 16 bit pcm encoded in the way that that one thing specifies
{
    std::cout << path << endl;

    std::ifstream inf = path;

    uint32_t size;
     
    inf.seekg(40);
    inf.read(reinterpret_cast<char *>(&size), 4);

    char *data = new char[size];
    int16_t *data_2 = new int16_t[size];

    inf.seekg(44);
    inf.read(data,size);

    memcpy(data_2,data,size);
    ass = size/2;
    inf.close();
    delete[] data;
    
    std::ofstream outfile("rape.txt");

    for ( int i = 0; i < ass; ++i )
        outfile << (int)data_2[i] << ",";

    return data_2;

} // (also this could prolly be done better)
 
void load_assets(DirtAssetManager * assets)
{
    if ( DirectoryExists("src") )
    {
        //Load Sprites
        std::vector<std::string> preload_paths;
        preload_paths = assets->load_list("project/preload.list");
        for ( const auto &i:preload_paths ) assets->load_directory(i);
        assets->target_file = "";
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
    std::string logos[3] = {"src/textures/raylib.png","src/textures/soloud.png","src/textures/dog.png"};
    double time = 0;
    double alpha = 0;
    
    bool interrupt = false;

    bool finished = false;
    double fintime = PI*3;

    while ( !finished && !attempt_exit && !true_exit )
    {
        Texture2D texture = assets.dynamic_get_tex(logos[(int) (time * PIDIV)]);

        if ( interrupt )
        {
            if (alpha > 0) alpha = alpha * 0.995 - 0.02;
            else finished = true;
        }
        else 
        {
            time += GetFrameTime()*0.75;
            alpha = std::clamp( abs(sin(time)*500)-150, 0.0 , 255.0);
        }

        BeginDrawing();
            ClearBackground(BLACK2);
            DrawTexture( texture,160 - texture.width*0.5 ,120 - texture.height*0.5,{255,255,255,(unsigned char)(alpha)});
        EndDrawing();

        if ( !GetCharPressed() == 0 || IsKeyDown(KEY_ESCAPE) ) interrupt = true;
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

    DirtAssetManager assets;
    assets.soloud = &soloud;

    SetExitKey(KEY_NULL); // So pressing escape won't attempt to close the window

    //PROCESS ARGS HERE
    
    BeginDrawing();
        ClearBackground(BLACK2);
        DrawText("pre-loading",10,10,10,GRAY);
    EndDrawing();

    soloud.init();

    std::thread loading_thread(load_assets,&assets);

    int step;

    std::string loading_text = "pre-loading...";
    
    while ( loading )
    {
        step = int(GetTime()*3) % 4;
        BeginDrawing();
            ClearBackground(BLACK2);
            DrawText((loading_text.substr(0,15+step) + ((assets.target_file != "") ? ("\ntarget: " + assets.target_file) : "")).c_str(),10,10,10,RAYWHITE);
        EndDrawing();
    }

    loading_thread.join();

    assets.get_files_status();
    assets.push_textures();
    assets.free_cpu_textures();
    assets.load_default_assets();

    intro(assets);

    while ( !true_exit )
    {
        BeginDrawing();
            ClearBackground(BLACK2);
            DrawTextureEx(assets.get_tex("dick"),{80,20},10,2,WHITE);
        EndDrawing();

        if ( IsKeyPressed(KEY_K) )
            assets.play("DOOR_BELL");
        if ( attempt_exit ) true_exit = tinyfd_messageBox("dirt","Are you sure you want to exit?","yesno","question",1);
        
        attempt_exit = WindowShouldClose();

        Vector2 mouse_position = GetMousePosition();

        if ( soloud.getActiveVoiceCount() > 0) SoLoud::Thread::sleep(50);
    }

    assets.cleanup();
    logprint("Exiting.");
    prompt_savelog();
    
    CloseWindow();

    return 0;
}

