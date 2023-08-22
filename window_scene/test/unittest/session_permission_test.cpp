#include <gtest/gtest.h>
#include <libxml/globals.h>
#include <libxml/xmlstring.h>
#include "window_scene_config.h"
#include "window_manager_hilog.h"
#include "common/include/session_permission.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionPermissionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionPermissionTest::SetUpTestCase()
{
}

void SessionPermissionTest::TearDownTestCase()
{
}

void SessionPermissionTest::SetUp()
{
}

void SessionPermissionTest::TearDown()
{
}
namespace{

/**
 * @tc.name: IsSystemServiceCalling
 * @tc.desc: test function : IsSystemServiceCalling
 * @tc.type: FUNC
 */    
HWTEST_F(SessionPermissionTest, IsSystemServiceCalling, Function | SmallTest | Level1)
{ 
    bool needPrintLog = true;
    bool result = SessionPermission::IsSystemServiceCalling(needPrintLog);
    ASSERT_EQ(result, false);
}
/**
 * @tc.name: IsSACalling
 * @tc.desc: test function : IsSACalling
 * @tc.type: FUNC
 */    
HWTEST_F(SessionPermissionTest, IsSACalling, Function | SmallTest | Level1)
{ 
    bool result = SessionPermission::IsSACalling();
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: VerifyCallingPermission
 * @tc.desc: test function : VerifyCallingPermission1
 * @tc.type: FUNC
*/    
HWTEST_F(SessionPermissionTest, VerifyCallingPermission, Function | SmallTest | Level1)
{
    const std::string *permissionNode =new string;
    bool result = SessionPermission::VerifyCallingPermission(*permissionNode);
    ASSERT_EQ(false, result);
}
 
/**
 * @tc.name: VerifySessionPermission
 * @tc.desc: test function : VerifySessionPermission
 * @tc.type: FUNC
*/    
HWTEST_F(SessionPermissionTest, VerifySessionPermission, Function | SmallTest | Level1)
{
    bool result = SessionPermission::VerifySessionPermission();
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: JudgeCallerIsAllowedToUseSystemAPI
 * @tc.desc: test function : JudgeCallerIsAllowedToUseSystemAPI
 * @tc.type: FUNC
*/    
HWTEST_F(SessionPermissionTest, JudgeCallerIsAllowedToUseSystemAPI, Function | SmallTest | Level1)
{
    bool result = SessionPermission::JudgeCallerIsAllowedToUseSystemAPI();
    ASSERT_EQ(true, result);
}
 

 

} // namespacecd 
} // namespace Rosen
} // namespace OHOS
