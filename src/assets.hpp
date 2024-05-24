//assets.hpp
#pragma once
#include <raylib.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <vector>

#include <unordered_map>
#include <algorithm>
#include <string>

#include "texmissing.hpp"
#include "sndmissing.hpp"
#include "icon.hpp"

#include "soloud.h"
#include "soloud_wav.h"

#define MINIMUM_LOAD_TIME (double)0.0

struct DirtAssetManager

{
    const char * VersionString = "0.0";

    uint16_t files = 0;
    uint16_t loaded_files = 0;
    
    //SoLoud engine pointer
    SoLoud::Soloud* soloud = nullptr; 
    
    //Asset types 
    struct texturex
    {
        bool hastexture = false;
        bool hasdata = false;

        Image data;
        Texture2D texture;

        uint8_t hframes = 1;
        uint8_t vframes = 1;
    };
    
    struct soundx
    {
        SoLoud::Wav data;
    };
    
    struct script
    {
        std::string data;
    };

    const Image missing_texture_data = {
        .data = MISSINGTEX_DATA,
        .width = MISSINGTEX_WIDTH,
        .height = MISSINGTEX_HEIGHT,
        .mipmaps = 1,
        .format = MISSINGTEX_FORMAT
    };

    const Image icon_image = {
        .data = ICON_DATA,
        .width = ICON_WIDTH,
        .height = ICON_HEIGHT,
        .mipmaps = 1,
        .format = ICON_FORMAT
    };

    texturex missing_texture;
    SoLoud::Wav missing_sound;

    std::string target_file;

    std::unordered_map<std::string, texturex> textures;
    std::vector<std::string> failed_textures = {};

    std::unordered_map<std::string, soundx> sounds;
    std::vector<std::string> failed_sounds = {};

    std::unordered_map<std::string, script> scripts;
    std::vector<std::string> failed_scripts = {};

    bool asset_failed(std::string name,const std::vector<std::string> &vector)
    {
        return std::binary_search(vector.begin(), vector.end(), name);
    }

    void load_default_assets()
    {
        target_file = "missing texture";
        missing_texture.texture = LoadTextureFromImage(missing_texture_data);

        int16_t * missing_snd_data;
        missing_snd_init(&missing_snd_data);
        missing_sound.loadRawWave16(missing_snd_data,MISSING_SND_SIZE);
        missing_snd_end(&missing_snd_data);
        
        SetWindowIcon(icon_image);
        SetWindowTitle(TextFormat("dirt v%s",VersionString));
    }

    std::string get_file_name(fs::path filepath)
    {
        const std::string file_name = filepath.filename().replace_extension("").string();
        return file_name;
    }

    std::vector<std::string> load_list(fs::path path)
    {
        std::ifstream list = path;
        std::string string{};
        std::vector<std::string> vector;
        while (std::getline(list, string)) vector.push_back(string);
        list.close();
        return vector;
    }

    //i wouldn't set a custom name if i were anyone, just name your files how you want them to appear...
    int load_tex(const char* tex,const char* name = "",bool define_alpha_color = true,Color alpha_color = {255,0,255,255})
    {
        target_file = tex;
        files++;
        const char* fail_message = TextFormat("Failed to load %s. (%s)",name,tex);
        const std::string keyname = (name == "") ? tex : name;
        textures[keyname] = texturex();
        textures[keyname].data = LoadImage(tex);

        if ( errorcheck( !IsImageReady(textures[keyname].data) , fail_message ) )
        {
            textures.erase(keyname);
            failed_textures.push_back(keyname);
            return 1;
        }
        else
        {
            loaded_files++;
            logprint(TextFormat("%s loaded as %s successfully.\n", tex, name));
            if ( define_alpha_color )
            {
                ImageFormat(&textures[keyname].data,7);
                ImageColorReplace(&textures[keyname].data,alpha_color,BLANK);
            }        
            return 0;
        }
    }

    int load_snd(const char* snd,const char* name = "")
    {
        files++; 
        target_file = snd; 
        const char* fail_message = TextFormat("Failed to load %s. (%s)",name,snd);
        const std::string keyname = (name == "") ? snd : name;
        sounds[keyname] = soundx();
        int err = sounds[keyname].data.load(snd);
        fail_message = TextFormat("Failed to load %s. (%s)\n%s",name,snd,soloud->getErrorString(err));
        if ( errorcheck( err , fail_message) )
        {
            sounds.erase(keyname);
            failed_sounds.push_back(keyname);
            return 1;
        }
        else
        {
            loaded_files++; //total succesfully loaded files
            logprint(TextFormat("%s loaded as %s successfully.\n", snd, name)); //print success
            return 0;
        }
    }
    
