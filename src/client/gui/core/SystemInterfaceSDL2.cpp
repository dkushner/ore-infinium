/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 Nuno Silva
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "SystemInterfaceSDL2.h"

#include <Rocket/Core.h>

#include <SDL.h>

#include <iostream>
#include <assert.h>

int SystemInterfaceSDL2::GetKeyModifiers()
{
    int Modifiers = 0;

    SDL_Keymod mod = SDL_GetModState();
    if (mod == KMOD_LSHIFT || mod == KMOD_RSHIFT) {
        Modifiers |= Rocket::Core::Input::KM_SHIFT;
    }

    if (mod == KMOD_LCTRL || mod == KMOD_RCTRL) {
        Modifiers |= Rocket::Core::Input::KM_CTRL;
    }

    if (mod == KMOD_LSHIFT || mod == KMOD_RSHIFT) {
        Modifiers |= Rocket::Core::Input::KM_SHIFT;
    }

    if (mod == KMOD_LALT || mod == KMOD_RALT) {
        Modifiers |= Rocket::Core::Input::KM_ALT;
    }

    //FIXME: not actually sure if that's meta or not
    if (mod == KMOD_LGUI || mod == KMOD_LGUI) {
        Modifiers |= Rocket::Core::Input::KM_META;
    }

    return Modifiers;
}

int SystemInterfaceSDL2::TranslateMouseButton(Uint8 button)
{
    switch (button) {
    case 1: //left
        return 0;
    case 2: //MMB
        return 3;
    case 3: //right
        return 2;

    //unhandled.
    default:
        return -1;
    }
}

