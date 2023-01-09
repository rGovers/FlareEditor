#include "Windows/ProfilerWindow.h"

#include <implot.h>

#include "ProfilerData.h"

ProfilerWindow::ProfilerWindow() : Window("Profiler")
{

}
ProfilerWindow::~ProfilerWindow()
{

}

void ProfilerWindow::Update()
{
    const std::vector<ProfileSnapshot> snapshots = ProfilerData::GetSnapshots();

    constexpr int TimeOffset = offsetof(ProfileFrame, Time);
    constexpr int FramesOffeset = offsetof(ProfileScope, Frames);

    constexpr int TimeFrameOffet = TimeOffset + FramesOffeset;

    for (const ProfileSnapshot snapshot : snapshots)
    {
        ImPlot::SetNextAxesToFit();
        if (ImPlot::BeginPlot(snapshot.Name.c_str()))
        {
            ImPlot::PlotLine<float>(snapshot.Scopes[0].Frames[0].Name, (float*)((char*)&snapshot.Scopes + TimeFrameOffet), (int)snapshot.Count, 1.0, 0.0, 0, snapshot.StartIndex, sizeof(ProfileScope));

            ImPlot::EndPlot();
        }
    }
}