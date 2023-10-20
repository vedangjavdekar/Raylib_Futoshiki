#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "Events.h"
#include "Actions.h"
#include "Grid.h"

namespace Engine
{
    struct ApplicationProps
    {
        size_t Width;
        size_t Height;
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
        Grid m_Grid;

        bool m_IsRunning;
    };
}