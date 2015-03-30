//------------------------------------------------------------------------------
//  imguirenderer.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imguirenderer.h"
#include "imgui/imgui.h"
#include "coregraphics/memorytextureloader.h"
#include "resources/resourcemanager.h"
#include "SOIL/SOIL.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "math/rectangle.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/displaydevice.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Base;
using namespace Input;



namespace Dynui
{

//------------------------------------------------------------------------------
/**
	Imgui rendering function
*/
static void
ImguiDrawFunction(ImDrawList** const commandLists, int numCommandLists)
{
	// get Imgui context
	ImGuiIO& io = ImGui::GetIO();

	// get rendering device
	const Ptr<RenderDevice>& device = RenderDevice::Instance();

	// get renderer
	const Ptr<ImguiRenderer>& renderer = ImguiRenderer::Instance();
	const Ptr<ShaderInstance>& shader = renderer->GetShader();
	const Ptr<BufferLock>& bufferLock = renderer->GetBufferLock();
	const Ptr<VertexBuffer>& vbo = renderer->GetVertexBuffer();
	const ImguiRendererParams& params = renderer->GetParams();

	// apply shader
	shader->Begin();
	shader->BeginPass(0);

	// create orthogonal matrix
	matrix44 proj = matrix44::orthooffcenterrh(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);

	// set in shader
	params.projVar->SetMatrix(proj);

	// commit variable changes
	shader->Commit();

	// setup device
	device->SetStreamSource(0, vbo, 0);
	device->SetVertexLayout(vbo->GetVertexLayout());

	IndexT vertexOffset = 0;
	IndexT bufferOffset = 0;
	IndexT i;
	for (i = 0; i < numCommandLists; i++)
	{
		const ImDrawList* commandList = commandLists[i];
		const unsigned char* buffer = (const unsigned char*)commandList->vtx_buffer.begin();
		const SizeT size = commandList->vtx_buffer.size() * (sizeof(float) * 4 + sizeof(int));					// 2 for position, 2 for uvs, 1 int for color

		bufferLock->WaitForRange(bufferOffset, size);
		memcpy(renderer->GetVertexPtr() + bufferOffset, buffer, size);
		for (ImVector<ImDrawCmd>::const_iterator command = commandList->commands.begin(); command != commandList->commands.end(); command++)
		{
			// if we render too many vertices, we will simply assert
			n_assert(vertexOffset + (IndexT)command->vtx_count < vbo->GetNumVertices() * vbo->GetBufferCount());

			// setup scissor rect
			Math::rectangle<int> scissorRect((int)command->clip_rect.x, (int)command->clip_rect.y, (int)command->clip_rect.z, (int)command->clip_rect.w);
			device->SetScissorRect(scissorRect, 0);

			// setup primitive
			CoreGraphics::PrimitiveGroup primitive;
			primitive.SetBaseVertex(vertexOffset);
			primitive.SetNumVertices(command->vtx_count);
			primitive.SetPrimitiveTopology(CoreGraphics::PrimitiveTopology::TriangleList);

			// prepare render device and draw
			device->SetPrimitiveGroup(primitive);
			device->Draw();

			// run shader post-draw
			shader->PostDraw();

			// increment vertex offset
			vertexOffset += command->vtx_count;
		}

		bufferLock->LockRange(bufferOffset, size);
		bufferOffset += size;
	}

	// finish rendering
	shader->EndPass();
	shader->End();
}

__ImplementClass(Dynui::ImguiRenderer, 'IMRE', Core::RefCounted);
__ImplementSingleton(Dynui::ImguiRenderer);

//------------------------------------------------------------------------------
/**
*/
ImguiRenderer::ImguiRenderer()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ImguiRenderer::~ImguiRenderer()
{
	__DestructSingleton
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiRenderer::Setup()
{
	// allocate imgui shader
	this->uiShader = ShaderServer::Instance()->CreateShaderInstance("shd:imgui");
	this->params.projVar = this->uiShader->GetVariableByName("TextProjectionModel");
	this->params.fontVar = this->uiShader->GetVariableByName("Texture");

	// create vertex buffer
	Util::Array<CoreGraphics::VertexComponent> components;
	components.Append(VertexComponent(VertexComponentBase::Position, 0, VertexComponentBase::Float2, 0));
	components.Append(VertexComponent(VertexComponentBase::TexCoord, 0, VertexComponentBase::Float2, 0));
	components.Append(VertexComponent(VertexComponentBase::Color,	 0, VertexComponentBase::UByte4N, 0));
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(components, 10000, NULL, 0, ResourceBase::UsageDynamic, ResourceBase::AccessWrite, ResourceBase::BufferTriple, ResourceBase::SyncingCoherentPersistent);

	// load buffer
	this->vbo = VertexBuffer::Create();
	this->vbo->SetLoader(vboLoader.upcast<Resources::ResourceLoader>());
	this->vbo->SetAsyncEnabled(false);
	this->vbo->Load();
	n_assert(this->vbo->IsLoaded());
	this->vbo->SetLoader(NULL);	

	// map buffer
	this->vertexPtr = (byte*)this->vbo->Map(ResourceBase::MapWrite);

	// create buffer lock
	this->bufferLock = BufferLock::Create();

	// get display mode, this will be our default size
	Ptr<DisplayDevice> display = DisplayDevice::Instance();
	DisplayMode mode = display->GetDisplayMode();

	// setup Imgui
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)mode.GetWidth(), (float)mode.GetHeight());
	io.DeltaTime = 1 / 60.0f;
	io.PixelCenterOffset = 0.0f;
	//io.FontTexUvForWhite = ImVec2(1, 1);
	io.RenderDrawListsFn = ImguiDrawFunction;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	ImVec4 nebulaOrange(219.0f / 255.0f, 127.0f / 255.0f, 0.0f, 1.0f);
	nebulaOrange.w = 115 / 255.0f;
	style.Colors[ImGuiCol_TitleBg] = nebulaOrange;
	nebulaOrange.w = 51 / 255.0f;
	style.Colors[ImGuiCol_TitleBgCollapsed] = nebulaOrange;
	nebulaOrange.w = 38 / 255.0f;
	style.Colors[ImGuiCol_ScrollbarBg] = nebulaOrange;
	nebulaOrange.w = 77 / 255.0f;
	style.Colors[ImGuiCol_ScrollbarGrab] = nebulaOrange;
	nebulaOrange.w = 102 / 255.0f;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = nebulaOrange;
	nebulaOrange.w = 115 / 255.0f;
	nebulaOrange.x = 228 / 255.0f;
	style.Colors[ImGuiCol_Header] = nebulaOrange;
	nebulaOrange.w = 204 / 255.0f;
	nebulaOrange.x = 183 / 255.0f;
	style.Colors[ImGuiCol_HeaderHovered] = nebulaOrange;
	nebulaOrange.w = 255 / 255.0f;
	nebulaOrange.x = 173 / 255.0f;
	style.Colors[ImGuiCol_HeaderActive] = nebulaOrange;
	nebulaOrange.w = 128 / 255.0f;
	style.Colors[ImGuiCol_CloseButton] = nebulaOrange;
	nebulaOrange.x = 183 / 255.0f;
	nebulaOrange.w = 153 / 255.0f;
	style.Colors[ImGuiCol_CloseButtonHovered] = nebulaOrange;
	nebulaOrange.x = 225 / 255.0f;
	style.Colors[ImGuiCol_Border] = nebulaOrange;

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_Tab] = Key::Tab;             
	io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
	io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
	io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
	io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
	io.KeyMap[ImGuiKey_Home] = Key::Home;
	io.KeyMap[ImGuiKey_End] = Key::End;
	io.KeyMap[ImGuiKey_Delete] = Key::Delete;
	io.KeyMap[ImGuiKey_Backspace] = Key::Back;
	io.KeyMap[ImGuiKey_Enter] = Key::Return;
	io.KeyMap[ImGuiKey_Escape] = Key::Escape;
	io.KeyMap[ImGuiKey_A] = Key::A;
	io.KeyMap[ImGuiKey_C] = Key::C;
	io.KeyMap[ImGuiKey_V] = Key::V;
	io.KeyMap[ImGuiKey_X] = Key::X;
	io.KeyMap[ImGuiKey_Y] = Key::Y;
	io.KeyMap[ImGuiKey_Z] = Key::Z;

	// start a new frame
	ImGui::NewFrame();

	// load default font
	const void* png_data;
	unsigned int png_size;
	ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);

	// load image using SOIL
	int width, height, channels;
	unsigned char* texData = SOIL_load_image_from_memory((unsigned const char*)png_data, png_size, &width, &height, &channels, SOIL_LOAD_AUTO);

	// setup texture
	this->fontTexture = Resources::ResourceManager::Instance()->CreateUnmanagedResource("ImguiFontTexture", Texture::RTTI).downcast<Texture>();
	Ptr<MemoryTextureLoader> texLoader = MemoryTextureLoader::Create();
	texLoader->SetImageBuffer(texData, width, height, PixelFormat::A8R8G8B8);
	this->fontTexture->SetLoader(texLoader.upcast<Resources::ResourceLoader>());
	this->fontTexture->SetAsyncEnabled(false);
	this->fontTexture->Load();
	n_assert(this->fontTexture->IsLoaded());
	this->fontTexture->SetLoader(0);

	// set texture in shader
	this->params.fontVar->SetTexture(this->fontTexture);
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiRenderer::Discard()
{
	this->uiShader->Discard();
	this->uiShader = 0;

	this->vbo->Unmap();
	this->vbo->Unload();
	this->vbo = 0;

	this->fontTexture->Unload();
	this->fontTexture = 0;
	ImGui::Shutdown();
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiRenderer::Render()
{
	// render ImGui
	ImGui::Render();
}

//------------------------------------------------------------------------------
/**
*/
bool
ImguiRenderer::HandleInput(const Input::InputEvent& event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (event.GetType())
	{
	case InputEvent::KeyDown:
		io.KeysDown[event.GetKey()] = true;
		if (event.GetKey() == Key::LeftControl || event.GetKey() == Key::RightControl) io.KeyCtrl = true;
		if (event.GetKey() == Key::LeftShift || event.GetKey() == Key::RightShift) io.KeyShift = true;
		return io.WantCaptureKeyboard;
	case InputEvent::KeyUp:
		io.KeysDown[event.GetKey()] = false;
		if (event.GetKey() == Key::LeftControl || event.GetKey() == Key::RightControl) io.KeyCtrl = false;
		if (event.GetKey() == Key::LeftShift || event.GetKey() == Key::RightShift) io.KeyShift = false;
		return false;									// not a bug, this allows keys to be let go even if we are over the UI
	case InputEvent::Character:
	{
		char c = event.GetChar();

		// ignore backspace as a character
		if (c > 0 && c < 0x10000)
		{
			io.AddInputCharacter((unsigned short)c);
		}
		return io.WantCaptureKeyboard;
	}
	case InputEvent::MouseMove:
		io.MousePos = ImVec2(event.GetAbsMousePos().x(), event.GetAbsMousePos().y());
		return io.WantCaptureMouse;
	case InputEvent::MouseButtonDoubleClick:
	case InputEvent::MouseButtonDown:
		io.MouseDown[event.GetMouseButton()] = true;
		return io.WantCaptureMouse;
	case InputEvent::MouseButtonUp:
		io.MouseDown[event.GetMouseButton()] = false;
		return false;									// not a bug, this allows keys to be let go even if we are over the UI
	case InputEvent::MouseWheelForward:
		io.MouseWheel = 1;
		return io.WantCaptureMouse;
	case InputEvent::MouseWheelBackward:
		io.MouseWheel = -1;
		return io.WantCaptureMouse;
	}
	
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ImguiRenderer::SetRectSize(SizeT width, SizeT height)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
}

} // namespace Dynui