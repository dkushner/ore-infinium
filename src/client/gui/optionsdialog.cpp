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
#include "mainmenu.h"
#include "gui.h"

#include "src/client/client.h"

#include "src/debug.h"

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>

#include <assert.h>

OptionsDialog::OptionsDialog(Client* client, MainMenu* parent) : m_client(client), m_parent(parent)
{
    loadDocument();
}

OptionsDialog::~OptionsDialog()
{
}

void OptionsDialog::ProcessEvent(Rocket::Core::Event& event)
{
    std::cout << "Options Processing element: " << event.GetCurrentElement()->GetId().CString() << " type: " << event.GetType().CString() << '\n';

    Rocket::Core::String param = event.GetParameter<Rocket::Core::String >("submit", "");
    if (param == "accept") {
        //TODO: save and apply settings

        close();
        //FIXME: there's *got* to be a better way. I hate myself for writing this statement. I've tried
        // making the main menu delete us, but there's no way to remove event listeners short of iterating over each one, which we'd have to store manually
        // hopefully i'm just wrong and there's a way better way
    } else if (param == "cancel") {
        close();
    }

    /*
     *        Rocket::Controls::ElementFormControlInput* spatialisation_option = dynamic_cast< Rocket::Controls::ElementFormControlInput* >(options_body->GetElementById("3d"));
     *                if (spatialisation_option != NULL)
     *                {
     *                        if (GameDetails::Get3DSpatialisation())
     *                                spatialisation_option->SetAttribute("checked", "");
     *                        else
     *                                spatialisation_option->RemoveAttribute("checked");
     }
     }

     // Sent from the 'onsubmit' action of the options menu; we read the values sent from the form and make the
     // necessary changes on the game details structure.
     else if (value == "store")
     {
         // First check which button was clicked to submit the form; if it was 'cancel', then we don't want to
         // propagate the changes.
         if (event.GetParameter< Rocket::Core::String >("submit", "cancel") == "accept")
         {
             // Fetch the results of the form submission. These are stored as parameters directly on the event itself.
             // Like HTML form events, the name of the parameter is the 'name' attribute of the control, and the value
             // is whatever was put into the 'value' attribute. Checkbox values are only sent through if the box was
             // clicked. Radio buttons send through one value for the active button.
             Rocket::Core::String graphics = event.GetParameter< Rocket::Core::String >("graphics", "ok");
             bool reverb = event.GetParameter< Rocket::Core::String >("reverb", "") == "true";
             bool spatialisation = event.GetParameter< Rocket::Core::String >("3d", "") == "true";

             if (graphics == "good")
                 GameDetails::SetGraphicsQuality(GameDetails::GOOD);
             else if (graphics == "ok")
                 GameDetails::SetGraphicsQuality(GameDetails::OK);
             else if (graphics == "bad")
                 GameDetails::SetGraphicsQuality(GameDetails::BAD);

             GameDetails::SetReverb(reverb);
             GameDetails::Set3DSpatialisation(spatialisation);
     }
     }
     */

}

void OptionsDialog::loadDocument()
{
    m_options = GUI::instance()->context()->LoadDocument("../client/gui/assets/optionsDialog.rml");
    m_options->RemoveReference();
    m_options->GetElementById("title")->SetInnerRML("fuck yeah, runtime options");

    m_options->GetElementById("form")->AddEventListener("submit", this);

//    m_options->GetElementById("form")->AddEventListener("submit", this);
//    Rocket::Controls::WidgetDropDown* resolution = dynamic_cast<Rocket::Controls::WidgetDropDown*>( m_options->GetElementById("resolution"));

}

void OptionsDialog::show()
{
    GUI::instance()->addInputDemand();

    loadDocument();
    m_options->Show(Rocket::Core::ElementDocument::MODAL);
}

void OptionsDialog::close()
{
    if (m_options) {
        GUI::instance()->removeInputDemand();
        m_options->Close();
        GUI::instance()->context()->UnloadDocument(m_options);
        m_options = nullptr;
    }
}

bool OptionsDialog::visible()
{
    return m_options->IsVisible();
}

Rocket::Core::ElementDocument* OptionsDialog::document()
{
    return m_options;
}

