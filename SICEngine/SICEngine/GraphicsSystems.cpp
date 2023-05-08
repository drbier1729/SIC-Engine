/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Brian Chen
 * Additional Authors: - Andrew Rudasics, Dylan Bier, Aseem Apastamb
 */
 

#include "ComponentSystems.h"

#include "SICEngine.h"
#include "GLM/glm/gtx/quaternion.hpp" // toMat4()
#include <SDL_scancode.h> // keyboard
#include <SDL_mouse.h> // Mouse button

namespace sic {

	void FollowCamSystem(PlayerController& controller)
	{
		Entity owner = System::ECS().GetOwner(controller);
		assert(owner.Has<Transform>());

		auto& tr{ owner.Get<Transform>() };

		if (System::Cam().View() == Camera::ViewMode::Side)
		{
			System::Cam()
				.SetTarget(tr.position)
				.SetPosition(glm::vec3(tr.position.xy, 25.0f));
		}
		else
		{
			System::Cam()
				.SetTarget(tr.position)
				.SetPosition(glm::vec3(tr.position.xy, 25.0f) - glm::vec3(0.0f, 40.0f, 0.0f));
		}

		// Keep background centered on player
		System::ECS().ForEach<BackgroundTag>([&tr](BackgroundTag& background) -> void
		{
			Entity e = System::ECS().GetOwner(background);
			assert(e.Has<Transform>());
			auto& background_tr{ e.Get<Transform>() };
			background_tr.position.x = tr.position.x;
			background_tr.position.y = tr.position.y;
		});
	}

	void DebugDrawCollidersSystem(Body& body)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto shader = System::Resources().LoadShaderProgram("debug.vert", "debug.frag");
		auto shape = System::Resources().GetBox2D();
		auto tex = System::Resources().LoadPNGImage(".\\Assets\\Sprites\\boxtexture.png");

		Entity owner = System::ECS().GetOwner(body);
		assert(owner.Has<Transform>());
		auto const& tr = owner.Get<Transform>();

		// build ModelTr
		glm::quat q = glm::quat(vec3(0.0f, 0.0f, body.rotation));
		mat4 ModelTr = sic::Translate(vec3(body.position, tr.position.z + 0.1f)) * glm::toMat4(q) * sic::Scale(vec3(body.width, 0.1f));
		vec4 DebugColor(0.0f, 0.0f, 1.0f, 1.0f);

		// Use Shader
		glUseProgram(shader);

		// draw collider
		System::Cam().LoadCamDataToGPU(shader);

		// Load Texture Uniform
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		int loc = glGetUniformLocation(shader, "tex");
		glUniform1i(loc, 0);
		
		loc = glGetUniformLocation(shader, "ModelTr");
		glUniformMatrix4fv(loc, 1, GL_FALSE, sic::Pntr(ModelTr));
		
		loc = glGetUniformLocation(shader, "DebugColor");
		glUniform4fv(loc, 1, &(DebugColor[0]));

		shape->DrawVAO();
		glUseProgram(0);

		// draw velocity
		vec2 X(0.0f, 1.0f);
		f32 XdotV = glm::dot(X, body.velocity);
		f32 m = glm::length(body.velocity);
		f32 angle = acos(XdotV / m) / glm::pi<float>() * 180.0f;
		if (body.velocity.x > 0) angle = -angle;

		ModelTr = sic::Translate(tr.position) * Rotate(2, angle) * sic::Scale(0.8f, m * 0.25f, 1.0f);
		DebugColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

		// Use Shader
		glUseProgram(shader);
		System::Cam().LoadCamDataToGPU(shader);

		// Load Texture Uniform
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		
		loc = glGetUniformLocation(shader, "tex");
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(shader, "ModelTr");
		glUniformMatrix4fv(loc, 1, GL_FALSE, sic::Pntr(ModelTr));

		loc = glGetUniformLocation(shader, "DebugColor");
		glUniform4fv(loc, 1, &(DebugColor[0]));

		System::Resources().GetArrow2D()->DrawVAO();
		
