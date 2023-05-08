//#include "PhysicsManager.h"
//
//#include "System.h"
//#include "Components.h"
//#include "Events.h"
//#include "EventManager.h"
//
//namespace sic {
//
//	// This functions checks if 2 (axis-aligned) boxes are colliding
//	PhysicsManager::CollisionInfo PhysicsManager::AABBtoAABB(vec3 const& Center0, f32 rx0, f32 ry0, f32 rz0,
//															 vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1)
//	{
//		CollisionInfo ci{};
//
//		vec3 dist{ Center1 - Center0 };
//
//		// Calculate overlap on all axes
//		f32 const x_overlap = rx1 + rx0 - abs(dist.x);
//		f32 const y_overlap = ry1 + ry0 - abs(dist.y);
//		f32 const z_overlap = rz1 + rz0 - abs(dist.z);
//
//		// SAT tests
//		if (x_overlap > 0.0f && y_overlap > 0.0f && z_overlap > 0.0f)
//		{
//			ci.collision = true;
//
//			// Find out which axis is axis of least penetration
//			f32 const min_overlap{ std::min({x_overlap, y_overlap, z_overlap}) };
//
//			if (x_overlap == min_overlap)
//			{
//				if (dist.x < 0.0f) { ci.normal = compass[1]; }// Left
//				else { ci.normal = compass[0]; }// Right
//
//				ci.penetrationDepth = x_overlap;
//			}
//			else if (y_overlap == min_overlap)
//			{
//				if (dist.y < 0.0f) { ci.normal = compass[3]; }// Down
//				else { ci.normal = compass[2]; }// Up
//
//				ci.penetrationDepth = y_overlap;
//			}
//			else
//			{
//				if (dist.z < 0.0f) { ci.normal = compass[5]; }// Back
//				else { ci.normal = compass[4]; }// Front
//
//				ci.penetrationDepth = z_overlap;
//			}
//		}
//
//		return ci;
//	}
//
//	// This functions checks if 2 circles are colliding
//	PhysicsManager::CollisionInfo PhysicsManager::CircleToCircle(vec3 const& Center0, f32 Radius0,
//																 vec3 const& Center1, f32 Radius1)
//	{
//		CollisionInfo ci{};
//
//		float totalR = Radius0 + Radius1;
//		totalR *= totalR;
//
//		vec3 const diff = Center1 - Center0;
//
//		if (totalR > glm::dot(diff, diff)) {
//			
//			ci.collision = true;
//			ci.penetrationDepth = glm::abs(Radius0 - Radius1);
//			
//			// TODO : watch out for floating point weirdness here
//			if (diff != vec3(0.0f))
//			{
//				// Normalize requires nonzero input vector
//				ci.normal = glm::normalize(diff);
//			}
//			// else: ci.normal = vec3(0.0f)
//		}
//
//		return ci;
//	}
//
//	// This function checks if a Circle is colliding with an AABB and returns a CollisionInfo object
//	PhysicsManager::CollisionInfo PhysicsManager::CircleToAABB(vec3 const& Center0, f32 Radius0,
//															   vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1)
//	{
//		CollisionInfo ci{};
//
//		vec3 D{ Center0 - Center1 };
//		vec3 halfExtents{ rx1, ry1, rz1 };
//		vec3 clampedD{ glm::clamp(D, -halfExtents, halfExtents) };
//
//		vec3 closest{ Center1 + clampedD };
//		D = closest - Center0;
//
//		if (D != vec3(0.0f))
//		{
//			if (glm::dot(D, D) < Radius0 * Radius0) {
//				ci.collision = true;
//
//				ci.penetrationDepth = Radius0 - glm::length(D);
//
//				f32 max = 0.0f;
//				u32 bestMatch = 0;
//
//				for (u32 i = 0; i < 6; ++i) {
//					if (D != vec3(0.0f))
//					{
//						float dot = glm::dot(glm::normalize(D), compass[i]);
//						if (dot > max) {
//							max = dot;
//							bestMatch = i;
//						}
//					}
//				}
//
//				ci.normal = compass[bestMatch];
//			}
//		}
//		return ci;
//	}
//
//	bool8 PhysicsManager::GroundedCheck(vec3 const& Center0, f32 rx0, f32 rz0,
//										vec3 const& Center1, f32 rx1, f32 ry1, f32 rz1)
//	{
//		bool8 collision = false;
//
//		vec3 dist{ Center1 - Center0 };
//
//		// Calculate overlap on all axes
//		f32 const x_overlap = rx1 + rx0 - abs(dist.x);
//		f32 const y_overlap = ry1 - abs(dist.y);
//		f32 const z_overlap = rz1 + rz0 - abs(dist.z);
//
//		// SAT tests
//		if (x_overlap > 0.0f && y_overlap > 0.0f && z_overlap > 0.0f)
//		{
//			collision = true;
//		}
//
//		return collision;
//	}
//
//	// TODO (dylan/aseem) : this is the dumbest O(n^2) collision detection
//	void PhysicsManager::DetectAABBCollisions()
//	{
//		CollisionInfo ci{};
//
//		// With AABBs
//		auto& aabbVec = m_ecs->GetComponentArray<AABB>();
//
//		for (auto i = aabbVec.begin(); i != aabbVec.end(); ++i) {
//			for (auto j = std::next(i); j != aabbVec.end(); ++j) {
//
//				EntityID entity_a = m_ecs->GetOwnerID(*i);
//				EntityID entity_b = m_ecs->GetOwnerID(*j);
//
//				// Static-Static culling
//				if (m_ecs->HasComponent(StaticTag::id, entity_a) && m_ecs->HasComponent(StaticTag::id, entity_b)) {
//					break;
//				}
//				
//				ci = AABBtoAABB(i->center, i->rx, i->ry, i->rz,
//								j->center, j->rx, j->ry, j->rz);
//
//				CollidingWith* cw_a = nullptr;
//				CollidingWith* cw_b = nullptr;
//
//				i8 index_a = -1;
//				i8 index_b = -1;
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_a)) {
//					cw_a = m_ecs->GetComponent<CollidingWith>(entity_a);
//					for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//						if (cw_a->entity_ids[i] == entity_b) {
//							index_a = i;
//							break;
//						}
//					}
//				}
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_b)) {
//					cw_b = m_ecs->GetComponent<CollidingWith>(entity_b);
//					for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//						if (cw_b->entity_ids[i] == entity_a) {
//							index_b = i;
//							break;
//						}
//					}
//				}
//
//				if (ci.collision) {
//					assert(m_ecs->HasComponent(RigidBody::id, entity_a));
//					assert(m_ecs->HasComponent(RigidBody::id, entity_b));
//
//					ci.entity_ids[0] = entity_a;
//					ci.entity_ids[1] = entity_b;
//
//					bool8 collision_enter_flag = false;
//
//					if (cw_a != nullptr) {
//						if (index_a == -1) {
//							for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//								if (cw_a->entity_ids[i] == NullEntityId) {
//									cw_a->entity_ids[i] = entity_b;
//									cw_a->names[i] = m_ecs->GetComponent<Name>(entity_b)->name;
//
//									System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//										{"EntityID_primary", entity_a},
//										{"EntityID_secondary", entity_b}
//									});
//
//									collision_enter_flag = true;
//
//									break;
//								}
//							}
//						}
//					}
//
//					if (cw_b != nullptr) {
//						if (index_b == -1) {
//							for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//								if (cw_b->entity_ids[i] == NullEntityId) {
//									cw_b->entity_ids[i] = entity_a;
//									cw_b->names[i] = m_ecs->GetComponent<Name>(entity_a)->name;
//
//									if (!collision_enter_flag) {
//										System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//											{"EntityID_primary", entity_a},
//											{"EntityID_secondary", entity_b}
//										});
//									}
//
//									break;
//								}
//							}
//						}
//					}
//
//					// Enqueue collision
//					auto p_ci = reinterpret_cast<CollisionInfo*>(m_collisions_resource.allocate(sizeof(CollisionInfo), alignof(CollisionInfo)));
//					*p_ci = ci;
//				}
//				else if (index_a > -1 || index_b > -1) {
//					if (cw_a != nullptr) {
//						assert(index_a != -1);
//						cw_a->entity_ids[index_a] = NullEntityId;
//						cw_a->names[index_a] = "<NONE>"_sid;
//					}
//
//					if (cw_b != nullptr) {
//						assert(index_b != -1);
//						cw_b->entity_ids[index_b] = NullEntityId;
//						cw_b->names[index_b] = "<NONE>"_sid;
//					}
//
//					System::Events().EnqueueEvent(EventType::CollisionExit, 0.0f, {
//						{"EntityID_primary", entity_a},
//						{"EntityID_secondary", entity_b}
//					});
//				}
//			}
//		}
//	}
//
//	// TODO (dylan/aseem) : this is the dumbest O(n^2) collision detection
//	void PhysicsManager::DetectCircleCollisions()
//	{
//		CollisionInfo ci{};
//
//		// With Circles
//		auto& circleVec = m_ecs->GetComponentArray<Circle>();
//
//		for (auto i = circleVec.begin(); i != circleVec.end(); ++i) {
//			for (auto j = std::next(i); j != circleVec.end(); ++j) {
//
//				EntityID entity_a = m_ecs->GetOwnerID(*i);
//				EntityID entity_b = m_ecs->GetOwnerID(*j);
//
//				// Static-Static culling
//				if (m_ecs->HasComponent(StaticTag::id, entity_a) && m_ecs->HasComponent(StaticTag::id, entity_b)) {
//					break;
//				}
//
//				ci = CircleToCircle(i->center, i->radius,
//					                j->center, j->radius);
//
//				CollidingWith* cw_a = nullptr;
//				CollidingWith* cw_b = nullptr;
//
//				i8 index_a = -1;
//				i8 index_b = -1;
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_a)) {
//					cw_a = m_ecs->GetComponent<CollidingWith>(entity_a);
//					for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//						if (cw_a->entity_ids[i] == entity_b) {
//							index_a = i;
//							break;
//						}
//					}
//				}
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_b)) {
//					cw_b = m_ecs->GetComponent<CollidingWith>(entity_b);
//					for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//						if (cw_b->entity_ids[i] == entity_a) {
//							index_b = i;
//							break;
//						}
//					}
//				}
//
//				if (ci.collision) {
//					assert(m_ecs->HasComponent(RigidBody::id, entity_a));
//					assert(m_ecs->HasComponent(RigidBody::id, entity_b));
//
//					ci.entity_ids[0] = entity_a;
//					ci.entity_ids[1] = entity_b;
//
//					bool8 collision_enter_flag = false;
//
//					if (cw_a != nullptr) {
//						if (index_a == -1) {
//							for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//								if (cw_a->entity_ids[i] == NullEntityId) {
//									cw_a->entity_ids[i] = entity_b;
//									cw_a->names[i] = m_ecs->GetComponent<Name>(entity_b)->name;
//
//									System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//										{"EntityID_primary", entity_a},
//										{"EntityID_secondary", entity_b}
//									});
//
//									collision_enter_flag = true;
//
//									break;
//								}
//							}
//						}
//					}
//
//					if (cw_b != nullptr) {
//						if (index_b == -1) {
//							for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//								if (cw_b->entity_ids[i] == NullEntityId) {
//									cw_b->entity_ids[i] = entity_a;
//									cw_b->names[i] = m_ecs->GetComponent<Name>(entity_a)->name;
//
//									if (!collision_enter_flag) {
//										System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//											{"EntityID_primary", entity_a},
//											{"EntityID_secondary", entity_b}
//										});
//									}
//
//									break;
//								}
//							}
//						}
//					}
//
//					// Enqueue collision
//					auto p_ci = reinterpret_cast<CollisionInfo*>(m_collisions_resource.allocate(sizeof(CollisionInfo), alignof(CollisionInfo)));
//					*p_ci = ci;
//				}
//				else if (index_a > -1 || index_b > -1) {
//					if (cw_a != nullptr) {
//						assert(index_a != -1);
//						cw_a->entity_ids[index_a] = NullEntityId;
//						cw_a->names[index_a] = "<NONE>"_sid;
//					}
//
//					if (cw_b != nullptr) {
//						assert(index_b != -1);
//						cw_b->entity_ids[index_b] = NullEntityId;
//						cw_b->names[index_b] = "<NONE>"_sid;
//					}
//
//					System::Events().EnqueueEvent(EventType::CollisionExit, 0.0f, {
//						{"EntityID_primary", entity_a},
//						{"EntityID_secondary", entity_b}
//					});
//				}
//			}
//		}
//	}
//
//	// TODO (dylan/aseem) : this is the dumbest O(n^2) collision detection
//	void PhysicsManager::DetectCircleAABBCollisions()
//	{
//		CollisionInfo ci{};
//
//		// With AABBs
//		auto& circleVec = m_ecs->GetComponentArray<Circle>();
//		auto& aabbVec = m_ecs->GetComponentArray<AABB>();
//
//		for (auto i = circleVec.begin(); i != circleVec.end(); ++i) {
//			for (auto j = aabbVec.begin(); j != aabbVec.end(); ++j) {
//
//				EntityID entity_a = m_ecs->GetOwnerID(*i);
//				EntityID entity_b = m_ecs->GetOwnerID(*j);
//
//				// Static-Static culling
//				if (m_ecs->HasComponent(StaticTag::id, entity_a) && m_ecs->HasComponent(StaticTag::id, entity_b)) {
//					break;
//				}
//
//				ci = CircleToAABB(i->center, i->radius,
//								  j->center, j->rx, j->ry, j->rz);
//
//				CollidingWith* cw_a = nullptr;
//				CollidingWith* cw_b = nullptr;
//
//				i8 index_a = -1;
//				i8 index_b = -1;
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_a)) {
//					cw_a = m_ecs->GetComponent<CollidingWith>(entity_a);
//					for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//						if (cw_a->entity_ids[i] == entity_b) {
//							index_a = i;
//							break;
//						}
//					}
//				}
//
//				if (m_ecs->HasComponent(CollidingWith::id, entity_b)) {
//					cw_b = m_ecs->GetComponent<CollidingWith>(entity_b);
//					for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//						if (cw_b->entity_ids[i] == entity_a) {
//							index_b = i;
//							break;
//						}
//					}
//				}
//
//				if (ci.collision) {
//					assert(m_ecs->HasComponent(RigidBody::id, entity_a));
//					assert(m_ecs->HasComponent(RigidBody::id, entity_b));
//
//					ci.entity_ids[0] = entity_a;
//					ci.entity_ids[1] = entity_b;
//
//					bool8 collision_enter_flag = false;
//
//					if (cw_a != nullptr) {
//						if (index_a == -1) {
//							for (i8 i = 0; i < cw_a->entity_ids.size(); ++i) {
//								if (cw_a->entity_ids[i] == NullEntityId) {
//									cw_a->entity_ids[i] = entity_b;
//									cw_a->names[i] = m_ecs->GetComponent<Name>(entity_b)->name;
//
//									System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//										{"EntityID_primary", entity_a},
//										{"EntityID_secondary", entity_b}
//									});
//
//									collision_enter_flag = true;
//
//									break;
//								}
//							}
//						}
//					}
//
//					if (cw_b != nullptr) {
//						if (index_b == -1) {
//							for (i8 i = 0; i < cw_b->entity_ids.size(); ++i) {
//								if (cw_b->entity_ids[i] == NullEntityId) {
//									cw_b->entity_ids[i] = entity_a;
//									cw_b->names[i] = m_ecs->GetComponent<Name>(entity_a)->name;
//
//									if (!collision_enter_flag) {
//										System::Events().EnqueueEvent(EventType::CollisionEnter, 0.0f, {
//											{"EntityID_primary", entity_a},
//											{"EntityID_secondary", entity_b}
//										});
//									}
//
//									break;
//								}
//							}
//						}
//					}
//
//					// Enqueue collision
//					auto p_ci = reinterpret_cast<CollisionInfo*>(m_collisions_resource.allocate(sizeof(CollisionInfo), alignof(CollisionInfo)));
//					*p_ci = ci;
//				}
//				else if (index_a > -1 || index_b > -1) {
//					if (cw_a != nullptr) {
//						assert(index_a != -1);
//						cw_a->entity_ids[index_a] = NullEntityId;
//						cw_a->names[index_a] = "<NONE>"_sid;
//					}
//
//					if (cw_b != nullptr) {
//						assert(index_b != -1);
//						cw_b->entity_ids[index_b] = NullEntityId;
//						cw_b->names[index_b] = "<NONE>"_sid;
//					}
//
//					System::Events().EnqueueEvent(EventType::CollisionExit, 0.0f, {
//						{"EntityID_primary", entity_a},
//						{"EntityID_secondary", entity_b}
//					});
//				}
//			}
//		}
//	}
//
//	//void PhysicsManager::DetectGroundCollisions() {
//	//	
//	//	// With GroundTags
//	//	auto& groundCheckerVec = m_ecs->GetComponentArray<GroundChecker>();
//	//	auto& aabbVec = m_ecs->GetComponentArray<AABB>();
//
//	//	for (auto i = groundCheckerVec.begin(); i != groundCheckerVec.end(); ++i) {
//	//		for (auto j = aabbVec.begin(); j != aabbVec.end(); ++j) {
//	//			
//	//			Entity entity_gc = m_ecs->GetOwner(*i);
//	//			Entity entity_aabb = m_ecs->GetOwner(*j);
//
//	//			assert(entity_gc.Has<RigidBody>() && "GroundChecker owner does not have a RigidBody.");
//	//			auto& rb = entity_gc.Get<RigidBody>();
//
//	//			if (entity_aabb.Has<GroundTag>()) {
//	//				
//	//				if (GroundedCheck(i->center, i->rx, i->rz,
//	//								  j->center, j->rx, j->ry, j->rz)) {
//	//					
//	//					rb.isGrounded = true;
//
//	//					auto& tr1 = entity_gc.Get<Transform>();
//	//					auto& tr2 = entity_aabb.Get<Transform>();
//
//	//					tr1.position.y = tr2.position.y + (tr2.scale.y * 0.5f) + (tr1.scale.y * 0.5f) + 0.05f;
//
//	//					break;
//	//				}
//	//				else {
//	//					rb.isGrounded = false;
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//
//
//	void PhysicsManager::RealignAABBs()
//	{
//		m_ecs->ForEach<AABB>([this](AABB& aabb)
//		{
//			Entity owner = m_ecs->GetOwner(aabb);
//
//			assert(owner.Has<Transform>() && "All entities with a collider must have a Transform.");
//			auto const& tr = owner.Get<Transform>();
//				
//			// Reposition
//			aabb.center = vec3(tr.position.x, tr.position.y, tr.position.z);
//
//			// Resize
//			aabb.rx = 0.5f * tr.scale.x;
//			aabb.ry = 0.5f * tr.scale.y;
//			aabb.rz = 0.5f * tr.scale.z;
//
//			// TODO : replace this method of drawing colliders:
//
//			/*if (!m_ecs->IsValidEntity(aabb.drawable_entity_id)) {
//				Entity e = System::ECS().MakeEntity();
//
//				SpriteRenderer& sr = AddIfNull<SpriteRenderer>(e);
//				sr.shape = System::Resources().GetBox2D();
//
//				float tile_x, tile_y;
//				tile_x = tile_y = 1.0f;
//				int rows, cols;
//				rows = cols = 1;
//				GLuint tex = System::Resources().LoadPNGImage(".\\Assets\\Sprites\\default_texture.png");
//				sr.material = Material{
//					tex,
//					System::Resources().LoadShaderProgram("final.vert", "final.frag"),
//					tile_x,
//					tile_y,
//					rows,
//					cols
//				};
//
//				AddIfNull<Transform>(e);
//				AddIfNull<DrawableColliderTag>(e);
//				Name& n = AddIfNull<Name>(e);
//				n.name = StringId{ "DrawableAABB" };
//
//				aabb.drawable_entity_id = e.Id();
//			}
//
//			Entity drawable = System::ECS().GetEntityFromID(aabb.drawable_entity_id);
//			Transform& dr_tr = drawable.Get<Transform>();
//			dr_tr.position = vec4(aabb.center, 1.0f);
//			dr_tr.scale.x = aabb.rx;
//			dr_tr.scale.y = aabb.ry;
//			dr_tr.scale.z = aabb.rz;
//			dr_tr.rotation = tr.rotation;
//
//			if (owner.Has<DestroyMeTag>()) {
//				System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
//					{"EntityID", drawable}
//					});
//			}*/
//		});
//	}
//
//	void PhysicsManager::RealignCircles()
//	{
//		m_ecs->ForEach<Circle>([this](Circle& circle) -> void
//		{
//			Entity owner = m_ecs->GetOwner(circle);
//
//			assert(owner.Has<Transform>() && "All entities with a collider must have a Transform.");
//			auto const& tr = owner.Get<Transform>();
//
//			// Reposition
//			circle.center = vec3{ tr.position.x, tr.position.y, tr.position.z };
//			
//			// Resize
//			circle.radius = 0.5f * std::max({ tr.scale.x, tr.scale.y, tr.scale.z });
//
//			// TODO : replace this method of drawing colliders:
//
//			/*if (!System::ECS().IsValidEntity(circle.drawable_entity_id)) {
//				Entity e = System::ECS().MakeEntity();
//
//				SpriteRenderer& sr = AddIfNull<SpriteRenderer>(e);
//				sr.shape = System::Resources().GetBox2D();
//
//				float tile_x, tile_y;
//				tile_x = tile_y = 1.0f;
//				int rows, cols;
//				rows = cols = 1;
//				GLuint tex = System::Resources().LoadPNGImage(".\\Assets\\Sprites\\default_texture.png");
//				sr.material = Material{
//					tex,
//					System::Resources().LoadShaderProgram("final.vert", "final.frag"),
//					tile_x,
//					tile_y,
//					rows,
//					cols
//				};
//
//				AddIfNull<Transform>(e);
//				AddIfNull<DrawableColliderTag>(e);
//				Name& n = AddIfNull<Name>(e);
//				n.name = StringId{ "DrawableCircle" };
//
//				circle.drawable_entity_id = e.Id();
//			}
//
//			Entity drawable = System::ECS().GetEntityFromID(circle.drawable_entity_id);
//			Transform& dr_tr = drawable.Get<Transform>();
//			dr_tr.position = vec4(circle.center, 1.0f);
//			dr_tr.scale.x = circle.radius * 2.0f;
//			dr_tr.scale.y = circle.radius * 2.0f;
//			dr_tr.scale.z = circle.radius * 2.0f;
//			dr_tr.rotation = tr.rotation;
//
//			if (owner.Has<DestroyMeTag>()) {
//				System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
//					{"EntityID", drawable}
//					});
//			}*/
//		});
//
//	}
//
//	//void PhysicsManager::RealignGroundCheckers()
//	//{
//	//	m_ecs->ForEach<GroundChecker>([this](GroundChecker& gc) -> void
//	//	{
//	//		Entity e = m_ecs->GetOwner(gc);
//
//	//		assert(e.Has<Transform>() && "All entities with a GroundChecker must have a RigidBody.");
//	//		auto const& tr = e.Get<Transform>();
//
//	//		// Reposition
//	//		gc.center = vec3(tr.position.x, tr.position.y - (tr.scale.y * 0.5f) - 0.1f, tr.position.z);
//	//		
//	//		// Resize
//	//		gc.rx = 0.5f*tr.scale.x;
//	//		gc.rz = 0.5f*tr.scale.z;
//	//	});
//	//}
//
//	void PhysicsManager::ResolveCollisions(f32 dt)
//	{
//		auto past_the_end_ptr = reinterpret_cast<CollisionInfo*>(m_collisions_resource.top());
//		
//		// TODO : this is sketchy. try to figure out how to use an iterator instead...
//		for (auto it = m_collisions_buffer.data(); it < past_the_end_ptr; ++it)
//		{
//			Entity a = m_ecs->GetEntityFromID(it->entity_ids[0]);
//			Entity b = m_ecs->GetEntityFromID(it->entity_ids[1]);
//
//			if (!a.Has<Projectile>() && !b.Has<Projectile>())
//			{
//				auto& a_wb{ a.Get<RigidBody>() };
//				auto& b_wb{ b.Get<RigidBody>() };
//
//				// Recalculate inverse mass in case mass was modified -- in DEBUG only
//				a_wb.invMass = (a_wb.mass > 0.0f) ? (1.0f / a_wb.mass) : 0.0f;
//				b_wb.invMass = (b_wb.mass > 0.0f) ? (1.0f / b_wb.mass) : 0.0f;
//
//				// If two objects are both immovable, our calc doesn't matter anyway so pass
//				if (a_wb.invMass <= 0.0f && b_wb.invMass <= 0.0f) { return; }
//
//				// Calculate relative velocity
//				vec4 relativeVel = b_wb.velocity - a_wb.velocity;
//
//				// Get collision normal from event args (ignore z direction for now)
//				vec4 normal(it->normal.xy, 0.0f, 0.0f);
//
//				// Calculate relative velocity in terms of the normal direction
//				f32 velAlongNormal = glm::dot(relativeVel, normal);
//
//				// Do not resolve if velocities are separating
//				if (velAlongNormal > 0.0f) { return; }
//
//				// Calculate restitution
//				f32 restitution = glm::min(a_wb.restitutionCoeff, b_wb.restitutionCoeff);
//
//				// Calculate impulse scalar
//				f32 impulseScalar = -(1.0f + restitution) * velAlongNormal;
//				impulseScalar /= (a_wb.invMass + b_wb.invMass);
//
//				// Apply impulse
//				vec4 impulse = impulseScalar * normal;
//				a_wb.velocity -= a_wb.invMass * impulse;
//				b_wb.velocity += b_wb.invMass * impulse;
//
//				if (a.Has<GroundTag>())
//				{
//					b_wb.isGrounded = true;
//				}
//				else if (b.Has<GroundTag>())
//				{
//					a_wb.isGrounded = true;
//				}
//
//				// Position correction
//				local_persistent constexpr float percent = 0.8f;
//				local_persistent constexpr float slop = 0.1f;
//
//				assert(a.Has<Transform>());
//				assert(b.Has<Transform>());
//				auto& a_tr = a.Get<Transform>();
//				auto& b_tr = b.Get<Transform>();
//
//				vec2 correction = glm::max(it->penetrationDepth - slop, 0.0f) / (a_wb.invMass + b_wb.invMass) * percent * normal;
//				a_tr.position -= vec4(a_wb.invMass * correction, 0.0, 0.0f);
//				b_tr.position += vec4(b_wb.invMass * correction, 0.0f, 0.0f);
//			}
//		}
//	}
//
//	void PhysicsManager::Update(f32 dt)
//	{
//		m_collisions_resource.Clear();
//
//		RealignAABBs();
//		RealignCircles();
//		//RealignGroundCheckers();
//
//		DetectAABBCollisions();
//		DetectCircleCollisions();
//		DetectCircleAABBCollisions();
//		//DetectGroundCollisions();
//
//		ResolveCollisions(dt);
//	}
//}