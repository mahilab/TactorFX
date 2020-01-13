#include "DeviceBar.hpp"
#include <thread>
#include "ImGui/Custom.hpp"
#include "Gui.hpp"

using namespace carnot;


DeviceBar::DeviceBar(Gui* gui) :
    Widget(gui)
{ }


/// Restarts Syntacts session
void DeviceBar::initialize()
{
    // make new session
    session = make<tact::Session>();
    onSessionInit.emit();
    getAvailable();
    switchDevice(session->getDefaultDevice());
}

void DeviceBar::switchDevice(const tact::Device &dev, double sampleRate)
{
    int result = SyntactsError_NoError;
    // close if open
    if (session->isOpen())
        result = session->close();
    if (result != SyntactsError_NoError)
    {
        gui->status->pushMessage("Failed to close device! Error: " + str(result), StatusBar::Error);
    }
    // open requested device
    result = session->open(dev, dev.maxChannels, sampleRate);
    if (result != SyntactsError_NoError)
    {
        gui->status->pushMessage("Failed to open device! Error: " + str(result), StatusBar::Error);
    }
    /// update current and available
    getCurrent();
    onSessionOpen.emit();
}

void DeviceBar::switchApi(const std::string &api)
{
    if (m_currentApi != api)
    {
        m_currentApi = api;
        switchDevice(m_available[m_currentApi][0]);
        gui->status->pushMessage("Switched API to " + m_currentApi);
    }
}

void DeviceBar::switchSampleRate(double sampleRate)
{
    switchDevice(m_currentDev, sampleRate);
    gui->status->pushMessage("Changed sample rate to " + str(sampleRate, "Hz"));
}

void DeviceBar::start()
{
    initialize();
}

void DeviceBar::update() {
    render();
}

void DeviceBar::render()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 7));
    ImGui::BeginFixed(getName().c_str(), rect.getPosition(), rect.getSize());
    ImGui::BeginGroup();
    renderApiSelection();
    ImGui::SameLine();
    renderDeviceSelection();
    ImGui::SameLine();
    renderDeviceSampleRates();
    ImGui::SameLine();
    renderDeviceDetails();
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_SYNC_ALT))
        initialize();
    gui->status->showTooltip("Refresh Device List");
    ImGui::EndGroup();
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_HELP"))
        {
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void DeviceBar::renderApiSelection()
{
    ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("##APIs", m_currentApi.c_str()))
    {
        for (auto &pair : m_available)
        {
            bool isSelected = m_currentApi == pair.first;
            if (ImGui::Selectable(pair.first.c_str(), isSelected))
            {
                switchApi(pair.first);
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    gui->status->showTooltip("Select API");
}

void DeviceBar::renderDeviceSelection()
{
    ImGui::PushItemWidth(320);
    std::string devName;
    if (session)
        devName = m_currentDev.name + " [" + std::to_string(m_currentDev.index) + "]";
    if (ImGui::BeginCombo("##Devices", devName.c_str()))
    {
        for (int i = 0; i < m_available[m_currentApi].size(); i++)
        {
            bool selected = (m_currentDev.index == m_available[m_currentApi][i].index);
            if (m_available[m_currentApi][i].index < 10)
                devName = "[" + std::to_string(m_available[m_currentApi][i].index) + "]  " + m_available[m_currentApi][i].name;
            else
                devName = "[" + std::to_string(m_available[m_currentApi][i].index) + "] " + m_available[m_currentApi][i].name;
            if (ImGui::Selectable(devName.c_str(), selected))
                switchDevice(m_available[m_currentApi][i]);
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    gui->status->showTooltip("Select Device");
}

void DeviceBar::renderDeviceSampleRates()
{
    ImGui::PushItemWidth(-55);
    std::string rate;
    if (session)
        rate = str(session->getSampleRate(), "Hz");
    if (ImGui::BeginCombo("##SampleRates", rate.c_str()))
    {
        for (auto &s : m_currentDev.sampleRates)
        {
            rate = str(s, "Hz");
            bool selected = false;
            if (session)
                selected = session->getSampleRate() == s;
            if (ImGui::Selectable(rate.c_str(), selected))
            {
                switchSampleRate(s);
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    gui->status->showTooltip("Select Sample Rate");
}

void DeviceBar::renderDeviceDetails()
{
    if (ImGui::Button("...") && session)
    {
        ImGui::OpenPopup("Device Details");
    }

    gui->status->showTooltip("List Devices");
    bool modalOpen = true;
    if (ImGui::BeginPopupModal("Device Details", &modalOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        // ImGui::SameLine(1);
        ImGui::Text("ID");
        ImGui::SameLine(50);
        ImGui::Text("Name");
        ImGui::SameLine(400);
        ImGui::Text("API");
        ImGui::SameLine(500);
        ImGui::Text("Channels");
        for (auto &pair : session->getAvailableDevices())
        {
            auto d = pair.second;
            auto api = d.apiName;
            if (d.isApiDefault)
                api += "*";
            std::string id = str(d.index);
            if (d.isDefault)
                id += "*";
            // ImGui::Separator();
            if (ImGui::Button(str("##", d.index).c_str(), ImVec2(-1, 0)))
            {
                switchDevice(d);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine(9);
            ImGui::Text(id.c_str());
            ImGui::SameLine(50);
            ImGui::Text(d.name.c_str());
            ImGui::SameLine(400);
            ImGui::Text(api.c_str());
            ImGui::SameLine(500);
            ImGui::Text(str(d.maxChannels).c_str());
        }
        ImGui::EndPopup();
    }
}

void DeviceBar::getCurrent()
{
    if (session) {
        m_currentDev = session->getCurrentDevice();
        m_currentApi = m_currentDev.apiName;
    }
}

void DeviceBar::getAvailable()
{
    m_available.clear();
    if (session) {
        auto devs = session->getAvailableDevices();
        for (auto &dev : devs)
        {
            if (dev.second.isApiDefault)
                m_available[dev.second.apiName].push_front(dev.second);
            else
                m_available[dev.second.apiName].push_back(dev.second);
        }
    }
}