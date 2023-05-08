/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: - Andrew Rudasics, Brian Chen
 */
 

#pragma once

#include <unordered_map>
#include <array>
#include <map>

#include <glew.h>
#include <document.h> //rapidjson
#include "fmod.hpp"

#include "StringId.h"
#include "SICTypes.h"
#include "MemResources.h"

#include "Shape.h"

namespace sic
{
	class ResourceManager
	{
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;
		struct Character {
			unsigned int TextureID;  // ID handle of the glyph texture
			glm::ivec2   Size;       // Size of glyph
			glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
			unsigned int Advance;    // Offset to advance to next glyph
		};

	public:
		ResourceManager();
		explicit ResourceManager(allocator_type alloc);
		ResourceManager(ResourceManager const& src, allocator_type alloc = {}) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager(ResourceManager&&, allocator_type alloc) = delete;
		ResourceManager& operator=(ResourceManager const& rhs) = delete;
		ResourceManager& operator=(ResourceManager&& rhs) = delete;

		~ResourceManager();

	public:
		// Asset loading
		GLuint LoadTGAImage(const char* file_path);
		GLuint LoadPNGImage(const char* file_path);
		GLuint LoadShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
		FMOD::Sound* LoadAudioFile(FMOD::System& fmod_sys, const char* file_name);
		FMOD::Sound* LoadAudioStream(FMOD::System& fmod_sys, const char* file_name);
		
		rapidjson::Document& LoadJson(const char* file_name);
		void LoadAudioManifest(const char* file_path);
		void LoadTextureManifest(const char* file_path);
		void LoadShaderManifest(const char* file_path);
		void LoadJsonManifest(const char* file_path);

		void ReloadAllJson(void);
		bool ReloadJsonFromDisk(StringId::underlying_type file_name_sid);
		bool WriteJsonToDisk(StringId::underlying_type file_name_sid);

		void LoadCharacters(void);
		void LoadRuntimeString(const char* string);

		// Accessors
		FMOD::Sound* GetSound(StringId::underlying_type hash);

		inline
		Shape* GetBox2D(void) { return &m_box; }

		inline
		Shape* GetArrow2D(void) { return &m_arrow; }

		inline
		Font* GetFont(void) { return &m_font; }

		inline
		std::pmr::unordered_map<char, Character>* GetCharMap(void) { return &m_characters; }

		const char* GetString(StringId::underlying_type text_sid) const;
		
		const char* GetFilepath(StringId::underlying_type sid) const;

		inline
		bool ValidFilepath(StringId::underlying_type sid) { return m_filepaths.find(sid) != m_filepaths.end(); }

		inline 
		rapidjson::Document& GetJson(StringId::underlying_type file_name_sid) { return m_json_docs.at(file_name_sid); }
		
		inline
		rapidjson::Document& GetJson(StringId file_name_sid) { return m_json_docs.at(file_name_sid.Id()); }

		inline
		rapidjson::Document& GetJson(const char* file_name) { StringId sid{ file_name };  return m_json_docs.at(sid.Id()); }

	private:
		GLuint ReadAndCompileShaderFile(const char* name, GLint GL_shader_macro);
		void JsonToDocument(const char* filePath, rapidjson::Document& d);
		StringId::underlying_type FilepathToSID(const char* filePath);

	private:
		std::pmr::unordered_map<StringId::underlying_type, GLuint> m_textures;
		std::pmr::unordered_map<StringId::underlying_type, GLuint>  m_shaders;
		std::pmr::unordered_map<StringId::underlying_type, FMOD::Sound*> m_sounds;
		std::pmr::unordered_map<StringId::underlying_type, rapidjson::Document> m_json_docs;

		std::pmr::unordered_map<StringId::underlying_type, const char*> m_filepaths;

		std::pmr::unordered_map<char, Character> m_characters;
		std::pmr::unordered_map<StringId::underlying_type, const char*> m_runtime_strings;

		Box2D m_box;
		Arrow2D m_arrow;
		Font m_font;


		// Temp storage for reading in data
		class_global constexpr size_t gm_buffer_size{ Kilobytes(64) };
		std::array<char, gm_buffer_size> m_raw_buffer;
		LinearMemResource m_file_read_buffer;
	};
}
