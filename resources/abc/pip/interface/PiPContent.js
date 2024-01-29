const pip = requireNapi("pip");

const TAG = "PiPContent";

export class PiPContent extends ViewPU {
    constructor(e, o, t, n = -1, i = void 0) {
        super(e, t, n);
        "function" == typeof i && (this.paramsGenerator_ = i);
        this.xComponentController = new XComponentController;
        this.xComponentId = "pipContent";
        this.setInitiallyProvidedValue(o);
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.xComponentController && (this.xComponentController = e.xComponentController);
        void 0 !== e.xComponentId && (this.xComponentId = e.xComponentId);
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
    }

    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal()
    }

    initialRender() {
        this.observeComponentCreation2(((e, o) => {
            Stack.create();
            Stack.size({ width: "100%", height: "100%" });
        }), Stack);
        this.observeComponentCreation2(((e, o) => {
            XComponent.create({
                id: this.xComponentId,
                type: "surface",
                controller: this.xComponentController
            }, "pipContent_XComponent");
            XComponent.onLoad((() => {
                pip.initXComponentController(this.xComponentController);
                console.debug(TAG, "XComponent onLoad done");
            }));
            XComponent.size({ width: "100%", height: "100%" });
        }), XComponent);
        Stack.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new PiPContent(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();