//------------------------------------------------------------------------------
// streamingtexturepagehandler.cc
// (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "streamingtexturepagehandler.h"
#include "http/html/htmlpagewriter.h"
#include "resources/resourcemanager.h"
#include "coregraphics/texture.h"
#include "coregraphics/pixelformat.h"
#if __WIN32__
#include "resources/streaming/poolresourcemapper.h"
#include "resources/streaming/resourcepool.h"
#include "resources/streaming/textureinfo.h"
#include "io/stream.h"
#include "coregraphics/streamtexturesaver.h"
#include "http/svg/svgpagewriter.h"
#include "http/svg/svglinechartwriter.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "io/xmlwriter.h"
#include "resources/resourceid.h"
#endif

namespace Debug
{

__ImplementClass(Debug::StreamingTexturePageHandler, 'STPH', Http::HttpRequestHandler);

using namespace Http;
using namespace Resources;
using namespace CoreGraphics;
using namespace Util;
using namespace IO;
//------------------------------------------------------------------------------
/**
*/
StreamingTexturePageHandler::StreamingTexturePageHandler() :
    defaultPoolName("tex:default_pool.xml"),
    modifiedPoolName("tex:modified_pool.xml")
{
    this->SetName("Texture Streaming");
    this->SetDesc("statistics and control of texture streaming");
    this->SetRootLocation("texturestreaming");
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingTexturePageHandler::HandleRequest(const Ptr<HttpRequest>& request) 
{
    n_assert(HttpMethod::Get == request->GetMethod());
    // load pool-setup if necessary
    if (this->poolSetupDict.Size() == 0)
    {
        this->LoadPoolXML();
        if (this->poolSetupDict.Size() == 0)
        {
            Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
            htmlWriter->SetStream(request->GetResponseContentStream());
            htmlWriter->SetTitle("Texture Streaming");
            if (htmlWriter->Open())
            {
                htmlWriter->Element(HtmlElement::Heading2, "Neither modified pool nor default pool were loaded. Please check their existence.");
                htmlWriter->Close();
                request->SetStatus(HttpStatus::OK);
                return;
            }
            else
            {
                request->SetStatus(HttpStatus::InternalServerError);
            }
        }
    }

    // first check if a command has been defined in the URI
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("slotset") && query["slotset"] == "true")
    {
        this->UpdateSlotSets(query);
    }
    if (query.Contains("pool"))
    {
        if (query.Contains("slot"))
        {
            this->HandleSlotRequest(query["pool"], query["slot"], request);
            return;
        }
        else
        {
            this->HandlePoolRequest(query["pool"], request);
            return;
        }
    }
    else if (query.Contains("img"))
    {
        this->HandleImageRequest(query, request->GetResponseContentStream());
        return;
    }
    else if (query.Contains("svg"))
    {
        this->SVGChart(query, request);
        return;
    }

    // create an array of strings containing all poolIDs
    // this allows us to easily sort pools by their ID
    this->dictSort.Clear();
    IndexT i;
    for (i = 0; i < this->poolSetupDict.Size(); i++)
    {
        this->dictSort.Append(this->poolSetupDict.KeyAtIndex(i).AsString());
    }
    this->dictSort.Sort();

    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Texture Streaming");
    if (htmlWriter->Open())
    {
        // --- setup head of page ---
        htmlWriter->Element(HtmlElement::Heading1, "Texture Streaming");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/texturestreaming");
        htmlWriter->Element(HtmlElement::Anchor, "Refresh");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();

#if !__WIN32__
        htmlWriter->Element(HtmlElement::Heading2, "ResourceStreaming interface not supported on this platform.");
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
        return;
#else
        // --- gather and print data ---
        if (!ResourceManager::Instance()->GetMapperByResourceType(Texture::RTTI)->GetRtti()->IsDerivedFrom(PoolResourceMapper::RTTI))
        {
            htmlWriter->Element(HtmlElement::Heading2, "ResourceStreaming interface not supported for current ResourceMapper.");
            htmlWriter->Close();
            request->SetStatus(HttpStatus::OK);
            return;
        }
        Ptr<PoolResourceMapper> mapper = ResourceManager::Instance()->GetMapperByResourceType(Texture::RTTI).downcast<PoolResourceMapper>();

        uint allocatedMemory = 0, usedMemory = 0, newAllocatedMemory = 0;
        IndexT poolIdx;
        PoolStatisticsRequest statisticsRequest;
        statisticsRequest.active = SET;
        Ptr<ResourcePool> pool = 0;
        for (poolIdx = 0; poolIdx < this->poolSetupDict.Size(); poolIdx++)
        {
            pool = mapper->GetPoolForDebug(this->poolSetupDict.KeyAtIndex(poolIdx));
            allocatedMemory += pool->GetPoolSize();
            usedMemory += pool->GetSlotStatistics(statisticsRequest) * pool->GetInfo()->GetSize();
            newAllocatedMemory += pool->GetInfo()->GetSize() * this->poolSetupDict.ValueAtIndex(poolIdx).numSlots;
        }
        htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Element(HtmlElement::TableData, "Maximum Texture Memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "Allocated texture memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(allocatedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "Currently used texture memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(usedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "texture memory left (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__ - allocatedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "new texture memory left (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__ - newAllocatedMemory));
        htmlWriter->End(HtmlElement::Table);

        htmlWriter->AddAttr("type", "text/css");
        htmlWriter->Begin(HtmlElement::Style);
            htmlWriter->Text("th{color:#FFFFFF}");
            htmlWriter->Text("td{min-width: 90");
        htmlWriter->End(HtmlElement::Style);

        htmlWriter->AddAttr("name", "addSubForm");
        htmlWriter->AddAttr("action", "/texturestreaming?slotSet=true&");
        htmlWriter->Begin(HtmlElement::Form);
            htmlWriter->AddAttr("type", "hidden");
            htmlWriter->AddAttr("name", "slotset");
            htmlWriter->AddAttr("value", "true");
            htmlWriter->Element(HtmlElement::Input, "");

            htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->Begin(HtmlElement::TableData);
            // create a table of all existing pools
            htmlWriter->AddAttr("border", "1");
            htmlWriter->AddAttr("rules", "cols");
            htmlWriter->Begin(HtmlElement::Table);
                // header
                htmlWriter->AddAttr("bgcolor", "#4F81BD");
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->AddAttr("align", "center");
                    htmlWriter->Element(HtmlElement::TableHeader, "PoolID");
                    this->AddPoolHeaders(htmlWriter, false);
                htmlWriter->End(HtmlElement::TableRow);

                // data
                String rowBGColor = "#B8CCE4";
                for (poolIdx = 0; poolIdx < this->dictSort.Size(); poolIdx++)
                {
                    if (poolIdx % 2 == 1)
                    {
                        rowBGColor = "#DBE5F1";
                    }
                    else
                    {
                        rowBGColor = "#B8CCE4";
                    }
                    pool = mapper->GetPoolForDebug(this->dictSort[poolIdx]);
                    htmlWriter->AddAttr("bgcolor", rowBGColor);
                    htmlWriter->Begin(HtmlElement::TableRow);
                        htmlWriter->Begin(HtmlElement::TableData);
                            htmlWriter->AddAttr("href", "/texturestreaming?pool=" + pool->GetId().AsString());
                            htmlWriter->Element(HtmlElement::Anchor, pool->GetId().AsString());
                        htmlWriter->End(HtmlElement::TableData);
                        this->AddPoolData(pool, htmlWriter, false);
                    htmlWriter->End(HtmlElement::TableRow);
                }
            htmlWriter->End(HtmlElement::Table);
            htmlWriter->End(HtmlElement::TableData);

            htmlWriter->Begin(HtmlElement::TableData);
            htmlWriter->AddAttr("type", "submit");
            htmlWriter->AddAttr("name", "buttonAllSlotSet");
            int estimatedHeight = mapper->GetNumPools() * 23 + 55;
            htmlWriter->AddAttr("style", "height:" + String::FromInt(estimatedHeight) + "; background-color:darkblue; color:white");
            htmlWriter->AddAttr("value", "S&#xA;U&#xA;B&#xA;M&#xA;I&#xA;T");
            htmlWriter->Element(HtmlElement::Input, "");
            htmlWriter->End(HtmlElement::TableData);
            htmlWriter->End(HtmlElement::Table);
        htmlWriter->End(HtmlElement::Form);

        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->Text("Glossary:"); htmlWriter->LineBreak();
        htmlWriter->Text("Size total: Total allocated size of pool in memory in kilo bytes."); htmlWriter->LineBreak();
        htmlWriter->Text("Work Load: Total of slots divided by (Active slots + RejectedRequests)."); htmlWriter->LineBreak();
        htmlWriter->Text("Slots total: Total amount of slots this pool has allocated."); htmlWriter->LineBreak();
        htmlWriter->Text("Active Slots: Slots which are recently used and not free for reuse."); htmlWriter->LineBreak();
        htmlWriter->Text("Rejected Requests: Counter of resource requests which haven't been successful because all slots are in use."); htmlWriter->LineBreak();
        htmlWriter->Text("No auto mip pool: Counter increases as textures request another mip level but no FITTING POOL was found for the appropriate format."); htmlWriter->LineBreak();
        htmlWriter->Text("No auto mip pool: Counter increases as textures request another mip level but no FREE SLOT was found for the appropriate format in fitting pool."); htmlWriter->LineBreak();
        htmlWriter->Text("New Size: Total allocated size of pool in memory in kilo bytes WHICH MAPPER WILL ALLOCATE ON NEXT STARTUP."); htmlWriter->LineBreak();
        htmlWriter->Text("Work Load: Total of NEW SLOT COUNTER divided by (Active slots + RejectedRequests)."); htmlWriter->LineBreak();
        htmlWriter->Text("New Slots total: Total amount of slots this pool WILL ALLOCATE ON NEXT STARTUP."); htmlWriter->LineBreak();

        // --- clean up ---
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
#endif
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
*/
#if __WIN32__
void
StreamingTexturePageHandler::HandlePoolRequest(const Util::String& poolName, const Ptr<Http::HttpRequest>& request)
{
    Dictionary<String,String> query = request->GetURI().ParseQuery();

    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Texture Streaming - Pool Nr. " + poolName);
    if (htmlWriter->Open())
    {
        // --- setup head of page ---
        htmlWriter->Element(HtmlElement::Heading1, "Texture Streaming - Pool Nr. " + poolName);
        htmlWriter->AddAttr("href", "/texturestreaming");
        htmlWriter->Element(HtmlElement::Anchor, "Back");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/texturestreaming?pool=" + query["pool"]);
        htmlWriter->Element(HtmlElement::Anchor, "Refresh");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();

        uint allocatedMemory = 0, usedMemory = 0, newAllocatedMemory = 0;
        IndexT poolIdx;
        PoolStatisticsRequest statisticsRequest;
        statisticsRequest.active = SET;
        const Ptr<PoolResourceMapper> mapper = ResourceManager::Instance()->GetMapperByResourceType(Texture::RTTI).downcast<PoolResourceMapper>();
        Ptr<ResourcePool> statPool = 0;
        for (poolIdx = 0; poolIdx < this->poolSetupDict.Size(); poolIdx++)
        {
            statPool = mapper->GetPoolForDebug(this->poolSetupDict.KeyAtIndex(poolIdx));
            allocatedMemory += statPool->GetPoolSize();
            usedMemory += statPool->GetSlotStatistics(statisticsRequest) * statPool->GetInfo()->GetSize();
            newAllocatedMemory += statPool->GetInfo()->GetSize() * this->poolSetupDict.ValueAtIndex(poolIdx).numSlots;
        }
        htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Element(HtmlElement::TableData, "Maximum Texture Memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "Allocated texture memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(allocatedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "Currently used texture memory (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(usedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "texture memory left (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__ - allocatedMemory));

        htmlWriter->Element(HtmlElement::TableRow, "");
        htmlWriter->Element(HtmlElement::TableData, "new texture memory left (kB)");
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(__maxTextureBytes__ - newAllocatedMemory));
        htmlWriter->End(HtmlElement::Table);

        // --- gather and print data ---
        const Ptr<ResourcePool>& pool = mapper->GetPoolForDebug(poolName);

        htmlWriter->Begin(HtmlElement::Style);
        htmlWriter->Text("th{color:#FFFFFF}");
        htmlWriter->Text("td{min-width: 90");
        htmlWriter->End(HtmlElement::Style);

        htmlWriter->AddAttr("name", "addSubForm");
        htmlWriter->AddAttr("action", "/texturestreaming?slotSet=true&");
        htmlWriter->Begin(HtmlElement::Form);
        // display extended pool information
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->AddAttr("align", "left");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("type", "hidden");
            htmlWriter->AddAttr("name", "slotset");
            htmlWriter->AddAttr("value", "true");
            htmlWriter->Element(HtmlElement::Input, "");

            htmlWriter->AddAttr("type", "hidden");
            htmlWriter->AddAttr("name", "pool");
            htmlWriter->AddAttr("value", query["pool"]);
            htmlWriter->Element(HtmlElement::Input, "");

            htmlWriter->AddAttr("bgcolor", "#4F81BD");
            htmlWriter->Begin(HtmlElement::TableRow);
            this->AddPoolHeaders(htmlWriter, true);
            // submit button
            //htmlWriter->Element(HtmlElement::TableHeader, "");
            htmlWriter->End(HtmlElement::TableRow);

            htmlWriter->Begin(HtmlElement::TableRow);
            this->AddPoolData(pool, htmlWriter, true);

            htmlWriter->Begin(HtmlElement::TableData);
            htmlWriter->AddAttr("type", "submit");
            htmlWriter->AddAttr("name", "buttonSingleSlotSet");
            htmlWriter->AddAttr("style", "background-color:darkblue; color:white");
            htmlWriter->AddAttr("value", "SUBMIT");
            htmlWriter->Element(HtmlElement::Input, "");
            htmlWriter->End(HtmlElement::TableData);

            htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->End(HtmlElement::Table);

        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();

        // create a table of all existing slots
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "#4F81BD");
            htmlWriter->Begin(HtmlElement::TableRow);
                /// slot link
                htmlWriter->AddAttr("align", "center");
                htmlWriter->Element(HtmlElement::TableHeader, "SlotID");
                htmlWriter->AddAttr("align", "center");
                htmlWriter->Element(HtmlElement::TableHeader, "ResourceID");
                htmlWriter->AddAttr("align", "center");
                htmlWriter->Element(HtmlElement::TableHeader, "State");
            htmlWriter->End(HtmlElement::TableRow);

            // iterate over slots
            String rowBGColor = "#B8CCE4";
            IndexT slotIdx;
            TextureInfo* poolInfo;
            for (slotIdx = 0; slotIdx < pool->GetNumSlots(); slotIdx++)
            {
                poolInfo = (TextureInfo*)pool->GetInfo();
                const ResourceId& resId = pool->GetIdForSlot(slotIdx);

                if (slotIdx % 2 == 1)
                {
                    rowBGColor = "#DBE5F1";
                }
                else
                {
                    rowBGColor = "#B8CCE4";
                }

                htmlWriter->AddAttr("bgcolor", rowBGColor);
                htmlWriter->Begin(HtmlElement::TableRow);
                    // slot link
                    htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/texturestreaming?pool=" + poolName + "&slot=" + resId.AsString());
                    htmlWriter->Element(HtmlElement::Anchor, String::FromInt(slotIdx));
                    htmlWriter->End(HtmlElement::TableData);
                    // resource
                    htmlWriter->AddAttr("align", "right");
                    htmlWriter->Element(HtmlElement::TableData, resId.AsString());
                    // state
                    htmlWriter->AddAttr("align", "right");
                    htmlWriter->Element(HtmlElement::TableData, pool->GetStateStringForSlot(resId).AsString());
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);

        htmlWriter->End(HtmlElement::Form);

        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->Text("Glossary:"); htmlWriter->LineBreak();
        htmlWriter->Text("Size total: Total allocated size of pool in memory in kilo bytes."); htmlWriter->LineBreak();
        htmlWriter->Text("Work Load: Total of slots divided by (Active slots + RejectedRequests)."); htmlWriter->LineBreak();
        htmlWriter->Text("Slots total: Total amount of slots this pool has allocated."); htmlWriter->LineBreak();
        htmlWriter->Text("Active Slots: Slots which are recently used and not free for reuse."); htmlWriter->LineBreak();
        htmlWriter->Text("Rejected Requests: Counter of resource requests which haven't been successful because all slots are in use."); htmlWriter->LineBreak();
        htmlWriter->Text("No auto mip pool: Counter increases as textures request another mip level but no FITTING POOL was found for the appropriate format."); htmlWriter->LineBreak();
        htmlWriter->Text("No auto mip pool: Counter increases as textures request another mip level but no FREE SLOT was found for the appropriate format in fitting pool."); htmlWriter->LineBreak();
        htmlWriter->Text("New Size: Total allocated size of pool in memory in kilo bytes WHICH MAPPER WILL ALLOCATE ON NEXT STARTUP."); htmlWriter->LineBreak();
        htmlWriter->Text("Work Load: Total of NEW SLOT COUNTER divided by (Active slots + RejectedRequests)."); htmlWriter->LineBreak();
        htmlWriter->Text("New Slots total: Total amount of slots this pool WILL ALLOCATE ON NEXT STARTUP."); htmlWriter->LineBreak();

        // --- clean up ---
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingTexturePageHandler::HandleSlotRequest(const Util::String& poolName, const Util::String& slotName, const Ptr<Http::HttpRequest>& request)
{
    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Texture Streaming - Pool Nr. " + poolName + " : Slot for Texture " + slotName);
    if (htmlWriter->Open())
    {
        // --- setup head of page ---
        htmlWriter->Element(HtmlElement::Heading1, "Texture " + slotName);
        htmlWriter->AddAttr("href", "/texturestreaming?pool=" + poolName);
        htmlWriter->Element(HtmlElement::Anchor, "Back to Pool");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();

        if (!ResourceManager::Instance()->GetMapperByResourceType(Texture::RTTI)->GetRtti()->IsDerivedFrom(PoolResourceMapper::RTTI))
        {
            htmlWriter->Element(HtmlElement::Heading2, "ResourceStreaming interface not supported for current ResourceMapper.");
            htmlWriter->Close();
            request->SetStatus(HttpStatus::OK);
            return;
        }

        const Ptr<PoolResourceMapper> mapper = ResourceManager::Instance()->GetMapperByResourceType(Texture::RTTI).downcast<PoolResourceMapper>();
        const Ptr<ResourcePool>& pool = mapper->GetPoolForDebug(poolName);
        const Ptr<ResourceSlot> slot = pool->GetSlot(slotName);
        if (!slot.isvalid())
        {
            htmlWriter->Element(HtmlElement::Heading2, "Texture " + slotName + " not found in current pool.");
            htmlWriter->Close();
            request->SetStatus(HttpStatus::OK);
        }

        const Ptr<Texture> tex = slot->GetResource().downcast<Texture>();
        n_assert(tex.isvalid());

        // display the texture image data
        IndexT mipLevel;
        for (mipLevel = 0; mipLevel < tex->GetNumMipLevels(); mipLevel++)
        {
            String fmt;
            fmt.Format("/texture?img=%s&mip=%d", slotName.AsCharPtr(), mipLevel);
            htmlWriter->AddAttr("src", fmt);
            htmlWriter->Element(HtmlElement::Image, "");
        }

        // --- clean up ---
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
Handle a "raw" texture image request.
*/
HttpStatus::Code
StreamingTexturePageHandler::HandleImageRequest(const Dictionary<String,String>& query, const Ptr<Stream>& responseStream)
{
    n_assert(query.Contains("img"));
    const Ptr<ResourceManager>& resManager = ResourceManager::Instance();

    // get input args
    ResourceId resId = ResourceId(query["img"]);
    ImageFileFormat::Code format = ImageFileFormat::InvalidImageFileFormat;
    if (query.Contains("fmt"))
    {
        format = ImageFileFormat::FromString(query["fmt"]);
    }
    if (ImageFileFormat::InvalidImageFileFormat == format)
    {
        format = ImageFileFormat::PNG;
    }
    IndexT mipLevel = 0;
    if (query.Contains("mip"))
    {
        mipLevel = query["mip"].AsInt();
    }

    // check if the request resource exists and is a texture
    if (!resManager->HasResource(resId))
    {
        return HttpStatus::NotFound;
    }
    const Ptr<Resource>& res = resManager->LookupResource(resId);
    if (!res->IsA(Texture::RTTI))
    {
        // resource exists but is not a texture
        return HttpStatus::NotFound;
    }
    const Ptr<Texture>& tex = res.downcast<Texture>();

    // attach a StreamTextureSaver to the texture
    // NOTE: the StreamSaver is expected to set the media type on the stream!
    HttpStatus::Code httpStatus = HttpStatus::InternalServerError;
    Ptr<StreamTextureSaver> saver = StreamTextureSaver::Create();
    saver->SetStream(responseStream);
    saver->SetFormat(format);
    saver->SetMipLevel(mipLevel);
    tex->SetSaver(saver.upcast<ResourceSaver>());
    if (tex->Save())
    {
        httpStatus = HttpStatus::OK;
    }
    tex->SetSaver(0);
    return httpStatus;
}

//------------------------------------------------------------------------------
/**
Handle a "raw" texture image request.
*/
bool
StreamingTexturePageHandler::SVGChart(const Dictionary<String,String>& query, const Ptr<HttpRequest>& request)
{
    Ptr<SvgPageWriter> svgWriter = SvgPageWriter::Create();
    svgWriter->SetStream(request->GetResponseContentStream());

    svgWriter->SetCanvasDimensions(100, 20);
    if (query["svg"] == "workload")
    {
        svgWriter->SetUnitDimensions(100, 30);
    }
    else
    {
        svgWriter->SetUnitDimensions(100, 100);
    }
    if (svgWriter->Open())
    {
        int total = query["total"].AsInt();
        int active = query["active"].AsInt();
        String percentageString;
        String colorString;
        if (total > 0)
        {
            float relation;
            if (active > 0)
            {
                relation = (float)active / (float)total;
            }
            else
            {
                relation = 0.0f;
            }
            float percentage = relation * 100.0f;
            percentageString.Format("%.2f", percentage);
            percentageString += "%";

            float red, green;
            // #FF0 == yellow

            float startGreenReduction = 0.6f, stopGreenReduction = 0.8f;
            float startRedIncrease = 0.2f, stopRedIncrease = 0.4f;

            if (relation < startGreenReduction)
            {
                green = 1.0f;
            }
            else if (relation > stopGreenReduction)
            {
                green = 0.0f;
            }
            else
            {
                green = 1.0f - ((relation - startGreenReduction) / (stopGreenReduction - startGreenReduction));
            }

            if (relation < startRedIncrease)
            {
                red = 0.0f;
            }
            else if (relation > stopRedIncrease)
            {
                red = 1.0f;
            }
            else
            {
                red = (relation - startRedIncrease) / (stopRedIncrease - startRedIncrease);
            }

            n_assert(green >= 0.0f && green <= 1.0f);
            n_assert(red >= 0.0f && red <= 1.0f);
            colorString = "#" + this->GetHexColorCode(red) + this->GetHexColorCode(green) + "0";
        }
        else
        {
            percentageString = "---";
            if (active > 0)
            {
                colorString = "#F00";
            }
            else
            {
                colorString = "#AAA";
            }
        }
        // render rectangle
        svgWriter->BeginPaintGroup(colorString, "black", 1);
        svgWriter->Rect(3, 3, 90, 70);
        svgWriter->EndGroup();
  
        svgWriter->BeginNode("text");
        svgWriter->SetString("x", "50");
        svgWriter->SetString("y", "25");
        svgWriter->SetString("style", "font-size: 18px");
        svgWriter->SetString("font-weight", "bold");
        svgWriter->SetString("text-anchor", "middle");
        svgWriter->WriteContent(percentageString);
        svgWriter->EndNode();

        request->SetStatus(HttpStatus::OK);

        svgWriter->Close();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
Util::String StreamingTexturePageHandler::GetHexColorCode(float colorValue)
{
    // normalize
    if (colorValue < 0.0f)
    {
        colorValue = 0.0f;
    }
    else if (colorValue >= 1.0f)
    {
        colorValue = 1.0f;
    }

    int colorKey = (int)(colorValue * 255.0f) / 16;
    if (colorKey < 10)
    {
        return String::FromInt(colorKey);
    }
    else
    {
        switch (colorKey)
        {
        case 10:
            return "A";
            break;
        case 11:
            return "B";
            break;
        case 12:
            return "C";
            break;
        case 13:
            return "D";
            break;
        case 14:
            return "E";
            break;
        case 15:
            return "F";
            break;
        default:
            n_error("huh? how did this happen?!");
            return "dummy";
        }
    }
}

//------------------------------------------------------------------------------
/**
    Adds several table headers by using given htmlWriter.
    Notice that the table row has to be opened and NOT YET been closed until calling this and
    should be closed any time after calling this.

    Currently adding:
    if extended:
    - width
    - height
    - pixelFormat
    - size per slot
    - num mip levels

    general:
    - size total
    - workload chart
    - total slots
    - total active slots
    - rejected requests (per frame)

    if extended:
    - auto mip pools not found (per frame)
    - auto mip slots not found (per frame)
    - rejected requests (total)
    - auto mip pools not found (total)
    - auto mip slots not found (total)

    general:
    - new total size
    - new workload chart
    - new slots total
*/
void
StreamingTexturePageHandler::AddPoolHeaders(const Ptr<HtmlPageWriter>& htmlWriter, bool extended)
{
    if (extended)
    {
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "Width");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "Height");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "PixelFormat");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "Size per Slot (kB)");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "Num Mips");
    }
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "Size total (kB)");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->AddAttr("width", "120");
    htmlWriter->Element(HtmlElement::TableHeader, "Workload");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "Slots total");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "Active Slots");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "Rejected Requests (this frame)");
    if (extended)
    {
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "No auto mip pool (this frame)");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "No auto mip slot (this frame)");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "Rejected Requests (total)");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "No auto mip pool (total)");
        htmlWriter->AddAttr("align", "center");
        htmlWriter->Element(HtmlElement::TableHeader, "No auto mip slot (total)");
    }
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "New Size");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "New Workload");
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Element(HtmlElement::TableHeader, "New Slots total");
}

