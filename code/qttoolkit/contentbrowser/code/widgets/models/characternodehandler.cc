//------------------------------------------------------------------------------
//  characternodehandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
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
#include "characters/characterskeleton.h"
#include "characters/characterjointmask.h"

#include <QDoubleSpinBox>
#include "mutablecharacterskeleton.h"
#include "graphicsfeatureunit.h"

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
	numFrames(0),
	currentFrame(0),
	currentMask(0),
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
	this->ui->maskBox->setEnabled(false);
	this->ui->maskName->setEnabled(false);

	// now fetch skins
	Ptr<FetchSkinList> skinMsg = FetchSkinList::Create();
    __Send(this->model, skinMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedSkinList, this, skinMsg.upcast<Messaging::Message>());
	
	// do the same or clips
	Ptr<FetchClips> clipMsg = FetchClips::Create();
	__Send(this->model, clipMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedClipList, this, clipMsg.upcast<Messaging::Message>());

	// connect UI used for joint mask management
	connect(this->ui->newMaskButton, SIGNAL(clicked()), this, SLOT(OnNewMask()));
	connect(this->ui->deleteMaskButton, SIGNAL(clicked()), this, SLOT(OnDeleteMask()));
	connect(this->ui->maskName, SIGNAL(editingFinished()), this, SLOT(OnMaskNameChanged()));
	connect(this->ui->maskBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaskSelected(int)));

	// connect handler for joint tree
	connect(this->ui->jointTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnJointRowClicked(QTreeWidgetItem*, int)));
	connect(this->ui->jointTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnJointRowItemContextMenu(const QPoint&)));
	this->ui->jointTree->header()->setResizeMode(QHeaderView::ResizeToContents);
	this->ui->jointTree->setContextMenuPolicy(Qt::CustomContextMenu);

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
    this->currentDurations.Clear();
	this->skinVisible.Clear();
	this->clipsPlaying.Clear();
	this->clipsMask.Clear();
	this->clipWeights.Clear();
	this->joints.Clear();
	this->masks.Clear();
	this->characterJointMasks.Clear();

	disconnect(this->ui->newMaskButton, SIGNAL(clicked()));
	disconnect(this->ui->deleteMaskButton, SIGNAL(clicked()));
	disconnect(this->ui->maskName, SIGNAL(editingFinished()));
	disconnect(this->ui->maskBox, SIGNAL(currentIndexChanged()));
	disconnect(this->ui->jointTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)));
	disconnect(this->ui->jointTree, SIGNAL(customContextMenuRequested(const QPoint&)));

	// make sure this object doesn't get called if the handler has been destroyed
	__AbortSingleFireCallbacks(CharacterNodeHandler, this);
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
	this->skeleton = &this->model->GetCharacter()->Skeleton();
	this->joints.Resize(this->skeleton->GetNumJoints());
	IndexT i;
	for (i = 0; i < this->joints.Size(); i++)
	{
		this->joints[i] = this->skeleton->GetJoint(i);
	}
	this->SetupJointHierarchy();

	// get masks from character
	SizeT numMasks = this->model->GetCharacter()->Skeleton().GetNumMasks();
	if (numMasks > 0)
	{
		this->ui->maskBox->setEnabled(true);
		this->ui->maskName->setEnabled(true);
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
}

//------------------------------------------------------------------------------
/**
	After this function is executed, this instance of the node handler is destroyed
*/
void
CharacterNodeHandler::OnNewMask()
{
	Util::String maskName = "<new mask>";
	IndexT i = 0;
	while (this->skeleton->GetMaskIndexByName(maskName) != InvalidIndex) maskName = maskName + Util::String::FromInt(i++);

	ToolkitUtil::JointMask newMask;
	newMask.weights.Resize(this->joints.Size());
	newMask.weights.Fill(1);
	newMask.name = maskName;

	Characters::CharacterJointMask gfxMask;
	gfxMask.SetName(maskName);
	gfxMask.SetWeights(newMask.weights);

	// add both the resource and directly to the graphics system
	this->skeleton->AddJointMask(gfxMask);
	this->masks.Append(newMask);
	this->characterJointMasks.Append(this->skeleton->GetMask(this->masks.Size() - 1));

	// tell model we have a change
	this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
	this->itemHandler->OnModelModified();

	
	this->ui->maskBox->addItem(newMask.name.AsCharPtr());
	this->ui->maskName->setText(newMask.name.AsCharPtr());
	this->ui->maskBox->setCurrentIndex(this->ui->maskBox->count() - 1);
}