Rocket::Core::Input::KeyIdentifier SystemInterfaceSDL2::TranslateKey(SDL_Keycode code)
{
    switch (code) {
    case SDLK_a:
        return Rocket::Core::Input::KI_A;
        break;
    case SDLK_b:
        return Rocket::Core::Input::KI_B;
        break;
    case SDLK_c:
        return Rocket::Core::Input::KI_C;
        break;
    case SDLK_d:
        return Rocket::Core::Input::KI_D;
        break;
    case SDLK_e:
        return Rocket::Core::Input::KI_E;
        break;
    case SDLK_f:
        return Rocket::Core::Input::KI_F;
        break;
    case SDLK_g:
        return Rocket::Core::Input::KI_G;
        break;
    case SDLK_h:
        return Rocket::Core::Input::KI_H;
        break;
    case SDLK_i:
        return Rocket::Core::Input::KI_I;
        break;
    case SDLK_j:
        return Rocket::Core::Input::KI_J;
        break;
    case SDLK_k:
        return Rocket::Core::Input::KI_K;
        break;
    case SDLK_l:
        return Rocket::Core::Input::KI_L;
        break;
    case SDLK_m:
        return Rocket::Core::Input::KI_M;
        break;
    case SDLK_n:
        return Rocket::Core::Input::KI_N;
        break;
    case SDLK_o:
        return Rocket::Core::Input::KI_O;
        break;
    case SDLK_p:
        return Rocket::Core::Input::KI_P;
        break;
    case SDLK_q:
        return Rocket::Core::Input::KI_Q;
        break;
    case SDLK_r:
        return Rocket::Core::Input::KI_R;
        break;
    case SDLK_s:
        return Rocket::Core::Input::KI_S;
        break;
    case SDLK_t:
        return Rocket::Core::Input::KI_T;
        break;
    case SDLK_u:
        return Rocket::Core::Input::KI_U;
        break;
    case SDLK_v:
        return Rocket::Core::Input::KI_V;
        break;
    case SDLK_w:
        return Rocket::Core::Input::KI_W;
        break;
    case SDLK_x:
        return Rocket::Core::Input::KI_X;
        break;
    case SDLK_y:
        return Rocket::Core::Input::KI_Y;
        break;
    case SDLK_z:
        return Rocket::Core::Input::KI_Z;
        break;
    case SDLK_0:
        return Rocket::Core::Input::KI_0;
        break;
    case SDLK_1:
        return Rocket::Core::Input::KI_1;
        break;
    case SDLK_2:
        return Rocket::Core::Input::KI_2;
        break;
    case SDLK_3:
        return Rocket::Core::Input::KI_3;
        break;
    case SDLK_4:
        return Rocket::Core::Input::KI_4;
        break;
    case SDLK_5:
        return Rocket::Core::Input::KI_5;
        break;
    case SDLK_6:
        return Rocket::Core::Input::KI_6;
        break;
    case SDLK_7:
        return Rocket::Core::Input::KI_7;
        break;
    case SDLK_8:
        return Rocket::Core::Input::KI_8;
        break;
    case SDLK_9:
        return Rocket::Core::Input::KI_9;
        break;
    case SDLK_KP_0:
        return Rocket::Core::Input::KI_NUMPAD0;
        break;
    case SDLK_KP_1:
        return Rocket::Core::Input::KI_NUMPAD1;
        break;
    case SDLK_KP_2:
        return Rocket::Core::Input::KI_NUMPAD2;
        break;
    case SDLK_KP_3:
        return Rocket::Core::Input::KI_NUMPAD3;
        break;
    case SDLK_KP_4:
        return Rocket::Core::Input::KI_NUMPAD4;
        break;
    case SDLK_KP_5:
        return Rocket::Core::Input::KI_NUMPAD5;
        break;
    case SDLK_KP_6:
        return Rocket::Core::Input::KI_NUMPAD6;
        break;
    case SDLK_KP_7:
        return Rocket::Core::Input::KI_NUMPAD7;
        break;
    case SDLK_KP_8:
        return Rocket::Core::Input::KI_NUMPAD8;
        break;
    case SDLK_KP_9:
        return Rocket::Core::Input::KI_NUMPAD9;
        break;
    case SDLK_LEFT:
        return Rocket::Core::Input::KI_LEFT;
        break;
    case SDLK_RIGHT:
        return Rocket::Core::Input::KI_RIGHT;
        break;
    case SDLK_UP:
        return Rocket::Core::Input::KI_UP;
        break;
    case SDLK_DOWN:
        return Rocket::Core::Input::KI_DOWN;
        break;
    case SDLK_DELETE:
        return Rocket::Core::Input::KI_DELETE;
        break;
    case SDLK_BACKSPACE:
        return Rocket::Core::Input::KI_BACK;
        break;
    case SDLK_KP_PLUS:
        return Rocket::Core::Input::KI_ADD;
        break;
    case SDLK_KP_BACKSPACE:
        return Rocket::Core::Input::KI_BACK;
        break;
    case SDLK_KP_DECIMAL:
        return Rocket::Core::Input::KI_DELETE;
        break;
    case SDLK_KP_DIVIDE:
        return Rocket::Core::Input::KI_DIVIDE;
        break;
    case SDLK_END:
        return Rocket::Core::Input::KI_END;
        break;
    case SDLK_ESCAPE:
        return Rocket::Core::Input::KI_ESCAPE;
        break;
    case SDLK_F1:
        return Rocket::Core::Input::KI_F1;
        break;
    case SDLK_F2:
        return Rocket::Core::Input::KI_F2;
        break;
    case SDLK_F3:
        return Rocket::Core::Input::KI_F3;
        break;
    case SDLK_F4:
        return Rocket::Core::Input::KI_F4;
        break;
    case SDLK_F5:
        return Rocket::Core::Input::KI_F5;
        break;
    case SDLK_F6:
        return Rocket::Core::Input::KI_F6;
        break;
    case SDLK_F7:
        return Rocket::Core::Input::KI_F7;
        break;
    case SDLK_F8:
        return Rocket::Core::Input::KI_F8;
        break;
    case SDLK_F9:
        return Rocket::Core::Input::KI_F9;
        break;
    case SDLK_F10:
        return Rocket::Core::Input::KI_F10;
        break;
    case SDLK_F11:
        return Rocket::Core::Input::KI_F11;
        break;
    case SDLK_F12:
        return Rocket::Core::Input::KI_F12;
        break;
    case SDLK_F13:
        return Rocket::Core::Input::KI_F13;
        break;
    case SDLK_F14:
        return Rocket::Core::Input::KI_F14;
        break;
    case SDLK_F15:
        return Rocket::Core::Input::KI_F15;
        break;
    case SDLK_HOME:
        return Rocket::Core::Input::KI_HOME;
        break;
    case SDLK_INSERT:
        return Rocket::Core::Input::KI_INSERT;
        break;
    case SDLK_LCTRL:
        return Rocket::Core::Input::KI_LCONTROL;
        break;
    case SDLK_LSHIFT:
        return Rocket::Core::Input::KI_LSHIFT;
        break;
//    case SDLK_
        //FIXME: UNFOUND?
//       return Rocket::Core::Input::KI_MULTIPLY;
        //      break;
    case SDLK_PAUSE:
        return Rocket::Core::Input::KI_PAUSE;
        break;
    case SDLK_RCTRL:
        return Rocket::Core::Input::KI_RCONTROL;
        break;
    case SDLK_RETURN:
    case SDLK_RETURN2:
        return Rocket::Core::Input::KI_RETURN;
        break;
    case SDLK_RSHIFT:
        return Rocket::Core::Input::KI_RSHIFT;
        break;
    case SDLK_SPACE:
        return Rocket::Core::Input::KI_SPACE;
        break;
    case SDLK_MINUS:
        return Rocket::Core::Input::KI_SUBTRACT;
        break;
    case SDLK_TAB:
        return Rocket::Core::Input::KI_TAB;
        break;
    };

    return Rocket::Core::Input::KI_UNKNOWN;
}

float SystemInterfaceSDL2::GetElapsedTime()
{
    return static_cast<float>(SDL_GetTicks()) / 1000.0f;
}

bool SystemInterfaceSDL2::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message)
{
    std::string Type;

    switch (type) {
    case Rocket::Core::Log::LT_ALWAYS:
        Type = "[Always]";
        break;
    case Rocket::Core::Log::LT_ERROR:
        Type = "[Error]";
        break;
    case Rocket::Core::Log::LT_ASSERT:
        Type = "[Assert]";
        break;
    case Rocket::Core::Log::LT_WARNING:
        Type = "[Warning]";
        break;
    case Rocket::Core::Log::LT_INFO:
        Type = "[Info]";
        break;
    case Rocket::Core::Log::LT_DEBUG:
        Type = "[Debug]";
        break;
    case Rocket::Core::Log::LT_MAX:
        Type = "[Max]";
        break;
    };

    printf("%s - %s\n", Type.c_str(), message.CString());

    return true;
}
