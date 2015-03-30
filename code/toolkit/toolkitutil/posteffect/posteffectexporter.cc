#include "stdneb.h"
#include "posteffectexporter.h"
#include "posteffectparser.h"
#include "db/dbfactory.h"
#include "base/exporterbase.h"
#include "db/database.h"
#include "attr/attrid.h"
#include "game/templateexporter.h"
#include "io/ioserver.h"

using namespace Db;
using namespace IO;
using namespace ToolkitUtil;
using namespace Attr;

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::PostEffectExporter, 'PEEX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PostEffectExporter::PostEffectExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PostEffectExporter::~PostEffectExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectExporter::Open()
{

	Base::ExporterBase::Open();
	if(!this->dbFactory.isvalid())
	{
		this->dbFactory = DbFactory::Instance();
	}
	n_assert(this->dbFactory);

	this->staticDb = DbFactory::Instance()->CreateDatabase();
	this->staticDb->SetURI(URI("db:static.db4"));
	this->staticDb->SetAccessMode(Database::ReadWriteCreate);
	this->staticDb->SetIgnoreUnknownColumns(true);

	Util::String s;
	s.Format("Could not open static database: %s", this->staticDb->GetURI().GetHostAndLocalPath().AsCharPtr());
	n_assert2(this->staticDb->Open(), s.AsCharPtr());


}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectExporter::Close()
{
	this->dbFactory = 0;
	this->staticDb->Close();
	this->staticDb = 0;
	ExporterBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectExporter::SetupTables()
{

	// make sure all required attributes exist

	if (!AttrId::IsValidName("GlobalLightTransform"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightTransform", Util::FourCC(), Matrix44Type, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightAmbient"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightAmbient", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightDiffuse"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightDiffuse", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightOpposite"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightOpposite", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightCastShadows"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightCastShadows", Util::FourCC(), BoolType, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightShadowIntensity"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightShadowIntensity", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightIntensity"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightIntensity", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("GlobalLightBacklightFactor"))
		AttributeDefinitionBase::RegisterDynamicAttribute("GlobalLightBacklightFactor", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("Saturation"))
		AttributeDefinitionBase::RegisterDynamicAttribute("Saturation", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("Balance"))
		AttributeDefinitionBase::RegisterDynamicAttribute("Balance", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("Luminance"))
		AttributeDefinitionBase::RegisterDynamicAttribute("Luminance", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("MaxLuminance"))
		AttributeDefinitionBase::RegisterDynamicAttribute("MaxLuminance", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FocusDistance"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FocusDistance", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FocusLength"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FocusLength", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FocusRadius"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FocusRadius", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FogHeight"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FogHeight", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FogColor"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FogColor", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("FogNearDist"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FogNearDist", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("FogFarDist"))
		AttributeDefinitionBase::RegisterDynamicAttribute("FogFarDist", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("BackLightFactor"))
		AttributeDefinitionBase::RegisterDynamicAttribute("BackLightFactor", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("BloomColor"))
		AttributeDefinitionBase::RegisterDynamicAttribute("BloomColor", Util::FourCC(), Float4Type, ReadWrite);
	if (!AttrId::IsValidName("BloomThreshold"))
		AttributeDefinitionBase::RegisterDynamicAttribute("BloomThreshold", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("BloomScale"))
		AttributeDefinitionBase::RegisterDynamicAttribute("BloomScale", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("SkyTexture"))
		AttributeDefinitionBase::RegisterDynamicAttribute("SkyTexture", Util::FourCC(), StringType, ReadWrite);
	if (!AttrId::IsValidName("SkyContrast"))
		AttributeDefinitionBase::RegisterDynamicAttribute("SkyContrast", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("SkyBrightness"))
		AttributeDefinitionBase::RegisterDynamicAttribute("SkyBrightness", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("SkyModel"))
		AttributeDefinitionBase::RegisterDynamicAttribute("SkyModel", Util::FourCC(), StringType, ReadWrite);
	if (!AttrId::IsValidName("AOStrength"))
		AttributeDefinitionBase::RegisterDynamicAttribute("AOStrength", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("AORadius"))
		AttributeDefinitionBase::RegisterDynamicAttribute("AORadius", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("AOPower"))
		AttributeDefinitionBase::RegisterDynamicAttribute("AOPower", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("AOAngleBias"))
		AttributeDefinitionBase::RegisterDynamicAttribute("AOAngleBias", Util::FourCC(), FloatType, ReadWrite);
	if (!AttrId::IsValidName("PEBlendTime"))
		AttributeDefinitionBase::RegisterDynamicAttribute("PEBlendTime", Util::FourCC(), FloatType, ReadWrite);	

	if (this->staticDb->HasTable("_PostEffect_Presets"))
	{
		this->staticDb->DeleteTable("_PostEffect_Presets");
	}
	
	{
		Ptr<Db::Table> table;
		Ptr<Db::Dataset> dataset;

		table = DbFactory::Instance()->CreateTable();
		table->SetName("_PostEffect_Presets");

		TemplateExporter::CreateColumn(table, Column::Primary, Attr::AttrId("Id"));

		this->staticDb->AddTable(table);
		dataset = table->CreateDataset();
		dataset->AddAllTableColumns();
		dataset->CommitChanges();

		AttrId id;		
		id = AttrId("GlobalLightTransform");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightAmbient");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightDiffuse");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightOpposite");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightCastShadows");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightShadowIntensity");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightIntensity");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightShadowBias");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("GlobalLightBacklightFactor");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("Saturation");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("Balance");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("Luminance");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("MaxLuminance");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FocusDistance");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FocusLength");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FocusRadius");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FogHeight");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FogColor");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FogNearDist");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("FogFarDist");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("BloomColor");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("BloomThreshold");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("BackLightFactor");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("BloomScale");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("SkyTexture");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("SkyContrast");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("SkyBrightness");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("SkyModel");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("SkyModel");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("AOStrength");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("AORadius");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("AOPower");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("AOAngleBias");
		TemplateExporter::CreateColumn(table, Column::Default, id);
		id = AttrId("PEBlendTime");
		TemplateExporter::CreateColumn(table, Column::Default, id);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectExporter::CheckDefaultPreset()
{
	Ptr< IO::IoServer> ioserver = IO::IoServer::Instance();
	if (!ioserver->FileExists("root:data/tables/posteffect/Default.xml"))
	{
		this->logger->Warning("No default preset found, creating...\n");
		PostEffect::PostEffectEntity::ParamSet parms;
		parms.Init();		
		if (!IO::IoServer::Instance()->DirectoryExists("root:data/tables/posteffect"))
		{
			IO::IoServer::Instance()->CreateDirectory("root:data/tables/posteffect");
		}
		PostEffectParser::Save("root:data/tables/posteffect/Default.xml", parms);
		parms.Discard();
	}	
}
//------------------------------------------------------------------------------
/**
*/
void
ToolkitUtil::PostEffectExporter::ExportAll()
{
	// check if default preset exist and create if it doesnt
	this->CheckDefaultPreset();

	Util::Array<Util::String> files = IO::IoServer::Instance()->ListFiles(IO::URI("root:data/tables/posteffect"), "*.xml", true);
	IndexT fileIndex;

	this->SetupTables();

	

	Ptr<Db::Table> table = this->staticDb->GetTableByName("_PostEffect_Presets");

	Ptr<Dataset> dataset;
	Ptr<ValueTable> valueTable;
	dataset = table->CreateDataset();
	dataset->AddAllTableColumns();
	dataset->PerformQuery();
	valueTable = dataset->Values();

	for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
	{
		PostEffect::PostEffectEntity::ParamSet parms;
		parms.Init();
		if (ToolkitUtil::PostEffectParser::Load(files[fileIndex], parms))
		{
			IndexT row = valueTable->AddRow();
			valueTable->SetString(AttrId("Id"), row, parms.common->GetName());
			valueTable->SetFloat(AttrId("PEBlendTime"), row, parms.common->GetBlendTime());
			valueTable->SetMatrix44(AttrId("GlobalLightTransform"), row, parms.light->GetLightTransform());
			valueTable->SetFloat4(AttrId("GlobalLightDiffuse"), row, parms.light->GetLightColor());
			valueTable->SetFloat4(AttrId("GlobalLightOpposite"), row, parms.light->GetLightOppositeColor());
			valueTable->SetFloat4(AttrId("GlobalLightAmbient"), row, parms.light->GetLightAmbientColor());
			valueTable->SetBool(AttrId("GlobalLightCastShadows"), row, parms.light->GetLightCastsShadows());
			valueTable->SetFloat(AttrId("GlobalLightShadowIntensity"), row, parms.light->GetLightShadowIntensity());
			valueTable->SetFloat(AttrId("GlobalLightIntensity"), row, parms.light->GetLightIntensity());
			valueTable->SetFloat(AttrId("GlobalLightBacklightFactor"), row, parms.light->GetBackLightFactor());
			valueTable->SetFloat(AttrId("GlobalLightShadowBias"), row, parms.light->GetLightShadowBias());
			valueTable->SetFloat(AttrId("Saturation"), row, parms.color->GetColorSaturation());
			valueTable->SetFloat4(AttrId("Balance"), row, parms.color->GetColorBalance());
			valueTable->SetFloat(AttrId("MaxLuminance"), row, parms.color->GetColorMaxLuminance());
			valueTable->SetFloat(AttrId("BloomScale"), row, parms.hdr->GetHdrBloomIntensity());
			valueTable->SetFloat4(AttrId("BloomColor"), row, parms.hdr->GetHdrBloomColor());
			valueTable->SetFloat(AttrId("BloomThreshold"), row, parms.hdr->GetHdrBloomThreshold());
			valueTable->SetFloat(AttrId("FogNearDist"), row, parms.fog->GetFogNearDistance());
			valueTable->SetFloat(AttrId("FogFarDist"), row, parms.fog->GetFogFarDistance());
			valueTable->SetFloat(AttrId("FogHeight"), row, parms.fog->GetFogHeight());
			valueTable->SetFloat4(AttrId("FogColor"), row, parms.fog->GetFogColorAndIntensity());
			valueTable->SetFloat(AttrId("FocusDistance"), row, parms.dof->GetFocusDistance());
			valueTable->SetFloat(AttrId("FocusLength"), row, parms.dof->GetFocusLength());
			valueTable->SetFloat(AttrId("FocusRadius"), row, parms.dof->GetFilterSize());
			valueTable->SetFloat(AttrId("SkyContrast"), row, parms.sky->GetSkyContrast());
			valueTable->SetFloat(AttrId("SkyBrightness"), row, parms.sky->GetSkyBrightness());
			valueTable->SetString(AttrId("SkyTexture"), row, parms.sky->GetSkyTexturePath());
			valueTable->SetFloat(AttrId("AOStrength"), row, parms.ao->GetStrength());
			valueTable->SetFloat(AttrId("AORadius"), row, parms.ao->GetRadius());
			valueTable->SetFloat(AttrId("AOPower"), row, parms.ao->GetPower());
		}
		else
		{
			this->logger->Error("Failed to parse post effect preset file: %s\n", files[fileIndex].AsCharPtr());
			this->SetHasErrors(true);
		}
	}
	dataset->CommitChanges();
	table->CommitChanges();
	table = 0;
}

}