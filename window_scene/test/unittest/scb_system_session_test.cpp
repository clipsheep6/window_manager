/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: check func UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, UpdateWindowMode, Function | SmallTest | Level1)
{
    scbSystemSession_->PresentFocusIfPointDown();
    scbSystemSession_->PresentFoucusIfNeed(2);
    ASSERT_EQ(WSError::WS_OK, scbSystemSession_->SetSystemSceneBlockingFocus(true));
    WSRect rect = {0, 0, 0, 0};
    scbSystemSession_->UpdatePointerArea(rect);
    auto ret = scbSystemSession_->UpdateWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, ret);
}

/**
 * @tc.name: RegisterBufferAvailableCallback
 * @tc.desc: check func RegisterBufferAvailableCallback
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, RegisterBufferAvailableCallback, Function | SmallTest | Level3)
{
    scbSystemSession_->RegisterBufferAvailableCallback([]() {});
}
}
}
}