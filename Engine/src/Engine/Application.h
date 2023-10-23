#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "Events.h"
#include "Actions.h"
#include "Grid.h"
#include "LevelSelection.h"
#include "Notifications.h"

namespace Engine
{
    struct ApplicationProps
    {
        int Width;
        int Height;
        std::string Title;
    };

    class Application
    {
    public:
        Application(ApplicationProps props);
        ~Application();

        void Run();
        void Close();
        
        static Application& Get();
        
        void AddEvent(const Event& customEvent);
        Notifications& GetNotifications();
        
    protected:
        void Init();
        void Update(const float deltaTime);
        void Draw();

        void ProcessEvents();
        void SetupKeybindings();
    protected:
        static Application* s_Instance;
        ApplicationProps m_ApplicationProps;

        ActionMap m_ActionMap;
        
        std::vector<Event> m_EventQueue;
        std::vector<Event> m_PropagatedEventQueue;

        Grid m_Grid;
        LevelSelection m_LevelSelection;
        Notifications m_Notifications;

        bool m_IsRunning;
    };
}