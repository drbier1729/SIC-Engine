/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: - Andrew Rudascis
 */
 

#include "ResourceManager.h"

#include <SDL_surface.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <fmod_errors.h>
#include <fmod_output.h>
#include <filereadstream.h> // rapidjson
#include <filewritestream.h> // rapidjson
#include <prettywriter.h> // rapidjson
#include "StringId.h"
#include "System.h"
#include "AudioManager.h"

#include <ft2build.h> // text rendering
#include FT_FREETYPE_H




namespace sic
{
	ResourceManager::ResourceManager() : ResourceManager(allocator_type())
	{

	}

	ResourceManager::ResourceManager(allocator_type alloc) 
		: m_textures(alloc), m_shaders(alloc), m_sounds(alloc), m_json_docs(alloc), m_filepaths(alloc), m_characters(alloc), m_runtime_strings(alloc),
		m_raw_buffer(), m_file_read_buffer(m_raw_buffer.data(), gm_buffer_size), m_box{}, m_arrow{}, m_font{}
	{
		LoadCharacters();
	}

	ResourceManager::~ResourceManager()
	{
		m_sounds.clear();
		m_json_docs.clear();
		m_shaders.clear();
		m_textures.clear();
		m_filepaths.clear();
		m_characters.clear();
		m_runtime_strings.clear();
	}

	/// <summary>
	/// Loads a texture from a .tga file type
	/// </summary>
	/// <param name="file_path"></param>
	/// <returns>GL identifier for texture</returns>
	GLuint ResourceManager::LoadTGAImage(const char* file_path)
	{
		auto file_path_id = FilepathToSID(file_path);

		auto element = m_textures.find(file_path_id);
		if (element != m_textures.end())
		{
			return element->second;
		}

		// Verify file extension
		i64 idx = -1;
		char file_extension[32];

		auto length = strlen(file_path);

		for (auto i = 0ull; i < length; i++)
		{
			if (file_path[i] == '.')
			{
				idx = i;
			}
		}

		if (idx == -1)
		{
			return 0;
		}
		else 
		{
			strcpy_s(file_extension, file_path + idx + 1);
		}

		if (strcmp(file_extension, "tga") != 0)
		{
			std::cout << "Invalid file extension \"." << file_extension << "\" for this function" << std::endl;
			return 0;
		}

		GLuint texture_id = 0;
		SDL_Surface* image = nullptr;
		SDL_RWops* rwop;
		rwop = SDL_RWFromFile(file_path, "rb");
		image = IMG_LoadTGA_RW(rwop);
		SDL_RWclose(rwop);

		if (!image)
		{
			std::cerr << "Could not load asset " << file_path << std::endl;
			return texture_id;
		}

		std::cout << "Loaded " << file_path << std::endl;

		// Convert SDL_Surface to texture
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int32_t color_mode = GL_RGB;

		if (image->format->BytesPerPixel == 4)
		{
			color_mode = GL_RGBA;
		}

		// Load texture into VRAM
		glTexImage2D(GL_TEXTURE_2D, 0, color_mode, image->w, image->h, 0, color_mode, GL_UNSIGNED_BYTE, image->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Add to loaded resources
		m_textures.insert_or_assign(file_path_id, texture_id);

		// Free image from CPU RAM
		SDL_FreeSurface(image);

		return texture_id;
	}

	GLuint ResourceManager::LoadPNGImage(const char* file_path)
	{
		auto file_path_id = FilepathToSID(file_path);

		auto element = m_textures.find(file_path_id);
		if (element != m_textures.end())
		{
			return element->second;
		}

		// Verify File Extension
		i64 idx = -1;
		char file_extension[32];

		i64 length = strlen(file_path);

		for (i64 i = length - 1; i >= 0; i--)
		{
			if (file_path[i] == '.')
			{
				idx = i;
				break;
			}
		}

		if (idx == -1)
		{
			return 0;
		}
		else
		{
			strcpy_s(file_extension, file_path + idx + 1);
		}

		if (strcmp(file_extension, "png") != 0 && strcmp(file_extension, "PNG") != 0)
		{
			std::cout << "Invalid file extension \"." << file_extension << "\" for this function" << std::endl;
			return 0;
		}

		GLuint texture_id = 0;

		//Load image at specified path
		SDL_Surface* image = IMG_Load(file_path);
		if (image == NULL)
		{
			std::cout << "Unable to load image" << file_path << "\nSDL_image Error : " << IMG_GetError() << std::endl;
			return 0;
		}

		// TODO : sorry this is hacked in here... ridiculous way to do this!
		if (file_path_id == ".\\Assets\\Sprites\\lightbulb.png"_sid)
		{
			SDL_SetWindowIcon(System::Window(), image);
		}

		std::cout << "Loaded image" << file_path << std::endl;

		// Convert SDL_Surface to texture
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int32_t color_mode = GL_RGB;

		if (image->format->BytesPerPixel == 4)
		{
			color_mode = GL_RGBA;
		}

		// Load texture into VRAM
		glTexImage2D(GL_TEXTURE_2D, 0, color_mode, image->w, image->h, 0, color_mode, GL_UNSIGNED_BYTE, image->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Add to loaded resources
		m_textures.insert_or_assign(file_path_id, texture_id);

		// Free image from CPU RAM
		SDL_FreeSurface(image);

		return texture_id;
	}

	/// <summary>
	///		Loads a shader program using vertex and fragment shader source files at provided paths
	/// </summary>
	/// <param name="vertex_shader_path">File path to vertex shader source (.vert)</param>
	/// <param name="fragment_shader_path">File path to fragment shader source (.frag)</param>
	/// <returns>GLUint identifier pointing to compiled shader program</returns>
	GLuint ResourceManager::LoadShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path)
	{
		// TODO: when we add serialization, change arguements to take a
		//		config file so we can build shader program more dynamically

		auto shader_id = FilepathToSID(vertex_shader_path);

		auto element = m_shaders.find(shader_id);
		if (element != m_shaders.end())
		{
			return element->second;
		}

		GLuint gProgramID = glCreateProgram();
		GLint status;

		GLuint vertexShaderID = ReadAndCompileShaderFile(vertex_shader_path, GL_VERTEX_SHADER);
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
		if (status == GL_TRUE)
		{
			printf("Vertex Shader: %s OK\n", vertex_shader_path);
			glAttachShader(gProgramID, vertexShaderID);
		}

		GLuint fragmentShaderID = ReadAndCompileShaderFile(fragment_shader_path, GL_FRAGMENT_SHADER);
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);
		if (status == GL_TRUE)
		{
			printf("Fragment Shader: %s OK\n", fragment_shader_path);
			glAttachShader(gProgramID, fragmentShaderID);
		}

		glBindAttribLocation(gProgramID, 0, "vertex");
		glBindAttribLocation(gProgramID, 1, "vertexNormal");

		glLinkProgram(gProgramID);
		glUseProgram(gProgramID);

		m_shaders.insert_or_assign(shader_id, gProgramID);

		return gProgramID;
	}

