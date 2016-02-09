//------------------------------------------------------------------------------
//  characternodehandler.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characternodehandler.h"
#include "graphics/graphicsprotocol.h"
#include "contentbrowserapp.h"
#include "previewer/previewstate.h"
#include "messaging/messagecallbackhandler.h"
#include "imgui.h"
#include "graphics/modelentity.h"
#include "characters/character.h"

using namespace Graphics;
using namespace Util;
namespace Widgets
{
__ImplementClass(Widgets::CharacterNodeHandler, 'CHNH', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
CharacterNodeHandler::CharacterNodeHandler() :
	selectedJoint(0),
	selectedMask(-1),
    currentSeekInterval(0),
	numFrames(0),
	currentFrame(0),
	paused(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterNodeHandler::~CharacterNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::Setup()
{
	// first get current model
	this->model = ContentBrowser::ContentBrowserApp::Instance()->GetPreviewState()->GetModel();
	this->ui->maskBox->clear();
	this->ui->maskName->setText("");

	// now fetch skins
	Ptr<FetchSkinList> skinMsg = FetchSkinList::Create();
    __Send(this->model, skinMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedSkinList, this, skinMsg.upcast<Messaging::Message>());
	
	// do the same or clips
	Ptr<FetchClips> clipMsg = FetchClips::Create();
	__Send(this->model, clipMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedClipList, this, clipMsg.upcast<Messaging::Message>());

	// connect all shortcuts for double clicking the lists
	connect(this->ui->availableSkins, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnSkinActivated()));
	connect(this->ui->visibleSkins, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnSkinDeactivated()));

	// connect buttons
	connect(this->ui->addClip, SIGNAL(clicked()), this, SLOT(OnAddClip()));
	connect(this->ui->removeClip, SIGNAL(clicked()), this, SLOT(OnRemoveClip()));
	connect(this->ui->showSkin, SIGNAL(clicked()), this, SLOT(OnShowSkin()));
	connect(this->ui->hideSkin, SIGNAL(clicked()), this, SLOT(OnHideSkin()));
	connect(this->ui->playClip, SIGNAL(clicked()), this, SLOT(OnPlayClip()));
	connect(this->ui->stopClip, SIGNAL(clicked()), this, SLOT(OnStopClip()));
	connect(this->ui->pauseClip, SIGNAL(clicked()), this, SLOT(OnPauseClip()));
	connect(this->ui->seekBar, SIGNAL(valueChanged(int)), this, SLOT(OnClipSeek(int)));

	// connect UI used for joint mask management
	connect(this->ui->newMaskButton, SIGNAL(clicked()), this, SLOT(OnNewMask()));
	connect(this->ui->deleteMaskButton, SIGNAL(clicked()), this, SLOT(OnDeleteMask()));
	connect(this->ui->maskName, SIGNAL(textEdited(const QString&)), this, SLOT(OnMaskNameChanged(const QString&)));
	connect(this->ui->maskBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaskSelected(int)));

