//------------------------------------------------------------------------------
//  streamsurfacematerialsaver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "streamsurfacematerialsaver.h"
#include "surfacematerial.h"
#include "io/xmlwriter.h"

namespace Materials
{
__ImplementClass(Materials::StreamSurfaceMaterialSaver, 'SSMS', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
StreamSurfaceMaterialSaver::StreamSurfaceMaterialSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
StreamSurfaceMaterialSaver::~StreamSurfaceMaterialSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamSurfaceMaterialSaver::OnSave()
{
	n_assert(this->stream.isvalid());
	const Ptr<SurfaceMaterial>& sur = this->resource.downcast<SurfaceMaterial>();

	Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	writer->SetStream(this->stream);
	if (writer->Open())
	{
		writer->BeginNode("NebulaT");
			writer->BeginNode("Surface");
				writer->SetString("template", sur->GetMaterialTemplate()->GetName().AsString());
				IndexT i;
				for (i = 0; i < sur->constantsByName.Size(); i++)
				{
					const Util::KeyValuePair<Util::StringAtom, Ptr<Materials::SurfaceConstant>>& pair = sur->constantsByName.KeyValuePairAtIndex(i);
					writer->BeginNode("Param");
						writer->SetString("name", pair.Key().AsString());
						const Util::Variant& val = pair.Value()->GetValue();

						// assume its texture if variant is object
						if (val.GetType() != Util::Variant::Object) writer->SetString("value", pair.Value()->GetValue().ToString());
						else
						{
							Ptr<CoreGraphics::Texture> tex = (CoreGraphics::Texture*)val.GetObject();
							writer->SetString("value", tex->GetResourceId().AsString());
						}
						
					writer->EndNode();
				}
			writer->EndNode();
		writer->EndNode();
	}
	else
	{
		n_error("Could not open '%s' for writing", stream->GetURI().LocalPath().AsCharPtr());
		return false;
	}
	return true;
}

} // namespace Materials