//------------------------------------------------------------------------------
/**
	After this function is executed, this instance of the node handler is destroyed
*/
void
CharacterNodeHandler::OnDeleteMask()
{
	// disable joint mask if it is the one we have removed
	IndexT i;
	for (i = 0; i < this->clips.Size(); i++)
	{
		if (this->clipsMask[i] == this->ui->maskBox->currentIndex() + 1)
		{
			Ptr<AnimSetJointMask> msg = AnimSetJointMask::Create();
			msg->SetJointMask("");
			msg->SetTrackIndex(i);
			__Send(this->model, msg);
			this->clipsMask[i] = 0;
		}
	}

	// remove from lists
	static_cast<MutableCharacterSkeleton*>(this->skeleton)->EraseMask(this->characterJointMasks[this->ui->maskBox->currentIndex()]);
	this->masks.EraseIndex(this->ui->maskBox->currentIndex());
	this->characterJointMasks.EraseIndex(this->ui->maskBox->currentIndex());
	this->ui->maskBox->removeItem(this->ui->maskBox->currentIndex());

	// tell model we have a change
	this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnMaskNameChanged()
{
	const QString& text = this->ui->maskName->text();
	this->ui->maskBox->setItemText(this->ui->maskBox->currentIndex(), text);
	this->masks[this->ui->maskBox->currentIndex()].name = text.toUtf8().constData();
	Characters::CharacterJointMask* mask = this->characterJointMasks[this->ui->maskBox->currentIndex()];
	static_cast<MutableCharacterSkeleton*>(this->skeleton)->RenameMask(mask->GetName(), text.toUtf8().constData(), mask);
	
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
		this->selectedMask = selected;
		this->currentMask = &this->masks[selected];
		this->ui->maskName->setText(this->currentMask->name.AsCharPtr());
		this->ui->maskName->setEnabled(true);
		this->ui->maskBox->setEnabled(true);
		this->SetupJointWeights();
	}	
	else
	{
		this->currentMask = 0;
		this->ui->maskName->setEnabled(false);
		this->ui->maskBox->setEnabled(false);
		this->ui->maskName->setText("");
		this->SetupJointHierarchy();
	}
}

