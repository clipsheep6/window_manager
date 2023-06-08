

#include "rs_surface_node.h"

#include <algorithm>
#include <string>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "RsSurfaceNode"};
}

RSSurfaceNode::SharedPtr RSSurfaceNode::Create(const RSSurfaceNodeConfig& surfaceNodeConfig,
    RSSurfaceNodeType type, bool isWindow)
{
    WLOGFI("RSSurfaceNode::Create mlx %{public}d", __LINE__);
    // auto transactionProxy = RSTransactionProxy::GetInstance();
    // if (transactionProxy == nullptr) {
    //     return nullptr;
    // }

    SharedPtr node(new RSSurfaceNode(surfaceNodeConfig, isWindow));
    if (node == nullptr) {
        WLOGFI("RSSurfaceNode::Create mlx %{public}d", __LINE__);
        return nullptr;
    }
    // RSNodeMap::MutableInstance().RegisterNode(node);

    // // create node in RS
    // RSSurfaceRenderNodeConfig config = {
    //     .id = node->GetId(),
    //     .name = node->name_,
    //     .additionalData = surfaceNodeConfig.additionalData,
    // };
    // if (!isWindow) {
    //     config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    // } else {
    //     config.nodeType = type;
    // }

    // RS_LOGD("RSSurfaceNode::Create %s type %d", config.name.c_str(), config.nodeType);

    // if (!node->CreateNodeAndSurface(config)) {
    //     ROSEN_LOGE("RSSurfaceNode::Create, create node and surface failed");
    //     return nullptr;
    // }

    // node->SetClipToFrame(true);
    // // create node in RT (only when in divided render and isRenderServiceNode_ == false)
    // if (!node->IsRenderServiceNode()) {
    //     std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeCreate>(node->GetId());
    //     transactionProxy->AddCommand(command, isWindow);

    //     command = std::make_unique<RSSurfaceNodeConnectToNodeInRenderService>(node->GetId());
    //     transactionProxy->AddCommand(command, isWindow);

    //     command = std::make_unique<RSSurfaceNodeSetCallbackForRenderThreadRefresh>(
    //         node->GetId(), [] { RSRenderThread::Instance().RequestNextVSync(); });
    //     transactionProxy->AddCommand(command, isWindow);
    //     node->SetFrameGravity(Gravity::RESIZE);
    // }

    // if (node->GetName().find("battery_panel") != std::string::npos ||
    //     node->GetName().find("sound_panel") != std::string::npos ||
    //     node->GetName().find("RosenWeb") != std::string::npos) {
    //     node->SetFrameGravity(Gravity::TOP_LEFT);
    // } else if (!isWindow) {
    //     node->SetFrameGravity(Gravity::RESIZE);
    // }
    // ROSEN_LOGD("RsDebug RSSurfaceNode::Create id:%" PRIu64, node->GetId());
    return node;
}



} // namespace Rosen
} // namespace OHOS