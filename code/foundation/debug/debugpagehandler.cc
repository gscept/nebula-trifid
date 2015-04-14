//------------------------------------------------------------------------------
//  debugpagehandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "debug/debugpagehandler.h"
#include "http/html/htmlpagewriter.h"
#include "http/svg/svglinechartwriter.h"
#include "debug/debugserver.h"
#include "debug/debugtimer.h"
#include "debug/debugcounter.h"
#include "util/variant.h"

namespace Debug
{
__ImplementClass(Debug::DebugPageHandler, 'DBPH', Http::HttpRequestHandler);

using namespace Http;
using namespace Util;
using namespace Timing;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DebugPageHandler::DebugPageHandler():
sortByColumn("Name")
{
    this->SetName("Debug");
    this->SetDesc("show debug subsystem information");
    this->SetRootLocation("debug");
}

//------------------------------------------------------------------------------
/**
*/
void
DebugPageHandler::HandleRequest(const Ptr<HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());

    // first check if a command has been defined in the URI
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("timer"))
    {
        this->HandleTimerRequest(query["timer"], request);
        return;
    }
    else if (query.Contains("counter"))
    {
        this->HandleCounterRequest(query["counter"], request);
        return;
    }
    else if (query.Contains("timerChart"))
    {
        this->HandleTimerChartRequest(query["timerChart"], request);
        return;
    }
    else if (query.Contains("counterChart"))
    {
        this->HandleCounterChartRequest(query["counterChart"], request);
        return;
    }
    else if (query.Contains("TimerTableSort"))
    {
        this->HandleTableSortRequest(query["TimerTableSort"], request);        
    }

    // no command, display root page
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT Debug Subsystem Info");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "Debug Subsystem");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");

        // display debug timers
        htmlWriter->Element(HtmlElement::Heading3, "Debug Timers");
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                htmlWriter->AddAttr("href", "/debug?TimerTableSort=Name");
                htmlWriter->Element(HtmlElement::Anchor, "Name");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Begin(HtmlElement::TableData);
                htmlWriter->AddAttr("href", "/debug?TimerTableSort=Min");
                htmlWriter->Element(HtmlElement::Anchor, "Min");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Begin(HtmlElement::TableData);
                htmlWriter->AddAttr("href", "/debug?TimerTableSort=Max");
                htmlWriter->Element(HtmlElement::Anchor, "Max");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Begin(HtmlElement::TableData);
                htmlWriter->AddAttr("href", "/debug?TimerTableSort=Avg");
                htmlWriter->Element(HtmlElement::Anchor, "Avg");
                htmlWriter->End(HtmlElement::TableData);
				htmlWriter->Begin(HtmlElement::TableData);
				htmlWriter->AddAttr("href", "/debug?TimerTableSort=Cur");
				htmlWriter->Element(HtmlElement::Anchor, "Cur");
				htmlWriter->End(HtmlElement::TableData);
            htmlWriter->End(HtmlElement::TableRow);

            // iterate through all debug timers
            Array<Ptr<DebugTimer> > debugTimers = DebugServer::Instance()->GetDebugTimers();
            // copy to dictionary for sort
            Dictionary<Variant, Ptr<DebugTimer> > sortedTimer;            
            sortedTimer.BeginBulkAdd();
            IndexT idx;
            for (idx = 0; idx < debugTimers.Size(); ++idx)
            {
                Array<Time> history = debugTimers[idx]->GetHistory();
                Time minTime, maxTime, avgTime, curTime;
                this->ComputeMinMaxAvgTimes(history, minTime, maxTime, avgTime, curTime);
                if (this->sortByColumn == "Name")
                {
            	    sortedTimer.Add(debugTimers[idx]->GetName().Value(), debugTimers[idx]);
                }
                else if (this->sortByColumn == "Min")
                {
                    sortedTimer.Add(float(minTime), debugTimers[idx]);
                }
                else if (this->sortByColumn == "Max")
                {
                    sortedTimer.Add(float(maxTime), debugTimers[idx]);
                }
                else if (this->sortByColumn == "Avg")
                {
                    sortedTimer.Add(float(avgTime), debugTimers[idx]);
                }
				else if (this->sortByColumn == "Cur")
				{
					sortedTimer.Add(float(curTime), debugTimers[idx]);
				}
            }
            sortedTimer.EndBulkAdd();
            debugTimers = sortedTimer.ValuesAsArray();

            IndexT i;
            for (i = 0; i < debugTimers.Size(); i++)
            {
                StringAtom name = debugTimers[i]->GetName();
                Array<Time> history = debugTimers[i]->GetHistory();
                Time minTime, maxTime, avgTime, curTime;
                this->ComputeMinMaxAvgTimes(history, minTime, maxTime, avgTime, curTime);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/debug?timer=" + name.AsString());
                        htmlWriter->Element(HtmlElement::Anchor, name.AsString());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Element(HtmlElement::TableData, String::FromFloat(float(minTime)));
                    htmlWriter->Element(HtmlElement::TableData, String::FromFloat(float(maxTime)));
                    htmlWriter->Element(HtmlElement::TableData, String::FromFloat(float(avgTime)));
					htmlWriter->Element(HtmlElement::TableData, String::FromFloat(float(curTime)));
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);

        // display debug counters
        htmlWriter->Element(HtmlElement::Heading3, "Debug Counters");
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "Name");
                htmlWriter->Element(HtmlElement::TableHeader, "Min");
                htmlWriter->Element(HtmlElement::TableHeader, "Max");
                htmlWriter->Element(HtmlElement::TableHeader, "Avg");
				htmlWriter->Element(HtmlElement::TableHeader, "Cur");
            htmlWriter->End(HtmlElement::TableRow);

            // iterate through all debug counters
            Array<Ptr<DebugCounter> > debugCounters = DebugServer::Instance()->GetDebugCounters();
            for (i = 0; i < debugCounters.Size(); i++)
            {
                StringAtom name = debugCounters[i]->GetName();
                Array<int> history = debugCounters[i]->GetHistory();
                int minCount, maxCount, avgCount, curCount;
                this->ComputeMinMaxAvgCounts(history, minCount, maxCount, avgCount, curCount);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/debug?counter=" + name.AsString());
                        htmlWriter->Element(HtmlElement::Anchor, name.Value());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(minCount));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(maxCount));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(avgCount));
					htmlWriter->Element(HtmlElement::TableData, String::FromInt(curCount));
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);

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
    Gets the min/max/avg time from an array of Time samples.
