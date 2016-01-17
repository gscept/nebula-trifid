//------------------------------------------------------------------------------
//  imguiconsole.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imguiconsole.h"
#include "imgui/imgui.h"
#include "input/key.h"

using namespace Input;
namespace Dynui
{
__ImplementClass(Dynui::ImguiConsole, 'IMCO', Core::RefCounted);
__ImplementInterfaceSingleton(Dynui::ImguiConsole);


//------------------------------------------------------------------------------
/**
*/
ImguiConsole::ImguiConsole() :
	moveScroll(false),
	visible(false),
	selectedSuggestion(0)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ImguiConsole::~ImguiConsole()
{
	__DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiConsole::Setup()
{
	// clear command buffer
	memset(this->command, '\0', 65535);

	// get script server
	this->scriptServer = Scripting::ScriptServer::Instance();

	// load commands into dictionary
	SizeT numCommands = this->scriptServer->GetNumCommands();
	for (IndexT i = 0; i < numCommands; i++)
	{
		const Ptr<Scripting::Command>& command = this->scriptServer->GetCommandByIndex(i);
		this->commands.Add(command->GetName(), command);
	}
	this->consoleBuffer.SetCapacity(2048);
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiConsole::Discard()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiConsole::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.KeysDownDuration[Key::F9] == 0.0f) this->visible = !this->visible;
	if (!this->visible) return;
	
	ImGui::Begin("Nebula T Console", NULL, ImVec2(300, 300), -1.0f, ImGuiWindowFlags_NoScrollbar);
	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImGui::GetWindowPos();	
	ImGui::BeginChild("output", ImVec2(0, windowSize.y - 75), true);
		for (int i = 0; i < this->consoleBuffer.Size(); i++)
		{
			ImGui::TextUnformatted(this->consoleBuffer[i].AsCharPtr());
		}
		if (moveScroll)
		{
			moveScroll = false;
			ImGui::SetScrollPosHere();
		}
	ImGui::EndChild();

	if (ImGui::InputText("input", this->command, sizeof(this->command), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		ImGui::SetKeyboardFocusHere();
		moveScroll = true;
		if (this->command[0] != '\0')
		{
			this->consoleBuffer.Add((const char*)this->command);
			this->consoleBuffer.Add("\n");

			// execute script
			this->Execute(this->command);

			// reset command to b empty
			memset(this->command, '\0', sizeof(this->command));
		}
	}		

	if (this->command[0] != '\0')
	{
		Util::Array<Ptr<Scripting::Command>> matches;
		IndexT i;
		for (i = 0; i < this->commands.Size(); i++)
		{
			const Util::String& name = this->commands.KeyAtIndex(i);
			if (name.FindStringIndex(this->command) == 0 && name != this->command) matches.Append(this->commands.ValueAtIndex(i));
		}

		// handle matches
		if (matches.Size() > 0)
		{
			if (io.KeysDown[Key::Up])	this->selectedSuggestion--;
			if (io.KeysDown[Key::Down]) this->selectedSuggestion++;
			this->selectedSuggestion = Math::n_iclamp(this->selectedSuggestion, 0, matches.Size() - 1);

			if (io.KeysDown[Key::Tab])
			{
				const Util::String& firstCommand = matches[this->selectedSuggestion]->GetName();
				memcpy(this->command, firstCommand.AsCharPtr(), firstCommand.Length());
				this->selectedSuggestion = 0;
			}
			else
			{
				ImGui::Begin("suggestions", NULL, ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::SetWindowPos(ImVec2(windowPos.x, windowPos.y + windowSize.y - 10));
				IndexT i;
				for (i = 0; i < matches.Size(); i++)
				{
					if (i == this->selectedSuggestion) ImGui::TextColored(ImVec4(0.5, 0.5, 0.7, 1.0f), matches[i]->GetName().AsCharPtr());
					else							   ImGui::Text(matches[i]->GetName().AsCharPtr());
				}
				ImGui::End();
			}
		}
	}
	
	ImGui::PopItemWidth();
	ImGui::End();

	//ImGui::ShowStyleEditor();
	// reset input
	//ImGui::Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiConsole::Execute(const Util::String& command)
{
	n_assert(!command.IsEmpty());
	Util::Array<Util::String> splits = command.Tokenize(" ");
	if (splits[0] == "help")
	{
		SizeT numCommands = this->scriptServer->GetNumCommands();
		IndexT i;
		for (i = 0; i < numCommands; i++)
		{ 
			const Ptr<Scripting::Command> cmd = this->scriptServer->GetCommandByIndex(i);
			Util::String output;
			output.Format("%s - %s\n", cmd->GetSyntax().AsCharPtr(), cmd->GetHelp().AsCharPtr());
			output.SubstituteString("<br />", "\n");
			this->consoleBuffer.Add(output);
		}
	}
	else if (splits[0] == "clear")
	{
		this->consoleBuffer.Reset();
	}
	else if (this->commands.Contains(splits[0]) && splits.Size() == 2)
	{
		if (splits[1] == "help")
		{
			this->consoleBuffer.Add(this->commands[splits[0]]->GetHelp());
		}
		else if (splits[1] == "syntax")
		{
			this->consoleBuffer.Add(this->commands[splits[0]]->GetSyntax());
		}
	}
	else if (!this->scriptServer->Eval(command))
	{
		this->consoleBuffer.Add(this->scriptServer->GetError() + "\n");
	}
	
	// add to previous commands
	this->previousCommands.Append(command);
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiConsole::AppendToLog(const Util::String & msg)
{
	this->consoleBuffer.Add(msg);	
}
} // namespace Dynui