		glUseProgram(0);
	}

	void DrawSpritesSystem(SpriteRenderer& sr)
	{
		Entity owner = System::ECS().GetOwner(sr);
		assert(owner.Has<Transform>());

		float alpha = 0.0f;
		bool8 IsSplash = false;
		if (owner.Has<SplashTag>()) IsSplash = true;


		// build ModelTr
		Transform& t = owner.Get<Transform>();
		Body* body = nullptr;
		glm::quat q = glm::quat(t.rotation);
		glm::mat4 ModelTr = sic::Translate(t.position) * glm::toMat4(q) * sic::Scale(t.scale);
		glm::vec4 DebugColor(0.0f, 0.0f, 0.0f, 1.0f);

		// draw sprite
		GLuint shader = sr.material.getShader();
		int loc = 0;
		sr.material.Use();
		System::Cam().LoadCamDataToGPU(shader);

		int frame = 0;
		bool8 flip = false;
		if (owner.Has<SpriteAnimator>())
		{
			auto& sa = owner.Get<SpriteAnimator>();
			frame = sa.frame;
			flip = sa.flip;
		}

		if (owner.Has<Patrol>()) {
			Patrol& p = owner.Get<Patrol>();
			DebugColor = p.isDetected ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		
		loc = glGetUniformLocation(shader, "frame");
		glUniform1i(loc, frame);

		loc = glGetUniformLocation(sr.material.getShader(), "flip");
		glUniform1i(loc, flip);

		loc = glGetUniformLocation(shader, "debug");
		glUniform1i(loc, System::Cam().DebugCollider());

		loc = glGetUniformLocation(shader, "ModelTr");
		glUniformMatrix4fv(loc, 1, GL_FALSE, sic::Pntr(ModelTr));

		loc = glGetUniformLocation(shader, "DebugColor");
		glUniform4fv(loc, 1, &(DebugColor[0]));

		loc = glGetUniformLocation(shader, "IsSplash");
		glUniform1i(loc, IsSplash);

		// implement splash
		if (IsSplash) {
			SplashTag& sp = owner.Get<SplashTag>();
			float fps = System::FrameRate().FrameRate();
			float speed = 0.5f;
			if (fps <= 0.1f) fps = 60.0f;
			alpha = t.position.w;
			speed /= fps;

			if (!sp.fadeIn) speed = -speed;
			alpha += speed;
			t.position.w = alpha;

			if (alpha <= -0.1f || 
				System::Input().isKeyTriggered(SDL_SCANCODE_ESCAPE)		 || 
				System::Input().isKeyTriggered(SDL_SCANCODE_SPACE)       ||
				System::Input().isKeyTriggered(SDL_SCANCODE_RETURN)      ||
				System::Input().isMouseButtonTriggered(SDL_BUTTON_LMASK) ||
				System::Input().isMouseButtonTriggered(SDL_BUTTON_RMASK))
			{
				System::Events().Reset();
				System::Physics().Clear();
				System::ECS().Reset();

				if (sp.nextLevel == 0) {
					System::SceneFactory().BuildScene("SplashLevel2.json"_sid);
				}
				else {
					System::SceneFactory().BuildScene("MainMenuLevel.json"_sid);
					System::App().currentState = Application::State::SceneIn;
					System::App().prevState = Application::State::Menu;
					System::App().currentScene = "MainMenuLevel.json"_sid;
					System::App().prevScene = "MainMenuLevel.json"_sid;
				}

				System::ECS().ForEach<Body>(WriteDataToPhysicsWorldSystem);
				if (!System::Cam().Debug()) { System::ECS().ForEach<PlayerController>(FollowCamSystem); }

				System::Events().Reset();
			}
			else if (alpha > 1.4f) {
				sp.fadeIn = false;
			}
		}
		loc = glGetUniformLocation(shader, "alpha");
		glUniform1f(loc, alpha);
		

		sr.shape->DrawVAO();

		sr.material.Unuse();
	}

	void DrawTextSystem(Text& text)
	{
		u32 shader = 0;		
		f32 size = 0.0f;
		const f32 screen_width  = static_cast<float>(System::Settings().screen_width);
		const f32 screen_height = static_cast<float>(System::Settings().screen_height);
		i32 loc = 0;
		vec3 pos(0.0f);
		vec3 pos_max(0.0f);
		vec3 pos_min(0.0f);
		vec3 color(0.0f);

		// Here only draw text in menu
		if (text.IsCast) return;

		Entity owner = System::ECS().GetOwner(text);
		assert(owner.Has<Transform>());
		Transform& transform = owner.Get<Transform>();

		// update menu item position
		auto const* Characters = System::Resources().GetCharMap();
		const char* tmp_text = System::Resources().GetString(text.text.Id());
		float width = 0.0f, height = 0.0f;
	
			for (int i = 0; i < std::strlen(tmp_text); ++i)
		{
			ResourceManager::Character ch = Characters->at(tmp_text[i]);
				
			// Advance include width + space between characters
			width += static_cast<float>(ch.Advance >> 6);
			height = glm::max(height, static_cast<float>(ch.Size.y));
		}
		transform.scale.x = width * text.scale;
		transform.scale.y = height * text.scale;
		
		// calculate pos. position range 0% ~ 100%
		pos.x = screen_width  * transform.position.x / 100.0f - 0.5f * transform.scale.x;
		pos.y = screen_height * transform.position.y / 100.0f - 0.5f * transform.scale.y;

		// calculatge pos's max
		pos_max.x = screen_width  - transform.scale.x;
		pos_max.y = screen_height - transform.scale.y;

		// fix pos if pos > pos_max
		if (pos.x > pos_max.x) pos.x = pos_max.x;
		if (pos.y > pos_max.y) pos.y = pos_max.y;
		if (pos.x < pos_min.x) pos.x = pos_min.x;
		if (pos.y < pos_min.y) pos.y = pos_min.y;

		shader = sic::System::Resources().LoadShaderProgram(".\\textRenderer.vert", ".\\textRenderer.frag");		
		size = text.scale;
		color = text.color;

		// draw text as MenuButton
		glUseProgram(shader);	
		System::Cam().LoadCamDataToGPU(shader);

		const char* menu_text = System::Resources().GetString(text.text.Id());

		Font* font = System::Resources().GetFont();
		font->Settings(menu_text, pos.x, pos.y, size, color);
		

		loc = glGetUniformLocation(shader, "color");
		glUniform3fv(loc, 1, &color[0]);

		loc = glGetUniformLocation(shader, "isText");
		glUniform1i(loc, true);
		font->DrawVAO();				

		glUseProgram(0);
	}

	void DrawCastSystem(Text& text) {
		f32 fps = 0.0f;
		f32 speed = 1.0f;
		f32 bot = -50.0f;
		f32 x = 0.0f;
		f32 y = 0.0f;
		f32 size = 0.0f;

		f32 screen_width  = System::Settings().screen_width;
		f32 screen_height = System::Settings().screen_height;

		u32 shader = 0;
		i32 loc = 0;

		vec3 color(0.0f);
		
		Entity owner = System::ECS().GetOwner(text);

		// assume it is not in the menu
		if (!text.IsCast) return;				

		assert(owner.Has<Transform>());
		auto& tr = owner.Get<Transform>();
		y = tr.position.y;

		// caculate moving speed
		fps = System::FrameRate().FrameRate();
		if (fps == 0.0f) fps = 60.0f;

		speed = speed * (1 / fps);

		// move down until it reaches the bottom
		speed = screen_height * speed / 100.0f;
		if(y >= bot) tr.position.y -= speed;		
		x = screen_width  * tr.position.x / 100.0f;
		y = screen_height * tr.position.y / 100.0f;

		shader = sic::System::Resources().LoadShaderProgram(".\\textRenderer.vert", ".\\textRenderer.frag");
		size = text.scale;
		color = text.color;		

		// draw text as MenuButton
		glUseProgram(shader);
		System::Cam().LoadCamDataToGPU(shader);

		const char* menu_text = System::Resources().GetString(text.text.Id());

		Font* font = System::Resources().GetFont();
		font->Settings(menu_text, x, y, size, color);


		loc = glGetUniformLocation(shader, "color");
		glUniform3fv(loc, 1, &color[0]);

		loc = glGetUniformLocation(shader, "isText");
		glUniform1i(loc, true);
		font->DrawVAO();

		glUseProgram(0);
	}

	void InitMenuSystem(MenuTag& mt)
	{
		Entity owner = System::ECS().GetOwner(mt);
		assert(owner.Has<Transform>());
		auto& tr = owner.Get<Transform>();

		tr.position.x = System::Cam().GetPosition().x;
		tr.position.y = System::Cam().GetPosition().y;
	}

	void MenuClickSystem(Text& text)
	{
		vec2 mouse;
		vec3 pos(0.0f);
		vec3 pos_max(0.0f);
		vec3 pos_min(0.0f);
		glm::ivec2 tmp;
		float screen_width  = 0.0f;
		float screen_height = 0.0f;
		float width = 100.0f;
		float height = 50.0f;
		bool collision = true;

		screen_width  = static_cast<float>(System::Settings().screen_width);
		screen_height = static_cast<float>(System::Settings().screen_height);
		tmp = System::Input().GetMousePosition();
		mouse = vec2(static_cast<float>(tmp.x), static_cast<float>(tmp.y));

		// convert mouse pos to text position
		mouse.y = screen_height - mouse.y;

		// get text's position
		Entity owner = System::ECS().GetOwner(text);
		assert(owner.Has<Transform>());

		Transform& transform = owner.Get<Transform>();

		// get text's width & height
		width = transform.scale.x;
		height = transform.scale.y;

		// calculate pos. position range 0% ~ 100%
		pos.x = screen_width  * transform.position.x / 100.0f  - 0.5f * transform.scale.x;
		pos.y = screen_height * transform.position.y / 100.0f  - 0.5f * transform.scale.y;

		// calculatge pos's max
		pos_max.x = screen_width  - transform.scale.x;
		pos_max.y = screen_height - transform.scale.y;

		// fix pos if pos > pos_max
		if (pos.x > pos_max.x) pos.x = pos_max.x;
		if (pos.y > pos_max.y) pos.y = pos_max.y;
		if (pos.x < pos_min.x) pos.x = pos_min.x;
		if (pos.y < pos_min.y) pos.y = pos_min.y;


		if (mouse.x < pos.x) {
			collision = false;
		}
		else if (mouse.x > pos.x + width) {
			collision = false;
		}
		else if (mouse.y < pos.y) {
			collision = false;
		}
		else if (mouse.y > pos.y + height) {
			collision = false;
		}

		if (collision)		  { text.color = vec3(0.0f, 0.0f, 1.0f); } // blue
		else if (text.active) { text.color = vec3(1.0f, 0.0f, 0.0f); } // red
		else				  { text.color = vec3(0.0f, 1.0f, 0.0f); } // green

		if (collision && System::Input().isMouseButtonTriggered(SDL_BUTTON_LMASK))
		{
			System::Audio().SetChannelVolume(sic::AudioType::MENU, 0.25f);
			System::Audio().PlaySound(".\\Assets\\Audio\\light_switch.mp3", sic::AudioType::MENU);
			switch (text.text.Id()) {
			case "Click Here to Start"_sid:
			{
				System::App().prevScene = System::App().currentScene;
				System::App().currentScene = "TutorialLevel.json"_sid;

				System::App().currentState = Application::State::SceneIn;
				break;
			}
			case "Restart Level"_sid:
			{
				System::App().currentState = Application::State::SceneIn;
				break;
			}
			case "Resume"_sid:
			{
				System::App().currentState = Application::State::Play;
				break;
			}
			case "Controls"_sid:
			{
				if (!text.active)
				{
					text.sub_0 = System::SceneFactory().BuildEntityFromArchetype("ControlsMenu.json"_sid).Id();
					text.active = true;
				}
				else
				{
					Entity sub = System::ECS().GetEntityFromID(text.sub_0);
					System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
						{ "EntityID", sub }
					});
					text.active = false;
				}
				break;
			}
			case "Credits"_sid:
			{
				if (!text.active)
				{
					text.sub_0 = System::SceneFactory().BuildEntityFromArchetype("ConfirmGoToCredits.json"_sid).Id();
					text.active = true;
				}
				else
				{
					Entity sub0 = System::ECS().GetEntityFromID(text.sub_0);
					System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
						{ "EntityID", sub0 }
						});
					text.active = false;
				}
				break;
			}
			case "Options"_sid:
			{
				if (!text.active)
				{
					text.sub_0 = System::SceneFactory().BuildEntityFromArchetype("MuteAudioButton.json"_sid).Id();
					text.sub_1 = System::SceneFactory().BuildEntityFromArchetype("ChangeWindowButton.json"_sid).Id();
					text.active = true;
				}
				else
				{
					Entity sub0 = System::ECS().GetEntityFromID(text.sub_0);
					System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
						{ "EntityID", sub0 }
					});
					
					Entity sub1 = System::ECS().GetEntityFromID(text.sub_1);
					System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
						{ "EntityID", sub1 }
					});
					text.active = false;
				}
				break;
			}
			case "Quit"_sid:
			{
				if (!text.active)
				{
					text.sub_0 = System::SceneFactory().BuildEntityFromArchetype("AreYouSure.json"_sid).Id();
					text.active = true;
				}
				else
				{
					Entity sub = System::ECS().GetEntityFromID(text.sub_0);
					System::Events().EnqueueEvent(EventType::Destroy, 0.0f, {
						{ "EntityID", sub }
					});
					text.active = false;
				}
				break;
			}
			case "Confirm: Quit"_sid:
			{
				System::Quit();
				break;
			}
			case  "Toggle Audio Mute/Unmute"_sid:
			{
				System::Audio().ToggleMuteAll();
				break;
			}
			case  "Toggle Fullscreen/Windowed"_sid:
			{
				System::Cam().ToggleFullscreen();
				break;
			}
			case "Confirm: Lose Progress & Go To Credits"_sid:
			{
				// Jump to credits level
				System::App().currentState = Application::State::SceneIn;
				System::App().prevScene = System::App().currentScene;
				System::App().currentScene = "CreditsLevel.json"_sid;
				break;
			}
			default: { break; }
			}
			
		}
	}

	// This also changes the game state...
	void CalculateFadeSystem(FadeScreen& f, f32 dt)
	{
		// SceneIn -> Play
		if (f.fadeIn && f.alpha > 0)
		{
			f.alpha -= 1.0f / f.time * dt;
			if (f.alpha <= 0)
			{
				if (System::App().currentScene == "MainMenuLevel.json"_sid)
				{
					System::App().currentState = Application::State::Menu;
				}
				else
				{
					System::App().currentState = Application::State::Play;
				}
			}
		}

		// SceneOut -> SceneIn
		else if (!f.fadeIn && f.alpha < f.maximum)
		{
			f.alpha += 1.0f / f.time * dt;
			if (f.alpha >= f.maximum)
			{
				System::App().currentState = Application::State::SceneIn;
			}
		}
	}

	void RenderFadeSystem(FadeScreen& f)
	{
		u32 shader = sic::System::Resources().LoadShaderProgram(".\\textRenderer.vert", ".\\textRenderer.frag");

		// draw fade out screen (it's a Box2D actually)
		glUseProgram(shader);

		u32 loc = 0;

		System::Cam().LoadCamDataToGPU(shader);
		loc = glGetUniformLocation(shader, "isText");
		glUniform1i(loc, false);
		loc = glGetUniformLocation(shader, "alpha");
		glUniform1f(loc, f.alpha);
		System::Resources().GetBox2D()->DrawVAO();

		glUseProgram(0);
	}

	void UpdateFrameSystem(SpriteAnimator& sa)
	{
		if (sa.frame_counter < sa.playback_speed)
		{
			++sa.frame_counter;
		}
		else
		{
			sa.frame_counter = 0;

			if (sa.frame == sa.clips[sa.clip].end_idx)
			{
				sa.frame = sa.clips[sa.clip].start_idx;
			}
			else
			{
				++sa.frame;
			}
		}
	}

	void PlayerAnimatorControllerSystem(SpriteAnimator& sa)
	{
		sic::Entity e = sic::System::ECS().GetOwner(sa);

		if (e.Has<Body>())
		{
			auto& rb = e.Get<Body>();
			f32 scale = (fabs(rb.velocity.x) > 0) ? 1.0f / fabs(rb.velocity.x) : 0.0f;
			if (scale > 1.0f) { scale = 1.0f; }
			if (scale < 0.0f) { scale = 0.0f; }
			sa.playback_speed = 2 + (6 * static_cast<i32>(scale)); // WARNING: this truncates a float to an int

			if (rb.velocity.x < -0.05f && sa.flip != true)
			{
				sa.flip = true;
			}
			else if (rb.velocity.x > 0.05f && sa.flip != false)
			{
				sa.flip = false;
			}

			if (fabs(rb.velocity.x) < 0.2f)
			{
				if (sa.clip != 0)
				{
					sa.clip = 0;
					sa.frame = sa.clips[sa.clip].start_idx;
				}
			}
			else
			{
				if (sa.clip != 2)
				{
					sa.clip = 2;
					sa.frame = sa.clips[sa.clip].start_idx;
				}

				int frame = sa.frames[sa.frame];
				if (rb.isGrounded && (frame == 8 || frame == 13))
				{
					System::Audio().SetChannelVolume(sic::AudioType::FOOTSTEPS, 0.25f);
					System::Audio().PlaySound(".\\Assets\\Audio\\footsteps.mp3", sic::AudioType::FOOTSTEPS);
				}
			}
		}
	}

}