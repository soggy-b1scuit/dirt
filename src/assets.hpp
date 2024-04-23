//assets.hpp
#pragma once
#include <raylib.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <unordered_map>
#include <string>

#include "texmissing.hpp"

#include "soloud.h"
#include "soloud_wav.h"

#define MINIMUM_LOAD_TIME (double)0.01

int files = 0;
int loaded_files = 0;

struct DirtAssetManager

{
    SoLoud::Soloud* soloud = nullptr;

    struct texturex
    {
        Image data;
        Texture2D texture;
        int hframes = 1;
        int vframes = 1;
    };

    struct soundx
    {
        SoLoud::Wav data;
    };

    Image missing_texture_data = {
        .data = MISSINGTEX_DATA,
        .width = MISSINGTEX_WIDTH,
        .height = MISSINGTEX_HEIGHT,
        .mipmaps = 1,
        .format = MISSINGTEX_FORMAT
    };

    texturex missing_texture;

    std::string target_file;

    std::unordered_map<std::string, texturex> textures;
    std::unordered_map<std::string, soundx> sounds;


    void load_missing_texture()
    {
        target_file = "missing texture";
        missing_texture.texture = LoadTextureFromImage(missing_texture_data);
    }

    int load_tex(const char* tex,const char* name = "",bool define_alpha_color = true,Color alpha_color = {255,0,255,255})
    {
        const char* fail_message = TextFormat("Failed to load %s. (%s)",name,tex);
        files += 1;
        target_file = tex;
        texturex texture;
        Image image = LoadImage(tex);

        if ( errorcheck( !IsImageReady(image) , fail_message ) )
        {
            WaitTime(MINIMUM_LOAD_TIME);
            return 1;
        }
        else
        {
            loaded_files += 1;
            logprint(TextFormat("%s loaded as %s successfully.\n", tex, name));
            if ( define_alpha_color )
            {
                ImageColorReplace(&image,alpha_color,BLANK);
            }        
            texture.data = image;
            textures[(name == "") ? tex : name] = texture;
            WaitTime(MINIMUM_LOAD_TIME);
            return 0;
        }

    }

    int load_snd(const char* tex,const char* name = "")
    {
        const char* fail_message = TextFormat("Failed to load %s. (%s)",name,tex);
        target_file = tex;
        soundx sound;
        int err = sound.data.load("dsasd");
        fail_message = TextFormat("Failed to load %s. (%s)\n%s",name,tex,soloud->getErrorString(err));
        
    }

    void load_directory(std::string path_string)
    {
        fs::path path{path_string};
        for ( const auto& dir_entry : fs::recursive_directory_iterator(path) )
        {
            if ( dir_entry.is_regular_file() )
            {
                fs::path file_path = fs::path(dir_entry);
                std::string file_path_string = file_path.string();
                std::string file_name = file_path.filename().string();
                std::string file_extension = file_path.extension().string();
                std::cout << ( file_extension == ".png" ) << "\n";
                if ( file_extension == ".png" || file_extension == ".bmp")
                {
                    load_tex(file_path_string.c_str(),erase_substring(file_name,file_extension).c_str());
                }
                if ( true_exit )
                {
                    break;
                }
            }
        }
    }

    void get_files_status()
    {
        logprint(TextFormat("%i out of %i attempted files succesfully loaded.\n",loaded_files,files));
    }

    void push_textures()
    {
        load_missing_texture();
        for ( auto &i:textures ) i.second.texture = LoadTextureFromImage(i.second.data);
    }

    texturex get_tex_full(std::string tex)
    {
        auto t = textures.find(tex);
        if ( t != textures.end() )
        {
            return t->second;
        }
        else
        {
            return missing_texture;
        }
    }

    Texture2D get_tex(std::string tex)
    {
        return get_tex_full(tex).texture;
    }

    void reload_texture(std::string tex)
    {
        texturex texture = get_tex_full(tex);
        UnloadTexture(texture.texture);
        texture.texture = LoadTextureFromImage(texture.data);
    }

    void cleanup()
    {
        logprint("Cleaning up Assets.\n");
        for ( auto &i:textures )
        {
            UnloadTexture(i.second.texture);
            UnloadImage(i.second.data);  
        } 
        UnloadTexture(missing_texture.texture);
    }
};
