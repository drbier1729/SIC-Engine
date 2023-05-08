/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Dylan Bier, Aseem Apastamb, Brian Chen
 */
 

#pragma once
#include "Application.h"
#include "StringId.h"

namespace PuzzlePrototype
{
    class PuzzlePrototype : public sic::Application
    {
    public:
        PuzzlePrototype() = default;
        ~PuzzlePrototype() = default;

        virtual void Initialize(void) override;
        virtual void Shutdown(void) override {}

        virtual void SceneIn(sic::f32 dt) override;
        virtual void SceneOut(sic::f32 dt) override;

        virtual void Update(sic::f32 dt) override;
        virtual void Render(void) override;
        virtual void ImGuiRender(void) override {}
        virtual void MenuUpdate(sic::f32 dt);

        virtual void HandleSDLEvent(SDL_Event& e);
    };
}