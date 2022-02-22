#include <cstdio>
#include <unistd.h>
#include <refbase.h>
#include "screen.h"
#include "screen_manager.h"
#include "display_manager.h"
#include "window.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_display_node.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
    class MyScreenListener : public OHOS::Rosen::ScreenManager::IScreenListener {
        void OnConnect(ScreenId) {
        }
        void OnDisconnect(ScreenId) {
        }
        void OnChange(const std::vector<ScreenId>& array, ScreenChangeEvent) {
            auto screen = ScreenManager::GetInstance().GetScreenById(array[0]);
            printf("ScreenManager::OnChange id=%u, w/h=%u/%u",
                static_cast<uint32_t>(array[0]), screen->GetWidth(), screen->GetHeight());
        }
    };

    class MyDisplayListener : public OHOS::Rosen::DisplayManager::IDisplayListener {
    public:
        void OnCreate(DisplayId) {
        }
        void OnDestroy(DisplayId) {
        }
        void OnChange(DisplayId id, DisplayChangeEvent event) {
            auto display = DisplayManager::GetInstance().GetDisplayById(id);
            printf("DisplayManager::OnChange id=%u, w/h=%d/%d",
                static_cast<uint32_t>(id), display->GetWidth(), display->GetHeight());
        }
    };
}

int main(int argc, char *argv[])
{
    // 创建VirtualScreen
    sptr<Surface> csurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferProducer> producer = csurface->GetProducer();
    sptr<Surface> psurface = Surface::CreateSurfaceAsProducer(producer);
    psurface = Surface::CreateSurfaceAsProducer(producer);
    VirtualScreenOption option = {"virtualscreen", 480, 320, 1.0, psurface, 0, false};
    ScreenId screenId = ScreenManager::GetInstance().CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        printf("CreateVirtualScreen fail\n");
        return -1;
    }

    // 组件扩展
    std::vector<ExpandOption> options;
    options.push_back({screenId, 0, 0});
    options.push_back({0, 480, 320});
    ScreenId groupId = ScreenManager::GetInstance().MakeExpand(options);
    if (groupId == SCREEN_ID_INVALID) {
        printf("MakeExpand fail\n");
        return -1;
    }

    // 查找扩展屏的Display
    sptr<Display> display = DisplayManager::GetInstance().GetDisplayByScreen(screenId);
    if (display == nullptr) {
        printf("Cannot get display from virtualScreen\n");
        return -1;
    }

    sptr<WindowOption> winop;
    winop->SetDisplayId(display->GetId());
    sptr<Window> win = Window::Create("widnow", winop);
    ScreenManager::GetInstance().DestroyVirtualScreen(screenId);
    return 0;
}