	rapidjson::Document& ResourceManager::LoadJson(const char* file_name)
	{
		auto file_name_id = FilepathToSID(file_name);

		auto element = m_json_docs.find(file_name_id);
		if (element != m_json_docs.end())
		{
			return element->second;
		}

		
		auto [it, b] = m_json_docs.emplace(file_name_id, rapidjson::Document{});

		JsonToDocument(file_name, it->second);
		assert(it->second.IsObject());

		return it->second;
	}

	void ResourceManager::ReloadAllJson(void)
	{
		for (auto&& [sid, doc] : m_json_docs)
		{
			doc.RemoveAllMembers();
			JsonToDocument(m_filepaths.at(sid), doc);
		}
	}

	bool ResourceManager::ReloadJsonFromDisk(StringId::underlying_type file_name_sid)
	{
		auto it = m_json_docs.find(file_name_sid);
		if (it != m_json_docs.end())
		{
			it->second.RemoveAllMembers();
			JsonToDocument(m_filepaths.at(file_name_sid), it->second);
			return true;
		}
		return false;
	}

	bool ResourceManager::WriteJsonToDisk(StringId::underlying_type file_name_sid)
	{
		auto doc_itr = m_json_docs.find(file_name_sid);
		assert(doc_itr != m_json_docs.end());
		
		auto path_itr = m_filepaths.find(file_name_sid);
		assert(path_itr != m_filepaths.end());
			
		FILE* fp{ nullptr };
		fopen_s(&fp, path_itr->second, "w");
		assert("File failed to open." && fp);

		char* buffer = static_cast<char*>(m_file_read_buffer.allocate(gm_buffer_size));
		assert("Buffer alloc failed." && buffer);

		rapidjson::FileWriteStream stream(fp, buffer, gm_buffer_size);
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);

