/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Dylan Bier
 * Additional Authors: - Andrew Rudasics, Aseem Apastamb
 */
 

#include "ComponentSerialization.h"

#include "Entities.h"

#include "ResourceManager.h"
#include "System.h"

#include "externals/World.h"

// thread safe random
#include <random>


namespace sic {

	template<>
	void Deserialize<Body>(rapidjson::Value const& val, Entity const& e)
	{
		Body& comp = DeserializeReflectable<Body>(val, e);

		// Calculate inverse mass, impulse, inverse impulse		
		comp.CalculateImpulse();

		if (e.Has<Transform>())
		{
			System::Physics().Add(e);
		}
		else
		{
			System::Physics().Add(&comp);
		}
	}


	template<>
	void Serialize<Joint>(rapidjson::Value& val, Joint const& jnt, rapidjson::MemoryPoolAllocator<>& alloc)
	{
		SerializeReflectable(val, jnt, alloc);

		rapidjson::Value body1;
		rapidjson::Value body2;

		body1.Set(NullEntityId);
		body2.Set(NullEntityId);

		// write entity ids
		body1.Set(jnt.body1->owner);
		body2.Set(jnt.body2->owner);

		auto itr = val.FindMember("entity1");
		if (itr == val.MemberEnd())
		{
			rapidjson::Value key;
			key.SetString("entity1", alloc);
			val.AddMember(key, body1, alloc);
		}
		else {
			itr->value = body1;
		}
		
		itr = val.FindMember("entity2");
		if (itr == val.MemberEnd())
		{
			rapidjson::Value key;
			key.SetString("entity2", alloc);
			val.AddMember(key, body2, alloc);
		}
		else {
			itr->value = body2;
		}
	}


	template<>
	void Deserialize<Joint>(rapidjson::Value const& val, Entity const& e)
	{
		Joint& comp = DeserializeReflectable<Joint>(val, e);


		// TODO : how should we "connect" two Body components using a joint? do we need to look them up from EntityID?
		// What if that entity hasn't been created yet? We'd need to go find it in the JSON, build it, then continue
		// deserializing this component.
	}


	template<>
	void Serialize<SpriteRenderer>(rapidjson::Value& val, SpriteRenderer const& sr, rapidjson::MemoryPoolAllocator<>& alloc)
	{
		SerializeReflectable(val, sr, alloc);

		// Write Material:: tile_x, tile_y, row, col
		rapidjson::Value tiles;
		rapidjson::Value rows_cols;

		rapidjson::Value::MemberIterator sr_itr;

		tiles.SetArray()
			.Reserve(2, alloc)
			.PushBack(sr.material.GetX(), alloc)
			.PushBack(sr.material.GetY(), alloc);

		rows_cols.SetArray()
			.Reserve(2, alloc)
			.PushBack(sr.material.GetRows(), alloc)
			.PushBack(sr.material.GetCols(), alloc);

		sr_itr = val.FindMember("tile");
		if (sr_itr == val.MemberEnd()) 
		{
			rapidjson::Value key;
			key.SetString("tile", alloc);
			val.AddMember(key, tiles, alloc);
		}
		else {
			sr_itr->value = tiles;
		}
		
		sr_itr = val.FindMember("spritesheetDimension");
		if (sr_itr == val.MemberEnd())
		{
			rapidjson::Value key;
			key.SetString("spritesheetDimension", alloc);
			val.AddMember(key, rows_cols, alloc);
		}
		else {
			sr_itr->value = rows_cols;
		}
	}


	template<>
	void Serialize<SpriteAnimator>(rapidjson::Value& val, SpriteAnimator const& sa, rapidjson::MemoryPoolAllocator<>& alloc)
	{
		//SerializeReflectable(val, sa, alloc);

		// Write Material:: tile_x, tile_y, row, col
		rapidjson::Value frames;
		rapidjson::Value clips;

		

		rapidjson::Value::MemberIterator sr_itr;

		frames.SetArray().Reserve(64, alloc);
		for (int i = 0; i < 64; i++)
		{
			frames.GetArray().PushBack(sa.frames[i], alloc);
		}

		rapidjson::Value key;
		key.SetString("frames", alloc);
		val.AddMember(key, frames, alloc);

		clips.SetArray().Reserve(16, alloc);
		for (int i = 0; i < 16; i++)
		{
			rapidjson::Value obj(rapidjson::kObjectType);
			obj.SetObject();
			obj.AddMember("name", sa.clips[i].state, alloc);
			obj.AddMember("start", sa.clips[i].start_idx, alloc);
			obj.AddMember("end", sa.clips[i].end_idx, alloc);
			clips.GetArray().PushBack(obj, alloc);
		}

		key.SetString("clips", alloc);
		val.AddMember(key, clips, alloc);
	}