	// connect clip list double click
	connect(this->ui->clipsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnClipDoubleClicked(QListWidgetItem*)));

	// create track controller
	this->trackController = new TrackController(this->ui);

	// connect controller stuff
	connect(this->trackController, SIGNAL(WeightChanged(IndexT)), this, SLOT(OnWeightChanged(IndexT)));
    connect(this->trackController, SIGNAL(ClipRemoved(IndexT)), this, SLOT(OnRemoveClip()));

	// connect skeleton render bool
	connect(this->ui->renderSkeleton, SIGNAL(toggled(bool)), this, SLOT(OnRenderSkeleton(bool)));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::Discard()
{
	this->model = 0;
	delete this->trackController;
	this->trackController = 0;
    this->currentDurations.Clear();
	this->skinVisible.Clear();
	this->clipsPlaying.Clear();
	this->clipsMask.Clear();
	this->clipWeights.Clear();
	this->joints.Clear();
	this->masks.Clear();
	this->characterJointMasks.Clear();

	// make sure this object doesn't get called if the handler has been destroyed
	__AbortSingleFireCallbacks(CharacterNodeHandler, this);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnSkinActivated()
{
	// get current item
	QListWidgetItem* currentItem = this->ui->availableSkins->currentItem();

	Ptr<ShowSkin> showSkin = ShowSkin::Create();
	showSkin->SetSkin(currentItem->text().toUtf8().constData());
	this->model->HandleMessage(showSkin.upcast<Messaging::Message>());

	// now remove item from visible skins and add it to available
	this->ui->visibleSkins->addItem(currentItem->text());
	this->ui->availableSkins->takeItem(this->ui->availableSkins->row(currentItem));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnSkinDeactivated()
{
	// get current item
	QListWidgetItem* currentItem = this->ui->visibleSkins->currentItem();

	Ptr<HideSkin> hideSkin = HideSkin::Create();
	hideSkin->SetSkin(currentItem->text().toUtf8().constData());
	this->model->HandleMessage(hideSkin.upcast<Messaging::Message>());

	// now remove item from visible skins and add it to available
	this->ui->availableSkins->addItem(currentItem->text());
	this->ui->visibleSkins->takeItem(this->ui->visibleSkins->row(currentItem));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnAddClip()
{
	// get selected item and index
	QListWidgetItem* item = this->ui->clipsList->selectedItems()[0];
	this->trackController->AddClip(item->text().toUtf8().constData());

    // set current duration in slot to be the actual duration
    SizeT duration = this->durations[this->ui->clipsList->currentIndex().row()];
    this->currentDurations[this->ui->clipsList->currentIndex().row()] = duration;

    // calculate maximum and set the seek bar to show this amount of frames
    SizeT maximum = 0;
    IndexT i;
    for (i = 0; i < this->currentDurations.Size(); i++)
    {
        maximum = Math::n_max(maximum, this->currentDurations[i]);
    }
    this->ui->seekBar->setMaximum(maximum / 40);

    // automatically start clips
	this->OnPlayClip();
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnRemoveClip()
{
	// get selected index
	QModelIndex index = this->ui->trackView->currentIndex();
	this->trackController->RemoveClip(index.row());

    // set duration to be 0
    this->currentDurations[this->ui->clipsList->currentIndex().row()] = 0;

    // calculate maximum and set the seek bar to show this amount of frames
    SizeT maximum = 0;
    IndexT i;
    for (i = 0; i < this->currentDurations.Size(); i++)
    {
        maximum = Math::n_max(maximum, this->currentDurations[i]);
    }
    this->ui->seekBar->setMaximum(maximum / 40);

    // automatically start clips
	this->OnPlayClip();
}

//------------------------------------------------------------------------------
/**
	Plays clip from track controller, if we have conflicting weights on the same track, 
	the latter clip will take over.
*/
void 
CharacterNodeHandler::OnPlayClip()
{
	// stop all prior clips
	Ptr<AnimStopAllTracks> stopMsg = AnimStopAllTracks::Create();
	stopMsg->SetAllowFadeOut(false);
	this->model->HandleMessage(stopMsg.upcast<Messaging::Message>());

	// get data from controller
	const Array<String>& clips = this->trackController->GetClips();
	const Array<IndexT>& tracks = this->trackController->GetTracks();
	const Array<float>& weights = this->trackController->GetWeights();

	// reset seek bar
	this->ui->seekBar->setValue(0);

	// iterate through clips
	IndexT i;
	for (i = 0; i < clips.Size(); i++)
	{
		// create message
		Ptr<AnimPlayClip> msg = AnimPlayClip::Create();

		// create message
		msg->SetClipName(clips[i]);
		msg->SetTrackIndex(i);
		msg->SetBlendWeight(weights[i]);
		msg->SetLoopCount(0);

		// send to model
        this->model->HandleMessage(msg.upcast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnPauseClip()
{
	// create message and send to model
	Ptr<AnimPauseAllTracks> msg = AnimPauseAllTracks::Create();
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnStopClip()
{
	// create message and send to model
	Ptr<AnimStopAllTracks> msg = AnimStopAllTracks::Create();
	msg->SetAllowFadeOut(false);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnClipSeek(int time)
{
	Ptr<AnimSeek> msg = AnimSeek::Create();
	msg->SetTime(time * 40);
    this->ui->currentFrame->setValue(time);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnClipDoubleClicked(QListWidgetItem* item)
{
	// simply call add clip
	this->OnAddClip();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnWeightChanged(int index)
{
	// get weight and track
	float weight = this->trackController->GetWeights()[index];

	// create message
	Ptr<AnimModifyBlendWeight> msg = AnimModifyBlendWeight::Create();
	msg->SetBlendWeight(weight);
	msg->SetTrackIndex(index);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnShowSkin()
{
	if (this->ui->availableSkins->currentItem() != NULL)
	{
		this->OnSkinActivated();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnHideSkin()
{
	if (this->ui->visibleSkins->currentItem() != NULL)
	{
		this->OnSkinDeactivated();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::ReshowSkins()
{
	SizeT numSkins = this->ui->visibleSkins->count();
	IndexT skinIndex;
	for (skinIndex = 0; skinIndex < numSkins; skinIndex++)
	{
		Ptr<ShowSkin> showSkin = ShowSkin::Create();
		QListWidgetItem* item = this->ui->visibleSkins->item(skinIndex);
		showSkin->SetSkin(item->text().toUtf8().constData());
		this->model->HandleMessage(showSkin.upcast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnRenderSkeleton(bool b)
{
	Ptr<SetRenderSkeleton> skelRend = SetRenderSkeleton::Create();
	skelRend->SetRender(b);
	this->model->HandleMessage(skelRend.downcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnFetchedSkinList(const Ptr<Messaging::Message>& msg)
{
	const Ptr<FetchSkinList>& skinMsg = msg.downcast<FetchSkinList>();

	// then copy list to our skin list
	this->skins = skinMsg->GetSkins();
	this->skinVisible.Resize(this->skins.Size());
	this->skinVisible.Fill(false);
	this->skinVisible[0] = true;

	// get joints
	const Characters::CharacterSkeleton& skeleton = this->model->GetCharacter()->Skeleton();
	this->joints.Resize(skeleton.GetNumJoints());
	IndexT i;
	for (i = 0; i < this->joints.Size(); i++)
	{
		this->joints[i] = skeleton.GetJoint(i);
	}

	// get masks from character
	SizeT numMasks = this->model->GetCharacter()->Skeleton().GetNumMasks();
	for (i = 0; i < numMasks; i++)
	{
		Characters::CharacterJointMask* mask = this->model->GetCharacter()->Skeleton().GetMask(i);
		this->characterJointMasks.Append(mask);

		// create toolkit counterpart
		ToolkitUtil::JointMask toolkitMask;
		toolkitMask.name = mask->GetName().AsString();
		toolkitMask.weights = mask->GetWeights();
		this->masks.Append(toolkitMask);

		// add to UI
		this->ui->maskBox->addItem(toolkitMask.name.AsCharPtr());
	}

	// hide UI showing skins
	this->ui->skinsFrame->setVisible(false);

	if (this->skins.Size() > 0)
	{
		// first add the first skin to our list of visible skins
		this->ui->visibleSkins->addItem(skins[0].AsString().AsCharPtr());

		// go through skins and add to available list
		IndexT i;
		for (i = 1; i < skins.Size(); i++)
		{
			this->ui->availableSkins->addItem(this->skins[i].AsString().AsCharPtr());

			// show all skins
			Ptr<ShowSkin> showMsg = ShowSkin::Create();
			showMsg->SetSkin(skins[i]);
			__Send(this->model, showMsg);
		}		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnFetchedClipList(const Ptr<Messaging::Message>& msg)
{
	const Ptr<FetchClips>& clipMsg = msg.downcast<FetchClips>();

	// then copy list to our clips list
	this->clips = clipMsg->GetClips();
	this->durations = clipMsg->GetLenghts();
	this->clipsPlaying.Resize(this->clips.Size());
	this->clipsPlaying.Fill(false);

	this->clipWeights.Resize(this->clips.Size());
	this->clipWeights.Fill(1);

	this->clipsMask.Resize(this->clips.Size());
	this->clipsMask.Fill(0);

    // resize current clip array
    this->currentDurations.Resize(this->clips.Size());
    this->currentDurations.Fill(0);

	// go through clips and add them
	IndexT i;
	for (i = 0; i < this->clips.Size(); i++)
	{
		this->ui->clipsList->addItem(this->clips[i].AsString().AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnNewMask()
{
	ToolkitUtil::JointMask newMask;
	newMask.weights.Resize(this->joints.Size());
	newMask.weights.Fill(1);
	newMask.name = "<new mask>";
	this->masks.Append(newMask);
	if (this->masks.Size() == 1)
	{
		this->currentMask = &this->masks[0];
		this->ui->maskName->setEnabled(true);
	}

	this->ui->maskBox->addItem(newMask.name.AsCharPtr());
	this->ui->maskBox->setCurrentIndex(this->ui->maskBox->count() - 1);
	this->ui->maskName->setText(newMask.name.AsCharPtr());

	// tell model we have a change
	this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnDeleteMask()
{
	this->masks.EraseIndex(this->ui->maskBox->currentIndex());
	this->ui->maskBox->removeItem(this->ui->maskBox->currentIndex());

	// tell model we have a change
	this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnMaskNameChanged(const QString& value)
{
	this->currentMask->name = value.toUtf8().constBegin();
	this->ui->maskBox->setItemText(this->ui->maskBox->currentIndex(), value);

	// tell model we have a change
	this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnMaskSelected(int selected)
{
	// if we remove items, we will automatically select the next one
	if (!this->masks.IsEmpty())
	{
		this->currentMask = &this->masks[selected];
		this->ui->maskName->setText(this->currentMask->name.AsCharPtr());
		this->ui->maskName->setEnabled(true);
	}	
	else
	{
		this->currentMask = 0;
		this->ui->maskName->setEnabled(false);
		this->ui->maskName->setText("");
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnFrame()
{
	//ImGui::NewFrame();
	ImGui::Begin("Character", NULL, ImVec2(0, 0), 0.8f);
	if (ImGui::TreeNode("Skins"))
	{
		IndexT i;
		for (i = 0; i < this->skins.Size(); i++)
		{
			bool selected = this->skinVisible[i];
			ImGui::Checkbox(this->skins[i].AsString().AsCharPtr(), &selected);
			if (i < this->skins.Size() - 1) ImGui::SameLine();
			if (selected != this->skinVisible[i])
			{
				if (!selected)
				{
					Ptr<HideSkin> hideMsg = HideSkin::Create();
					hideMsg->SetSkin(this->skins[i]);
					__Send(this->model, hideMsg);
				}
				else
				{
					// show all skins
					Ptr<ShowSkin> showMsg = ShowSkin::Create();
					showMsg->SetSkin(this->skins[i]);
					__Send(this->model, showMsg);
				}
			}
			this->skinVisible[i] = selected;
		}
		ImGui::TreePop();
	}	
	if (ImGui::TreeNode("Animations"))
	{
		bool isFirstClipPlaying = true;
		IndexT i;
		for (i = 0; i < this->clips.Size(); i++)
		{
			bool selected = this->clipsPlaying[i];
			ImGui::Checkbox(this->clips[i].AsString().AsCharPtr(), &selected);
			if (selected)
			{
				this->numFrames = Math::n_max(this->numFrames, this->durations[i]);

				if (!isFirstClipPlaying)
				{
					ImGui::PushID(i);
					ImGui::PushItemWidth(100);

					int item = this->clipsMask[i];
					const char** names = new const char*[this->characterJointMasks.Size() + 1];
					names[0] = "None";
					IndexT j;
					for (j = 0; j < this->characterJointMasks.Size(); j++)
					{
						names[j + 1] = this->characterJointMasks[j]->GetName().Value();
					}
					//ImGui::SameLine(ImGui::GetWindowWidth() - 360);
					ImGui::SameLine(ImGui::GetWindowWidth() - 360);
					ImGui::Combo("Mask", &item, names, this->characterJointMasks.Size() + 1);

					//ImGui::SameLine(ImGui::GetWindowWidth() - 200);
					ImGui::SameLine();
					float weight = this->clipWeights[i];
					ImGui::SliderFloat("Weight", &weight, 0, 1);
					ImGui::PopItemWidth();
					ImGui::PopID();

					if (weight != this->clipWeights[i])
					{
						Ptr<AnimModifyBlendWeight> msg = AnimModifyBlendWeight::Create();
						msg->SetBlendWeight(weight);
						msg->SetTrackIndex(i);
						__Send(this->model, msg);
						this->clipWeights[i] = weight;
					}

					if (item != this->clipsMask[i])
					{
						Ptr<AnimModifyJointMask> msg = AnimModifyJointMask::Create();
						msg->SetJointMask(names[item]);
						msg->SetTrackIndex(i);
						__Send(this->model, msg);
						this->clipsMask[i] = item;
					}
					delete[] names;
				}

				if (selected != this->clipsPlaying[i])
				{				
					// create message
					Ptr<AnimPlayClip> msg = AnimPlayClip::Create();
					msg->SetClipName(clips[i]);
					msg->SetTrackIndex(i);
					msg->SetBlendWeight(1);
					msg->SetLoopCount(0);
					__Send(this->model, msg);
					this->paused = false;
				}
				isFirstClipPlaying = false;
			}
			else if (selected != this->clipsPlaying[i])
			{
				Ptr<AnimStopTrack> msg = AnimStopTrack::Create();
				msg->SetTrackIndex(i);
				__Send(this->model, msg);
				this->numFrames = 0;
				this->clipsMask[i] = 0;
			}
			this->clipsPlaying[i] = selected;
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Masks"))
	{
		//int selected = -1;
		ImGui::RadioButton("None", &this->selectedMask, -1);
		IndexT i;
		for (i = 0; i < this->characterJointMasks.Size(); i++)
		{
			ImGui::RadioButton(this->characterJointMasks[i]->GetName().Value(), &this->selectedMask, i);
		}

		if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) selectedJoint = -1;
		if (this->selectedMask >= 0)
		{
			// a bit lazy but whatever
			const Ptr<CameraEntity>& cam = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
			const ImGuiIO& io = ImGui::GetIO();

			IndexT j;
			for (j = 0; j < this->joints.Size(); j++)
			{
				const Characters::CharacterJoint& joint = this->joints[j];
				Math::point pos = joint.GetPoseMatrix().get_position();
				Math::float2 screenPos = cam->CalculateScreenSpacePosition(pos);
				Util::StringAtom name = joint.GetName();

				float mask = this->characterJointMasks[this->selectedMask]->GetWeight(j);
				if (j == selectedJoint)
				{
					ImGui::Begin(name.Value(), NULL, ImVec2(), 0.1f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);										
					ImGui::SetWindowPos(ImVec2(screenPos.x() * io.DisplaySize.x - ImGui::GetWindowSize().x / 2, screenPos.y() * io.DisplaySize.y - ImGui::GetWindowSize().y / 2));
					ImGui::PushItemWidth(100);
					ImGui::SliderFloat("Mask", &mask, 0, 1);
					ImGui::PopItemWidth();
					ImGui::End();
					if (mask != this->characterJointMasks[this->selectedMask]->GetWeight(j))
					{
						// set mask in character and in resource in parallel...
						this->characterJointMasks[this->selectedMask]->SetWeight(j, mask);
						this->masks[this->selectedMask].weights[j] = mask;

						// mark model as changed
						this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
						this->itemHandler->OnModelModified();
					}
				}
				else
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
					ImGui::SetNextWindowSize(ImVec2(100, 20));
					ImGui::Begin(name.Value(), NULL, ImVec2(), 0.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
					ImGui::SetWindowPos(ImVec2(screenPos.x() * io.DisplaySize.x - ImGui::GetWindowSize().x / 2, screenPos.y() * io.DisplaySize.y - ImGui::GetWindowSize().y / 2));
					if (ImGui::Button(name.Value(), ImVec2(100, 20))) this->selectedJoint = j;
					ImGui::SetWindowSize(ImGui::GetItemRectSize());
					ImGui::End();
					ImGui::PopStyleVar(2);
				}
			}
		}
		else
		{
			// new mask, selected the 0 joint
			selectedJoint = -1;
		}
		ImGui::TreePop();
	}
	int frame = this->currentFrame;
	ImGui::SliderInt("Seek", &frame, 0, this->numFrames);
	if (frame != this->currentFrame)
	{
		// if we start seeking, just stop animations
		Ptr<AnimPauseAllTracks> msg2 = AnimPauseAllTracks::Create();
		msg2->SetPause(true);
		__Send(model, msg2);
		this->paused = true;

		// then seek to wanted time
		Ptr<AnimSeek> msg = AnimSeek::Create();
		msg->SetTime(frame);
		__Send(model, msg);
		this->currentFrame = frame;
	}
	
	if (ImGui::Button("Pause/Unpause"))
	{
		// pause tracks with flipped bool
		Ptr<AnimPauseAllTracks> msg = AnimPauseAllTracks::Create();
		msg->SetPause(!this->paused);
		__Send(model, msg);

		this->paused = !this->paused;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		// just seek to the start, but dont pause animations
		Ptr<AnimSeek> msg = AnimSeek::Create();
		msg->SetTime(0);
		__Send(model, msg);
		this->currentFrame = 0;
	}
	ImGui::End();
}

} // namespace Widgets