//------------------------------------------------------------------------------
/**
    Adds several row data corresponding to AddPoolHeaders by using given htmlWriter.
    Notice that the table row has to be opened and NOT YET been closed until calling this and
    should be closed any time after calling this.
*/
void
StreamingTexturePageHandler::AddPoolData(const Ptr<ResourcePool>& pool, const Ptr<HtmlPageWriter>& htmlWriter, bool extended)
{
    TextureInfo* poolInfo = (TextureInfo*)pool->GetInfo();

    PoolStatisticsRequest statisticsRequest;
    statisticsRequest.Reset();
    statisticsRequest.none = SET;
    IndexT slotsTotal = pool->GetSlotStatistics(statisticsRequest);

    statisticsRequest.Reset();
    statisticsRequest.active = SET;
    IndexT slotsActive = pool->GetSlotStatistics(statisticsRequest);

    if (extended)
    {
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(poolInfo->GetWidth()));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(poolInfo->GetHeight()));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, PixelFormat::ToString(poolInfo->GetPixelFormat()));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(poolInfo->GetSize() / 1024));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(poolInfo->GetMipLevels()));
    }

    // size total
    htmlWriter->AddAttr("align", "right");
    htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetPoolSize() / 1024));

    // workload chart
    htmlWriter->Begin(HtmlElement::TableData);
    String fmt;
    fmt.Format("/texturestreaming?svg=workload&total=%i&active=%i", slotsTotal, slotsActive + pool->GetRejectedRequests());
    htmlWriter->AddAttr("data", fmt);
    htmlWriter->Element(HtmlElement::Object, "");
    htmlWriter->End(HtmlElement::TableData);

    // num slots
    htmlWriter->AddAttr("align", "right");
    htmlWriter->Element(HtmlElement::TableData, String::FromInt(slotsTotal));

    // active slots
    htmlWriter->AddAttr("align", "right");
    htmlWriter->Element(HtmlElement::TableData, String::FromInt(slotsActive));

    // rejected requests
    htmlWriter->AddAttr("align", "right");
    if (pool->GetRejectedRequests() != 0)
    {
        htmlWriter->AddAttr("bgcolor", "red");
        htmlWriter->Begin(HtmlElement::TableData);
        htmlWriter->AddAttr("color", "white");
        htmlWriter->Begin(HtmlElement::Font);
        htmlWriter->Begin(HtmlElement::Bold);
        htmlWriter->Text(String::FromInt(pool->GetRejectedRequests()));
        htmlWriter->End(HtmlElement::Bold);
        htmlWriter->End(HtmlElement::Font);
        htmlWriter->End(HtmlElement::TableData);
    }
    else
    {
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetRejectedRequests()));
    }
    if (extended)
    {
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetAutoMipPoolsNotFound()));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetAutoMipSlotsNotFound()));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetRejectedRequests(true)));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetAutoMipPoolsNotFound(true)));
        htmlWriter->AddAttr("align", "right");
        htmlWriter->Element(HtmlElement::TableData, String::FromInt(pool->GetAutoMipSlotsNotFound(true)));
    }

    // new pool size
    htmlWriter->AddAttr("align", "right");
    htmlWriter->Element(HtmlElement::TableData, String::FromInt(this->poolSetupDict[pool->GetId()].numSlots * pool->GetInfo()->GetSize() / 1024));

    // new workload chart
    htmlWriter->Begin(HtmlElement::TableData);
    fmt.Format("/texturestreaming?svg=workload&total=%i&active=%i", this->poolSetupDict[pool->GetId()].numSlots, slotsActive + pool->GetRejectedRequests());
    htmlWriter->AddAttr("data", fmt);
    htmlWriter->Element(HtmlElement::Object, "");
    htmlWriter->End(HtmlElement::TableData);

    // new slot count
    htmlWriter->AddAttr("align", "center");
    htmlWriter->Begin(HtmlElement::TableData);
    htmlWriter->AddAttr("type", "text");
    htmlWriter->AddAttr("name", pool->GetId().AsString());
    htmlWriter->AddAttr("value", String::FromInt(this->poolSetupDict[pool->GetId()].numSlots));
    htmlWriter->AddAttr("style", "text-align:right");
    htmlWriter->Element(HtmlElement::Input, "");
    htmlWriter->End(HtmlElement::TableData);
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingTexturePageHandler::LoadPoolXML()
{
    const Ptr<IoServer>& ioServer = IoServer::Instance();

    URI poolFile;
    this->poolSetupDict.Clear();
    if (ioServer->FileExists(this->modifiedPoolName))
    {
        // load modified pools
        poolFile = this->modifiedPoolName;
    }
    else if (ioServer->FileExists(this->defaultPoolName))
    {
        // load default pools
        poolFile = this->defaultPoolName;
    }
    else
    {
        return;
    }
    PoolResourceMapper::ReadTexturePoolFromXML(poolFile, this->poolSetupDict);
}

//------------------------------------------------------------------------------
/**
    This will parse slot numbers of given query to change modified_pools.xml
*/
void
StreamingTexturePageHandler::UpdateSlotSets(const Util::Dictionary<Util::String,Util::String>& query)
{
    // ensure modified or default xml is loaded
    n_assert(this->poolSetupDict.Size() != 0);
    n_assert(query.Size() != 0);

    // do changes
    bool saveNeeded = false;

    IndexT i;
    String curKey;
    for (i = 0; i < query.Size(); i++)
    {
        curKey = query.KeyAtIndex(i);
        if (this->poolSetupDict.Contains(curKey) && this->poolSetupDict[curKey].numSlots != query.ValueAtIndex(i).AsInt())
        {
            this->poolSetupDict[curKey].numSlots = query.ValueAtIndex(i).AsInt();
            saveNeeded = true;
        }
    }

    // save to file if any changes were made
    if (saveNeeded)
    {
        PoolResourceMapper::WriteTexturePoolToXML(this->modifiedPoolName, this->poolSetupDict);
    }
}
#endif
} // namespace FrameCapture