		const auto result = doc_itr->second.Accept(writer);

		fclose(fp);

		m_file_read_buffer.Clear();

		return result;
	}
#pragma region Manifest Loading
	
	void ResourceManager::LoadTextureManifest(const char* file_path)
	{
		rapidjson::Document& doc = LoadJson(file_path);
		assert("Invalid JSON." && doc.IsObject());

		rapidjson::Value::ConstMemberIterator doc_itr;
		rapidjson::Value::ConstMemberIterator aud_itr;

		doc_itr = doc.FindMember("Files");
		assert(doc_itr->value.IsArray());

		rapidjson::Value const& aud = doc_itr->value.GetArray();
		for (rapidjson::Value::ConstValueIterator itr = aud.Begin(); itr != aud.End(); ++itr)
		{
			rapidjson::Value const& config = itr->GetObject();
			rapidjson::Value::ConstMemberIterator obj = config.FindMember("File");
			const char* path = obj->value.GetString();

			LoadPNGImage(path);
		}
	}

	void ResourceManager::LoadShaderManifest(const char* file_path)
	{
		rapidjson::Document& doc = LoadJson(file_path);
		assert("Invalid JSON." && doc.IsObject());

		rapidjson::Value::ConstMemberIterator doc_itr;
		rapidjson::Value::ConstMemberIterator aud_itr;

		doc_itr = doc.FindMember("Files");
		assert(doc_itr->value.IsArray());

		rapidjson::Value const& aud = doc_itr->value.GetArray();
		for (rapidjson::Value::ConstValueIterator itr = aud.Begin(); itr != aud.End(); ++itr)
		{
			rapidjson::Value const& config = itr->GetObject();
			rapidjson::Value::ConstMemberIterator obj = config.FindMember("Vertex");
			const char* vert_path = obj->value.GetString();
			obj = config.FindMember("Fragment");
			const char* frag_path = obj->value.GetString();

			LoadShaderProgram(vert_path, frag_path);
		}
	}

	void ResourceManager::LoadJsonManifest(const char* file_path)
	{
		rapidjson::Document& doc = LoadJson(file_path);
		assert("Invalid JSON." && doc.IsObject());

		rapidjson::Value::ConstMemberIterator doc_itr;
		rapidjson::Value::ConstMemberIterator aud_itr;

		doc_itr = doc.FindMember("Files");
		assert(doc_itr->value.IsArray());

		rapidjson::Value const& aud = doc_itr->value.GetArray();
		for (rapidjson::Value::ConstValueIterator itr = aud.Begin(); itr != aud.End(); ++itr)
		{
			rapidjson::Value const& config = itr->GetObject();
			rapidjson::Value::ConstMemberIterator obj = config.FindMember("File");
			const char* path = obj->value.GetString();
			
			LoadJson(path);
		}
	}

	void ResourceManager::LoadAudioManifest(const char* file_path)
	{
		rapidjson::Document& doc = LoadJson(file_path);
		assert("Invalid JSON." && doc.IsObject());

		rapidjson::Value::ConstMemberIterator doc_itr;
		rapidjson::Value::ConstMemberIterator aud_itr;

		doc_itr = doc.FindMember("Files");
		assert(doc_itr->value.IsArray());

		rapidjson::Value const& aud = doc_itr->value.GetArray();
		for (rapidjson::Value::ConstValueIterator itr = aud.Begin(); itr != aud.End(); ++itr)
		{
			rapidjson::Value const& config = itr->GetObject();
			rapidjson::Value::ConstMemberIterator obj = config.FindMember("File");
			const char* path = obj->value.GetString();

			obj = config.FindMember("Type");
			bool is_stream = obj->value.GetBool();

			if (!is_stream)
			{
				LoadAudioFile(System::Audio().SoundSystem(), path);
			}
			else
			{
				LoadAudioStream(System::Audio().SoundSystem(), path);
			}
		}

	}
#pragma endregion