static int jointidx = 0;
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
		int prevJointIdx = jointidx;
		ImGui::InputInt("Test joint index", &jointidx);
		if (ImGui::Button("Test Mix anim"))
		{
			Ptr<Graphics::SetSkeletonEvalMode> eval = Graphics::SetSkeletonEvalMode::Create();
			eval->SetMode(Characters::CharacterSkeletonInstance::Mix);
			__Send(this->model, eval);
		}

		if (prevJointIdx != jointidx)
		{
			Ptr<Graphics::SetSkeletonJointMatrix> joint = Graphics::SetSkeletonJointMatrix::Create();
			joint->SetIndex(prevJointIdx);
			joint->SetTransform(Math::matrix44::identity());
			__Send(this->model, joint);
		}
		{
			Ptr<Graphics::SetSkeletonJointMatrix> joint = Graphics::SetSkeletonJointMatrix::Create();
			Math::float4 pos = this->joints[jointidx].GetPoseMatrix().get_position();
			Math::float4 at = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform().get_position();
			Math::matrix44 trans = Math::matrix44::lookatrh(pos, at, Math::vector::upvec());
			trans.set_position(Math::point(0));
			joint->SetTransform(trans);
			joint->SetIndex(jointidx);
			__Send(this->model, joint);
		}

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
						Ptr<AnimSetJointMask> msg = AnimSetJointMask::Create();
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
	if (this->ui->displayJoints->isChecked())
	{
		// a bit lazy but whatever
		const Ptr<CameraEntity>& cam = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
		const ImGuiIO& io = ImGui::GetIO();

		IndexT j;
		for (j = 0; j < this->joints.Size(); j++)
		{
			ImVec4 color;
			if (this->currentMask)
			{
				float weight = this->currentMask->weights[j];
				color = ImVec4(Math::n_lerp(color.x, 1.0f, 1-weight), Math::n_lerp(color.y, 1.0f, weight), color.z, 1.0f);
			}
			const Characters::CharacterJoint& joint = this->joints[j];
			Math::point pos = joint.GetPoseMatrix().get_position();
			Math::float2 screenPos = cam->CalculateScreenSpacePosition(pos);
			Util::StringAtom name = joint.GetName();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(100, 20));
			ImGui::Begin(name.Value(), NULL, ImVec2(), 0.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			ImGui::SetWindowPos(ImVec2(screenPos.x() * io.DisplaySize.x - ImGui::GetWindowSize().x / 2, screenPos.y() * io.DisplaySize.y - ImGui::GetWindowSize().y / 2));
			ImGui::TextColored(color, joint.GetName().Value());
			ImGui::SetWindowSize(ImGui::GetItemRectSize());
			ImGui::End();
			ImGui::PopStyleVar(2);
		}
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

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnJointRowClicked(QTreeWidgetItem* item, int column)
{
	QObject* obj = this->sender();
	if (column == 1 && this->currentMask != 0)
	{
		Characters::CharacterJoint* joint = (Characters::CharacterJoint*)item->data(column, Qt::ItemDataRole::UserRole).constData();
		const int index = item->data(0, Qt::ItemDataRole::UserRole).toInt();

		QDialog dialog;
		dialog.setModal(true);
		dialog.setWindowFlags(Qt::FramelessWindowHint);

		QVBoxLayout layout;
		dialog.setLayout(&layout);
		layout.setContentsMargins(QMargins(0, 0, 0, 0));

		QDoubleSpinBox box;
		box.setRange(0, 1);
		box.setSingleStep(0.01);
		layout.addWidget(&box);
		box.setValue(this->currentMask->weights[index]);

		//QShortcut shortcut(&dialog);
		//shortcut.setKey(Qt::Key_Enter);
		//connect(&shortcut, SIGNAL(activated()), &dialog, SLOT(accept()));

		connect(&box, SIGNAL(editingFinished()), &dialog, SLOT(accept()));
		QSize size = item->sizeHint(column);
		bool expanded = item->isExpanded();
		QWidget* widget = this->ui->jointTree->itemWidget(item, column);
		this->ui->jointTree->setItemWidget(item, column, &dialog);

		int result = dialog.exec();
		if (result == QDialog::Accepted)
		{
			// only update if it has changed
			if (this->currentMask->weights[index] != box.value())
			{
				// mark model as changed
				this->masks[this->selectedMask].weights[index] = box.value();
				this->characterJointMasks[this->selectedMask]->SetWeight(index, box.value());
				this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
				this->itemHandler->OnModelModified();
			}

			// reset item state
			this->ui->jointTree->setItemWidget(item, column, widget);
			item->setText(1, QString::number(box.value()));
			item->setSizeHint(column, size);
			item->setExpanded(expanded);
		}
	}
	else
	{
		item->setExpanded(!item->isExpanded());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::OnJointRowItemContextMenu(const QPoint& point)
{
	if (this->currentMask == 0) return;

	QMenu menu;
	QAction* action1 = menu.addAction("Flood up");
	QAction* action2 = menu.addAction("Flood down");
	QTreeWidgetItem* item = this->ui->jointTree->itemAt(point);

	QPoint globalPos = this->ui->jointTree->mapToGlobal(point);
	QAction* selectedAction = menu.exec(globalPos);
	if (selectedAction != 0)
	{
		QDialog dialog;
		dialog.setWindowModality(Qt::WindowModal);
		dialog.setWindowFlags(Qt::FramelessWindowHint);

		QVBoxLayout layout;
		dialog.setLayout(&layout);
		layout.setContentsMargins(QMargins(0, 0, 0, 0));

		QFrame frame;
		layout.addWidget(&frame);
		frame.setFrameShadow(QFrame::Plain);
		frame.setFrameShape(QFrame::Box);
		
		QVBoxLayout frameLayout;
		frame.setLayout(&frameLayout);
		frame.setContentsMargins(QMargins(1, 1, 1, 1));

		QDoubleSpinBox box;
		box.setRange(0, 1);
		box.setSingleStep(0.1);
		frameLayout.addWidget(&box);
		box.setValue(0.0f);
		connect(&box, SIGNAL(editingFinished()), &dialog, SLOT(accept()));
		dialog.move(globalPos);

		int result = dialog.exec();
		if (result == QDialog::Accepted)
		{
			if (selectedAction == action1)
			{	
				// update joint mask upwards
				this->RecurseFloodJointMaskUp(item, box.value());

				// we have changes, so update the state
				this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
				this->itemHandler->OnModelModified();
			}
			else if (selectedAction == action2)
			{
				// update joint mask downwards
				this->RecurseFloodJointMaskDown(item, box.value());

				// we have changes, so update the state
				this->itemHandler->GetAttributes()->SetJointMasks(this->masks);
				this->itemHandler->OnModelModified();
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::SetupJointHierarchy()
{
	this->ui->jointTree->clear();
	this->jointIndexToItemMap.Clear();
	IndexT i;
	for (i = 0; i < this->joints.Size(); i++)
	{
		const Characters::CharacterJoint& joint = this->joints[i];
		QTreeWidgetItem* newItem = new QTreeWidgetItem;
		newItem->setData(1, Qt::ItemDataRole::UserRole, &joint);
		newItem->setData(0, Qt::ItemDataRole::UserRole, i);
		newItem->setText(0, joint.GetName().Value());
		newItem->setText(1, "N/A");
		if (this->jointIndexToItemMap.Contains(joint.GetParentJointIndex()))
		{
			QTreeWidgetItem* item = this->jointIndexToItemMap[joint.GetParentJointIndex()];
			item->addChild(newItem);
		}
		else
		{
			this->ui->jointTree->addTopLevelItem(newItem);
		}
		this->jointIndexToItemMap.Add(i, newItem);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::SetupJointWeights()
{
	IndexT i;
	for (i = 0; i < this->joints.Size(); i++)
	{
		const Characters::CharacterJoint& joint = this->joints[i];
		float weight = this->currentMask->weights[i];
		QTreeWidgetItem* item = this->jointIndexToItemMap[i];
		item->setText(1, QString::number(weight));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::RecurseFloodJointMaskDown(QTreeWidgetItem* item, float value)
{
	// get joint and index
	Characters::CharacterJoint* joint = (Characters::CharacterJoint*)item->data(1, Qt::ItemDataRole::UserRole).constData();
	const int index = item->data(0, Qt::ItemDataRole::UserRole).toInt();

	// update UI and mask
	item->setText(1, QString::number(value));
	this->masks[this->selectedMask].weights[index] = value;
	this->characterJointMasks[this->selectedMask]->SetWeight(index, value);

	// recurse to children
	int numchildren = item->childCount();
	for (int i = 0; i < numchildren; i++)
	{
		this->RecurseFloodJointMaskDown(item->child(i), value);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeHandler::RecurseFloodJointMaskUp(QTreeWidgetItem* item, float value)
{
	while (item->parent() != 0)
	{
		// get joint and index
		Characters::CharacterJoint* joint = (Characters::CharacterJoint*)item->data(1, Qt::ItemDataRole::UserRole).constData();
		const int index = item->data(0, Qt::ItemDataRole::UserRole).toInt();

		// update UI and mask
		item->setText(1, QString::number(value));
		this->masks[this->selectedMask].weights[index] = value;
		this->characterJointMasks[this->selectedMask]->SetWeight(index, value);

		// set to next parent
		item = item->parent();
	}
}

} // namespace Widgets