*/
void
DebugPageHandler::ComputeMinMaxAvgTimes(const Array<Time>& times, Time& outMin, Time& outMax, Time& outAvg, Timing::Time& outCur) const
{
    if (times.Size() > 0)
    {
        outMin = 10000000.0f;
        outMax = -10000000.0f;
        outAvg = 0.0;
		outCur = times.Back();
        IndexT i;
        for (i = 0; i < times.Size(); i++)
        {
            outMin = n_min(float(outMin), float(times[i]));
            outMax = n_max(float(outMax), float(times[i]));
            outAvg += times[i];
        }
        outAvg /= times.Size();
    }
    else
    {
        outMin = 0.0;
        outMax = 0.0;
        outAvg = 0.0;
		outCur = 0.0;
    }
}

//------------------------------------------------------------------------------
/**
    Gets the min/max/avg counter values from an array of counter samples.
*/
void
DebugPageHandler::ComputeMinMaxAvgCounts(const Array<int>& counterValues, int& outMin, int& outMax, int& outAvg, int& outCur) const
{
    if (counterValues.Size() > 0)
    {
        outMin = (1<<30);
        outMax = -(1<<30);
        outAvg = 0;
		outCur = counterValues.Back();
        IndexT i;
        for (i = 0; i < counterValues.Size(); i++)
        {
            outMin = n_min(outMin, counterValues[i]);
            outMax = n_max(outMax, counterValues[i]);
            outAvg += counterValues[i];
        }
        outAvg /= counterValues.Size();
    }
    else
    {
        outMin = 0;
        outMax = 0;
        outAvg = 0;
		outCur = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Handles a HTTP request for a specific debug timer.
*/
void
DebugPageHandler::HandleTimerRequest(const String& timerName, const Ptr<HttpRequest>& request)
{
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT Debug Timer Info");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "Timer: " + timerName);
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/debug");
        htmlWriter->Element(HtmlElement::Anchor, "Debug Subsystem Home");
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("data", "/debug?timerChart=" + timerName);
        htmlWriter->Element(HtmlElement::Object, timerName);    
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
    Handles a HTTP request for a specific debug counter.
*/
void
DebugPageHandler::HandleCounterRequest(const String& counterName, const Ptr<HttpRequest>& request)
{
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT Debug Counter Info");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "Counter: " + counterName);
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/debug");
        htmlWriter->Element(HtmlElement::Anchor, "Debug Subsystem Home");
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("data", "/debug?counterChart=" + counterName);
        htmlWriter->Element(HtmlElement::Object, counterName);    
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
    Writes an SVG chart for a debug timer.
*/
void
DebugPageHandler::HandleTimerChartRequest(const String& timerName, const Ptr<HttpRequest>& request)
{
    // setup a SVG line chart writer
    Ptr<DebugTimer> debugTimer = DebugServer::Instance()->GetDebugTimerByName(timerName);
    if (debugTimer.isvalid())
    {
        Ptr<SvgLineChartWriter> writer = SvgLineChartWriter::Create();
        writer->SetStream(request->GetResponseContentStream());
        writer->SetCanvasDimensions(1024, 256);
        if (writer->Open())
        {
            // get min/max/avg times, convert time to float array
            Array<Time> timeArray = debugTimer->GetHistory();
            Time minTime, maxTime, avgTime, curTime;
            this->ComputeMinMaxAvgTimes(timeArray, minTime, maxTime, avgTime, curTime);
            Array<float> floatArray(timeArray.Size(), 0);
            IndexT i;
            for (i = 0; i < timeArray.Size(); i++)
            {
                floatArray.Append(float(timeArray[i]));
            }

            // setup the svg chart writer and draw the chart
            writer->SetupXAxis("frames", "frames", -int(floatArray.Size()), 0);
            writer->SetupYAxis("samples", "samples", 0.0f, float(maxTime));
            writer->AddTrack("samples", "red", floatArray);
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

//------------------------------------------------------------------------------
/**
    Writes an SVG chart for a debug counter.
*/
void
DebugPageHandler::HandleCounterChartRequest(const String& counterName, const Ptr<HttpRequest>& request)
{
    // setup a SVG line chart writer
    Ptr<DebugCounter> debugCounter = DebugServer::Instance()->GetDebugCounterByName(counterName);
    if (debugCounter.isvalid())
    {
        Ptr<SvgLineChartWriter> writer = SvgLineChartWriter::Create();
        writer->SetStream(request->GetResponseContentStream());
        writer->SetCanvasDimensions(1024, 256);
        if (writer->Open())
        {
            // get min/max/avg values, convert int to float array
            // FIXME: SvgLineChartWriter should also accept integer tracks!
            Array<int> intArray = debugCounter->GetHistory();
            if (intArray.Size() > 0)
            {        
                int minVal, maxVal, avgVal, curVal;
                this->ComputeMinMaxAvgCounts(intArray, minVal, maxVal, avgVal, curVal);

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
                writer->AddTrack("samples", "red", floatArray);
                writer->Draw(); 
            }
            writer->Close();
        }
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::NotFound);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugPageHandler::HandleTableSortRequest(const Util::String& columnName, const Ptr<Http::HttpRequest>& request)
{
    this->sortByColumn = columnName;
    request->SetStatus(HttpStatus::OK);
}
} // namespace Debug