	template<>
	void Deserialize<SpriteRenderer>(rapidjson::Value const& val, Entity const& e)
	{
		SpriteRenderer& comp = DeserializeReflectable<SpriteRenderer>(val, e);

		auto end = val.MemberEnd();

		float tile_x, tile_y;
		tile_x = tile_y = 1.0f;

		auto mat_itr = val.FindMember("tile");
		if (mat_itr != end)
		{
			assert(mat_itr->value.IsArray());
			assert(mat_itr->value.Size() == 2);
			rapidjson::Value const& tile = mat_itr->value.GetArray();

			tile_x = tile[0].GetFloat();
			tile_y = tile[1].GetFloat();
		}

		int rows, cols;
		rows = cols = 1;

		mat_itr = val.FindMember("spritesheetDimension");
		if (mat_itr != end)
		{
			assert(mat_itr->value.IsArray());
			assert(mat_itr->value.Size() == 2);
			rapidjson::Value const& sd = mat_itr->value.GetArray();

			rows = sd[0].GetInt();
			cols = sd[1].GetInt();
		}

		// TODO : we should deserialize the Sprite's material from an array of vertex 
		// shaders (strings), an array of frag shaders (strings), and a filepath for a 
		// texture (string).

		// TODO : these should be loaded already! This should be a call to a method like:
		// ResourceManager::GetMaterial(StringId::underlying_type)

		auto texture_path = System::Resources().GetFilepath(comp.texture_file.Id());

		GLuint tex;
		if (texture_path != nullptr)
		{
			tex = System::Resources().LoadPNGImage(texture_path);
		}
		else
		{
			tex = System::Resources().LoadPNGImage(".\\Assets\\Sprites\\default_texture.png");
		}


		comp.material = Material{
			tex,
			System::Resources().LoadShaderProgram("final.vert", "final.frag"),
			tile_x,
			tile_y,
			rows,
			cols
		};

		// TODO : we should deserialize the Sprite's mesh from ... ?? Maybe a name (string)
		// and array of vertices?
		comp.shape = System::Resources().GetBox2D();
	}

	template<>
	void Deserialize<SpriteAnimator>(rapidjson::Value const& val , Entity const& e)
	{
		SpriteAnimator& comp{ AddIfNull<SpriteAnimator>(e) };

		auto end = val.MemberEnd();

		rapidjson::Value::ConstMemberIterator sa_itr;
		sa_itr = val.FindMember("frames");
		if (sa_itr != end)
		{
			assert(sa_itr->value.IsArray());
			rapidjson::Value const& frames = sa_itr->value.GetArray();
			for (rapidjson::SizeType i = 0; i < frames.Size(); i++)
			{
				comp.frames[i] = frames[i].GetInt();
			}
		}

		sa_itr = val.FindMember("clips");
		if (sa_itr != end)
		{
			assert(sa_itr->value.IsArray());
			rapidjson::Value const& clips = sa_itr->value.GetArray();
			for (rapidjson::SizeType i = 0; i < clips.Size(); i++)
			{
				rapidjson::Value const& clip = clips[i].GetObject();
				rapidjson::Value::ConstMemberIterator clip_itr;

				clip_itr = clip.FindMember("name");
				assert(clip_itr->value.IsInt());
				u32 name = clip_itr->value.GetInt();

				clip_itr = clip.FindMember("start");
				assert(clip_itr->value.IsInt());
				i32 start = clip_itr->value.GetInt();

				clip_itr = clip.FindMember("end");
				assert(clip_itr->value.IsInt());
				i32 end = clip_itr->value.GetInt();

				comp.clips[i] = AnimClip(name, start, end);
			}

		}
	}

	template<>
	void Deserialize<Patrol>(rapidjson::Value const& val, Entity const& e)
	{
		Patrol& comp{ DeserializeReflectable<Patrol>(val, e) };
		float probability = 0.5f;

		std::random_device device;
		std::mt19937_64 RNGen(device());
		std::uniform_real_distribution<> random(0.0, 1.0);

		comp.direction = random(RNGen) > probability ? vec3(1.0f, 0.0f, 0.0f) : vec3(-1.0f, 0.0f, 0.0f);
	}

	template<>
	void Deserialize<Text>(rapidjson::Value const& val, Entity const& e)
	{
		Text& comp{ DeserializeReflectable<Text>(val, e) };

		auto itr = val.FindMember("text");
		if (itr != val.MemberEnd())
		{
			assert(itr->value.IsString());
			System::Resources().LoadRuntimeString(itr->value.GetString());
		}
	}

}