    void load_asset( fs::path file_path )
    {
        std::string file_path_string = file_path.string();
        std::string file_name = file_path.filename().replace_extension("").string();
        fs::path file_extension = file_path.extension();

        if ( file_extension == ".png" || file_extension == ".bmp") 
            load_tex(file_path_string.c_str(),file_name.c_str());
        else if ( file_extension == ".wav" || file_extension == ".ogg") 
            load_snd(file_path_string.c_str(),file_name.c_str());
        else if ( file_extension == ".drt" || file_extension == ".dirtscript" )
            printf("not right now");
        else if ( file_extension == ".map" || file_extension == ".dirtmap")
            printf("not right now");
    }

    void load_directory( fs::path path )
    {
        std::string error = "Attempted to load invalid directory (" + path.string() + ")";
        if ( fs::is_regular_file(path) ) 
        {
            load_asset(path);
            return;
        }
        if ( errorcheck( !fs::is_directory(path) , error.c_str())) return;
        for ( const auto& dir_entry : fs::recursive_directory_iterator(path) )
        {
            if ( true_exit ) return;
            if ( dir_entry.is_regular_file() ) load_asset(dir_entry);
        }
    }

    void get_files_status()
    {
        logprint(TextFormat("%i out of %i attempted files succesfully loaded.\n",loaded_files,files));
    }

    void push_texture(texturex * tex,bool unload_image = false)
    {
        tex->texture = LoadTextureFromImage(tex->data);
        if ( unload_image )
        {
            UnloadImage(tex->data);          
            tex->hasdata = false;  
        }
        tex->hastexture = true;
    }

    void push_textures()
    {
        logprint("pushing textures\n");
        for (const auto&i:failed_textures)
        {
            std::cout << i << " failed" << endl;
        }

        for ( auto &i:textures ) push_texture(&i.second);
    }

    void free_cpu_textures()
    {
        logprint("freeing cpu textures\n");
        for ( auto &i:textures ) UnloadImage(i.second.data);
    }

    bool tex_exists(std::string tex)
    {
        auto t = textures.find(tex);
        return ( t != textures.end() );
    }

    texturex * get_tex_full(std::string tex)
    {
        auto t = textures.find(tex);
        if ( t != textures.end() )
            return &t->second;
        return &missing_texture;
    }

    Texture2D get_tex(std::string tex)
    {
        return get_tex_full(tex)->texture;
    }

    void reload_texture(std::string tex)
    {
        if ( !tex_exists(tex)  ) return;
        texturex * texture = get_tex_full(tex);
        if ( !texture->hasdata ) return;

        if (texture->hastexture) UnloadTexture(texture->texture);
        texture->texture = LoadTextureFromImage(texture->data);     
    }
    
    Texture2D dynamic_get_tex(std::string &tex) // you have to use filepath
    {
        std::string fname = get_file_name(tex);
        if ( asset_failed(fname,failed_textures) ) return missing_texture.texture;
        if (!tex_exists(fname)) 
        {
            load_tex(tex.c_str(),fname.c_str());
            push_textures();
        }
        return get_tex(fname);
    }

    bool snd_exists(std::string snd)
    {
        auto t = sounds.find(snd);
        return ( t != sounds.end() );
    }

    SoLoud::Wav* get_snd(std::string snd)
    {
        auto s = sounds.find(snd);
        if (s != sounds.end())
            return &s->second.data;
        return &missing_sound;
    }

    SoLoud::Wav* dynamic_get_snd(std::string &snd)
    {
        std::string fname = get_file_name(snd);
        if ( asset_failed(fname,failed_sounds) ) return &missing_sound;
        if (!snd_exists(fname))
        {
            load_snd(snd.c_str(),fname.c_str());
        }
        return get_snd(fname);
    }

    int play(std::string snd,float volume = -1.0f, float pitch = 1.0f,float pan = 0.0f, SoLoud::Bus* bus = nullptr)
    {
        SoLoud::Wav* sound = get_snd(snd);

        int handle;
        if ( bus == nullptr )
            handle = soloud->play(*sound,volume,pan);
        else    
            handle = bus->play(*sound,volume,pan);
        
        soloud->setRelativePlaySpeed(handle,pitch);
        
        return handle;
    }

    void cleanup()
    {
        logprint("Cleaning up Assets.\n");
        for ( auto &i:textures )
        {
            if (i.second.hastexture) UnloadTexture(i.second.texture);
            if (i.second.hasdata) UnloadImage(i.second.data);
        } 
        UnloadTexture(missing_texture.texture);
        soloud->deinit();
    }
};
