//------------------------------------------------------------------------------
//  NetworkPageHandler.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/debug/networkpagehandler.h"
#include "http/html/htmlpagewriter.h"
#include "network/multiplayerserver.h"  
#include "http/svg/svglinechartwriter.h"
#include "debug/debugserver.h"
#include "network/session.h"
#include "network/player.h"

namespace Debug
{
__ImplementClass(Debug::NetworkPageHandler, 'DNPH', Debug::DebugPageHandler);

using namespace Http;
using namespace Util;
using namespace IO;
using namespace Multiplayer;

//------------------------------------------------------------------------------
/**
*/
NetworkPageHandler::NetworkPageHandler()
{
    this->SetName("Network");
    this->SetDesc("show network debug information");
    this->SetRootLocation("network");
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPageHandler::HandleRequest(const Ptr<Http::HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());

    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("counterChart"))
    {
        this->HandleCounterChartRequest(query["counterChart"], request);
        return;
    }

    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Nebula3 Network Info");
    if (htmlWriter->Open())
    {
        String str;
        htmlWriter->Element(HtmlElement::Heading1, "Network");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");        

        // list all statistics variables
        Multiplayer::MultiplayerServer::Instance()->WriteStatistics(htmlWriter);

        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);        
    }
    else
    {
        request->SetStatus(HttpStatus::ServerError);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkPageHandler::HandleCounterChartRequest(const Util::String& counterName, const Ptr<Http::HttpRequest>& request)
{
    const Ptr<Session>& session = MultiplayerServer::Instance()->GetSession();
    const Util::Array<Ptr<Player>> player = session->GetPlayers(true);
        
    Util::Array<Util::String> tokens = counterName.Tokenize("_");
    n_assert(tokens.Size() == 2);
    IndexT playerIndex = (IndexT)tokens[0].AsInt();
    if (player.Size() > playerIndex)
    {
        // setup a SVG line chart writer
        Util::Dictionary<Util::StringAtom, Multiplayer::Player::StatisticValue>& counterSet = player[playerIndex]->GetDebugCounterSet();            
        if (counterSet.Contains(tokens[1]))
        {
            const Multiplayer::UniquePlayerId& uniqueId = player[playerIndex]->GetUnqiuePlayerId();
            const RakNetGUID& guid = uniqueId.GetRaknetGuid();
            const SystemAddress& systemAddress = MultiplayerServer::Instance()->GetRakPeerInterface()->GetSystemAddressFromGuid(guid); 
            Util::String addressAsString(systemAddress.ToString(false));
          
            Ptr<SvgLineChartWriter> writer = SvgLineChartWriter::Create();
            writer->SetStream(request->GetResponseContentStream());
            writer->SetCanvasDimensions(1024, 256);
            if (writer->Open())
            {
                writer->Text(0, 250, counterSet[tokens[1]].GetDescription()); 
                // get min/max/avg values, convert int to float array
                // FIXME: SvgLineChartWriter should also accept integer tracks!
                Array<int> intArray = counterSet[tokens[1]].ValuesAsArray();
                Array<Timing::Tick> timeArray = counterSet[tokens[1]].TimeStampsAsArray();
                int minVal, maxVal, avgVal;
                this->ComputeMinMaxAvgCounts(intArray, minVal, maxVal, avgVal);

                // for maxVal == minVal set valid maxVal
                if (minVal == maxVal) maxVal = minVal + 1;

                Array<float> floatArray(intArray.Size(), 0);
                IndexT i;
                for (i = 0; i < intArray.Size(); i++)
                {
                    floatArray.Append(float(intArray[i]));
                }

                // setup the svg chart writer and draw the chart
                writer->SetupXAxis("frames", "frames", -int(floatArray.Size()), 0);
                writer->SetupYAxis("samples", "samples", 0.0f, float(maxVal));
                writer->AddTrack("samples", "red", floatArray, timeArray);
                writer->Draw();
                writer->Close();
            }
            request->SetStatus(HttpStatus::OK);
        }
        else
        {
            request->SetStatus(HttpStatus::NotFound);
        }
    }
}

} // namespace Debug
