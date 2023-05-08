/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Aseem Apastamb
 * Additional Authors: - Dylan Bier
 */
 

// This file is included in ComponentDeserializers.h

namespace sic {

	template<ValidComponent C>
	C& AddIfNull(Entity const& e)
	{
		if (e.Has<C>() == false)
		{
			return e.Add<C>();
		}
		return e.Get<C>();
	}

	template<ValidComponent C>
	rapidjson::Value& SerializeReflectable(rapidjson::Value& val, C const& comp, rapidjson::MemoryPoolAllocator<>& alloc) {

		Reflector::Class const* objectInfo = Reflector::GetClass<C>();

		rapidjson::Value::MemberIterator c_itr;

		for (auto const& field : objectInfo->fields) {
			if (field.type == nullptr) {
				break;
			}

			rapidjson::Value value;
			i8 const* source = reinterpret_cast<i8 const*>(&comp) + field.offset;

			switch (field.type->enumName) {
				case Reflector::TypeName::u8:
				case Reflector::TypeName::u16:
				case Reflector::TypeName::u32: {
					u32 destination = 0;
					memcpy(&destination, source, field.type->size);
					value.SetUint(destination);
					break;
				}
				case Reflector::TypeName::EntityID:
				case Reflector::TypeName::u64: {
					u64 destination = 0;
					memcpy(&destination, source, field.type->size);
					value.SetUint64(destination);
					break;
				}
				case Reflector::TypeName::i8:
				case Reflector::TypeName::i16:
				case Reflector::TypeName::i32: {
					i32 destination = 0;
					memcpy(&destination, source, field.type->size);
					value.SetInt(destination);
					break;
				}
				case Reflector::TypeName::i64: {
					i64 destination = 0;
					memcpy(&destination, source, field.type->size);
					value.SetInt64(destination);
					break;
				}
				case Reflector::TypeName::f32: {
					f32 destination = 0.0f;
					memcpy(&destination, source, field.type->size);
					value.SetFloat(destination);
					break;
				}
				case Reflector::TypeName::f64: {
					f64 destination = 0.0f;
					memcpy(&destination, source, field.type->size);
					value.SetDouble(destination);
					break;
				}
				case Reflector::TypeName::vec2: {
					vec2 destination = vec2(0);
					memcpy(&destination, source, field.type->size);
					value.SetArray()
						.Reserve(2, alloc)
						.PushBack(destination.x, alloc)
						.PushBack(destination.y, alloc);
					break;
				}
				case Reflector::TypeName::vec3: {
					vec3 destination = vec3(0);
					memcpy(&destination, source, field.type->size);
					value.SetArray()
						.Reserve(3, alloc)
						.PushBack(destination.x, alloc)
						.PushBack(destination.y, alloc)
						.PushBack(destination.z, alloc);
					break;
				}
				case Reflector::TypeName::vec4: {
					vec4 destination = vec4(0);
					memcpy(&destination, source, field.type->size);
					value.SetArray()
						.Reserve(4, alloc)
						.PushBack(destination.x, alloc)
						.PushBack(destination.y, alloc)
						.PushBack(destination.z, alloc)
						.PushBack(destination.w, alloc);
					break;
				}
				case Reflector::TypeName::StringId: {
					StringId destination;
					memcpy(&destination, source, field.type->size);
					#ifdef _DEBUG
					value.SetString(rapidjson::StringRef(destination.Str()), alloc);
					#else
					value.SetUint64(destination.Id());
					#endif
					break;
				}
				case Reflector::TypeName::bool8: {
					bool8 destination = false;
					memcpy(&destination, source, field.type->size);
					value.SetBool(destination);
					break;
				}
				default: {
					assert(false);
					break;
				}
			}

			c_itr = val.FindMember(field.name);
			if (c_itr == val.MemberEnd()) {
				rapidjson::Value key;

				key.SetString(field.name, alloc);

				val.AddMember(key, value, alloc);
			}
			else {
				c_itr->value = value;
			}
		}

		return val;
	}

	template<ValidComponent C>
	C& DeserializeReflectable(rapidjson::Value const& val, Entity const& e) {
	
		C& comp{ AddIfNull<C>(e) };

		Reflector::Class const* objectInfo = Reflector::GetClass<C>();

		for (auto const& field : objectInfo->fields) {
			if (field.type == nullptr) {
				break;
			}

			if (val.HasMember(field.name)) {
				i8* destination = reinterpret_cast<i8*>(&comp) + field.offset;
				switch (field.type->enumName) {
					case Reflector::TypeName::u8:
					case Reflector::TypeName::u16:
					case Reflector::TypeName::u32: {
						u32 source = val[field.name].GetUint();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::EntityID:
					case Reflector::TypeName::u64: {
						u64 source = val[field.name].GetUint64();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::i8:
					case Reflector::TypeName::i16:
					case Reflector::TypeName::i32: {
						i32 source = val[field.name].GetInt();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::i64: {
						i64 source = val[field.name].GetInt64();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::f32: {
						f32 source = val[field.name].GetFloat();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::f64: {
						f64 source = val[field.name].GetDouble();
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::vec2:
					case Reflector::TypeName::vec3:
					case Reflector::TypeName::vec4: {
						rapidjson::Value const& source = val[field.name].GetArray();
						assert(source.IsArray());
						for (rapidjson::SizeType i = 0; i < source.Size(); ++i) {
							f32 f = source[i].GetFloat();
							memcpy(destination + (i * sizeof(f32)), &f, sizeof(f32));
						}
						break;
					}
					case Reflector::TypeName::StringId: {
						rapidjson::Value const& sid = val[field.name];
						StringId source = sid.IsString() ? StringId{ sid.GetString() } : StringId{ sid.GetUint64() };
						memcpy(destination, &source, field.type->size);
						break;
					}
					case Reflector::TypeName::bool8: {
						bool8 source = val[field.name].GetBool();
						memcpy(destination, &source, field.type->size);
						break;
					}
					default: {
						assert(false);
						break;
					}
				}
			}
		}

		return comp;
	}


	template<ValidComponent C>
	void Serialize(rapidjson::Value& val, C const& comp, rapidjson::MemoryPoolAllocator<>& alloc)
	{
		SerializeReflectable<C>(val, comp, alloc);
	}

	template<ValidComponent C>
	void Deserialize(rapidjson::Value const& val, Entity const& e)
	{
		DeserializeReflectable<C>(val, e);
	}


}