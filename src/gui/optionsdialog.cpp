/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "optionsdialog.h"
#include "gui.h"

#include "../game.h"

#include <Rocket/Core.h>

#include <iostream>

#include <assert.h>

OptionsDialog::OptionsDialog(Game* game) : m_game(game)
{
    m_options = GUI::instance()->context()->LoadDocument("../gui/assets/optionsDialog.rml");
    assert(m_options);
//    m_options->GetElementById("title")->SetInnerRML("fuck yeah, runtime text");

 //   m_options->GetElementById("element1")->AddEventListener("click", this);
    m_options->Show();
    std::cout << "OPTIONS DIALOG CTOR \n";
}

OptionsDialog::~OptionsDialog()
{

}

void OptionsDialog::ProcessEvent(Rocket::Core::Event& event)
{
    std::cout << "Processing element: " << event.GetCurrentElement()->GetId().CString() << " type: " << event.GetType().CString() << '\n';
}

void OptionsDialog::show()
{
    m_options->Show();
}