#pragma region Strings and Characters
	void ResourceManager::LoadCharacters(void)
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			assert("ERROR::FREETYPE: Could not init FreeType Library");
		}

		FT_Face face;
		if (FT_New_Face(ft, ".\\Assets\\Fonts\\Abel-Regular.ttf", 0, &face))
		{
			assert("ERROR::FREETYPE: Failed to load font");
		}
		else 
		{
			FT_Set_Pixel_Sizes(face, 0, 48);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

			for (unsigned char c = 0; c < 128; c++)
			{
				// load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					assert("ERROR::FREETYTPE: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				m_characters.insert_or_assign(c, character);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// destroy freetype
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void ResourceManager::LoadRuntimeString(const char* string)
	{
		StringId sid{ string };
		
		auto it = m_runtime_strings.find(sid.Id());
		if (it == m_runtime_strings.end())
		{
			m_runtime_strings.emplace(sid.Id(), string);
		}
	}
	
	const char* ResourceManager::GetString(StringId::underlying_type text_sid) const
	{
		auto it = m_runtime_strings.find(text_sid);
		if (it != m_runtime_strings.end())
		{
			return it->second;
		}
		return nullptr;
	}

	const char* ResourceManager::GetFilepath(StringId::underlying_type sid) const
	{
		auto it = m_filepaths.find(sid);
		if (it != m_filepaths.end())
		{
			return it->second;
		}
		return nullptr;
	}
#pragma endregion

#pragma region FMOD setup
	
	FMOD::Sound* ResourceManager::LoadAudioFile(FMOD::System& m_sound_system, const char* file_name)
	{
		StringId sound_id(file_name);
		
		auto element = m_sounds.find(sound_id.Id());
		if (element != m_sounds.end())
		{
			return element->second;
		}

		FMOD::Sound* sound;

		FMOD_RESULT result = m_sound_system.createSound(file_name, FMOD_DEFAULT, 0, &sound);
		assert(result == FMOD_OK);

		m_sounds.insert_or_assign(sound_id.Id(), sound);

		return sound;
	}

	FMOD::Sound* ResourceManager::LoadAudioStream(FMOD::System& m_sound_system, const char* file_name)
	{
		StringId sound_id(file_name);

		auto element = m_sounds.find(sound_id.Id());
		if (element != m_sounds.end())
		{
			return element->second;
		}

		FMOD::Sound* sound;

		FMOD_RESULT result = m_sound_system.createStream(file_name, FMOD_DEFAULT, 0, &sound);
		assert(result == FMOD_OK);

		m_sounds.insert_or_assign(sound_id.Id(), sound);

		return sound;
	}

	FMOD::Sound* ResourceManager::GetSound(StringId::underlying_type hash)
	{
		auto element = m_sounds.find(hash);
		if (element != m_sounds.end())
		{
			return element->second;
		}

		return NULL;
	}

#pragma endregion

#pragma region Helpers

	// Helpers
	GLuint ResourceManager::ReadAndCompileShaderFile(const char* name, GLint GL_shader_macro)
	{
		std::ifstream file_stream;
		file_stream.open(name, std::ios_base::binary);	// Open
		assert("File failed to open." && file_stream);

		file_stream.seekg(0, std::ios_base::end);		// Position at end
		size_t length = file_stream.tellg();			//   to get the length

		char* content = static_cast<char*>(m_file_read_buffer.allocate(length + 1ull)); // Create buffer of needed length
		assert("Buffer alloc failed." && content);

		file_stream.seekg(0, std::ios_base::beg);		// Position at beginning
		file_stream.read(content, length);				//   to read complete file
		file_stream.close();							// Close

		content[length] = char(0);						// Finish with a NULL

		GLuint shaderID = glCreateShader(GL_shader_macro);
		glShaderSource(shaderID, 1, &content, NULL);
		glCompileShader(shaderID);

		m_file_read_buffer.Clear();

		return shaderID;
	}

	void ResourceManager::JsonToDocument(const char* filePath, rapidjson::Document& d)
	{
		FILE* fp{ nullptr };
		fopen_s(&fp, filePath, "r");
		assert("File failed to open." && fp);

		// Make sure file size is less than buffer size

		char* readBuffer = static_cast<char*>(m_file_read_buffer.allocate(gm_buffer_size));
		assert("Buffer alloc failed." && readBuffer);

		rapidjson::FileReadStream is(fp, readBuffer, gm_buffer_size);

		d.ParseStream(is);

		fclose(fp);

		m_file_read_buffer.Clear();
	}

	StringId::underlying_type ResourceManager::FilepathToSID(const char* filePath)
	{
		StringId file_name_id{ filePath };
		auto element = m_filepaths.find(file_name_id.Id());
		if (element != m_filepaths.end())
		{
			return file_name_id.Id();
		}


		auto [it, b] = m_filepaths.emplace(file_name_id.Id(), filePath);

		return file_name_id.Id();
	}

#pragma endregion
}