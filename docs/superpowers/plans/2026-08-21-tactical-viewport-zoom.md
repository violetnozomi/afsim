# Tactical Viewport Zoom Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace whole-widget scaling with map-style, cursor-anchored tactical viewport zoom and drag panning while keeping overlays and resource forms fixed.

**Architecture:** Add a focused Qt geometry helper that owns the content-to-view transform, zoom anchor preservation, pan bounds, and fit reset. `NrmTacticalView` will apply that transform only while drawing the topology layer; its title, summary, legends, selection prompt, and details card remain in widget coordinates. The Dock remains the percentage/control owner and receives wheel-driven percentage changes from the tactical view.

**Tech Stack:** C++17-compatible project code, Qt 5.12 Widgets/Gui/Core, existing CMake and assert-based NRM tests.

**Spec:** User-confirmed chat design on 2026-08-21: map-style local zoom, drag panning, fit-all reset, 50%–500%, fixed overlays and forms.

## Global Constraints

- Modify only the independent `network_resource_manager` extension; AFSIM core modification count stays zero.
- Do not alter route selection, gateway policy, simulation state, JSON contracts, or stable enum codes.
- Preserve node click details and source/destination tactical selection after all transforms.
- Keep `Link-11`, `Link-16`, `SATCOM`, and `CDL` display names unchanged.
- Do not add external dependencies, commit, push, or change user service state.

---

### Task 1: Viewport transform model

**Files:**
- Create: `warlock/source/NrmTacticalViewport.hpp`
- Create: `warlock/source/NrmTacticalViewport.cpp`
- Modify: `warlock/source/NrmUiScale.hpp`
- Modify: `tests/UiScaleTest.cpp`
- Modify: `CMakeLists.txt`
- Modify: `warlock/source/CMakeLists.txt`

**Interfaces:**
- Consumes: `QRectF`, `QPointF`, `QTransform`, and `WkNrm::UiScale` bounds.
- Produces: `TacticalViewport::SetPlotRect`, `SetScalePercent`, `PanBy`, `FitAll`, `ContentToView`, `ViewToContent`, `Transform`, and `ScalePercent`; 50%–500% bounds in 25% steps.

- [x] **Step 1: Write failing cursor-anchor, inverse-map, pan, clamp, and fit tests**

```cpp
WkNrm::TacticalViewport viewport;
viewport.SetPlotRect(QRectF(100.0, 80.0, 1000.0, 600.0));
const QPointF anchor(750.0, 350.0);
const QPointF contentAtAnchor = viewport.ViewToContent(anchor);
viewport.SetScalePercent(200, anchor);
assert(Distance(viewport.ContentToView(contentAtAnchor), anchor) < 0.001);
assert(Distance(viewport.ViewToContent(viewport.ContentToView(QPointF(800.0, 400.0))),
                QPointF(800.0, 400.0)) < 0.001);
viewport.PanBy(QPointF(-100.0, 50.0));
viewport.FitAll();
assert(viewport.ScalePercent() == 100);
```

- [x] **Step 2: Build the focused test and confirm it fails because the viewport type is absent**

Run:

```bash
cmake --build /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24 --target nrm_ui_scale_test -j 4
```

Expected: compilation failure for missing `NrmTacticalViewport.hpp`.

- [x] **Step 3: Implement the bounded affine viewport transform**

```cpp
class TacticalViewport
{
public:
   void SetPlotRect(const QRectF& aPlotRect);
   void SetScalePercent(int aPercent, const QPointF& aViewAnchor);
   void PanBy(const QPointF& aViewDelta);
   void FitAll();
   QPointF ContentToView(const QPointF& aContentPoint) const;
   QPointF ViewToContent(const QPointF& aViewPoint) const;
   QTransform Transform() const;
   int ScalePercent() const;
};
```

Clamp pan to `±(scale - 1) * plotSize / 2` at scale above 100%; center content below 100%. Preserve the content coordinate under the supplied anchor during zoom. Update the shared scale constants in this same red-green cycle because the viewport test exercises the confirmed 200% behavior.

- [x] **Step 4: Build and run `nrm_ui_scale_test` until all viewport assertions pass**

Run:

```bash
cmake --build /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24 --target nrm_ui_scale_test -j 4
/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24/nrm_ui_scale_test
```

Expected: exit code 0.

### Task 2: Tactical view drawing and interaction

**Files:**
- Modify: `warlock/source/NrmTacticalView.hpp`
- Modify: `warlock/source/NrmTacticalView.cpp`
- Modify: `warlock/source/NrmPlugin.cpp`

**Interfaces:**
- Consumes: `TacticalViewport` from Task 1 and existing `UiScale` increase/decrease helpers.
- Produces: wheel zoom around cursor, left-background drag pan, fixed overlays, inverse-transformed node hit testing, and `UiScalePercentChanged(int)`.

- [x] **Step 1: Replace whole-widget logical scaling with a fixed widget/plot rectangle**

Draw the background, plot frame, title, summary, details, prompt, and legends with the identity painter transform. Wrap only grid/topology/link/route/node/label drawing in:

```cpp
painter.save();
painter.setClipRect(mPlotRect.adjusted(1.0, 1.0, -1.0, -1.0));
painter.setTransform(mViewport.Transform(), true);
// topology content
painter.restore();
```

- [x] **Step 2: Add cursor-anchored wheel zoom**

```cpp
void TacticalView::wheelEvent(QWheelEvent* aEventPtr)
{
   const int next = aEventPtr->angleDelta().y() > 0
                       ? UiScale::IncreasePercent(mViewport.ScalePercent())
                       : UiScale::DecreasePercent(mViewport.ScalePercent());
   mViewport.SetScalePercent(next, aEventPtr->posF());
   emit UiScalePercentChanged(next);
   update();
}
```

Only consume the event when the cursor is inside the plot and outside the fixed details card.

- [x] **Step 3: Add click-versus-drag handling**

On left press inside the plot, remember the position. Once movement exceeds `QApplication::startDragDistance()`, pan by each screen-space delta and show a closed-hand cursor. On release without a drag, map the screen point with `ViewToContent` before `HitTestPlatform`; on drag release, do not select a node.

- [x] **Step 4: Synchronize wheel zoom back to the Dock and compile the plugin**

Connect `TacticalView::UiScalePercentChanged` to `DockWidget::SetUiScalePercent`. Existing Dock-to-view signaling remains guarded by equal percentage, preventing recursion.

Run:

```bash
cmake --build /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24 --target NetworkResourceManager -j 4
```

Expected: plugin compiles and links successfully.

### Task 3: Controls, regression verification, and documentation

**Files:**
- Modify: `tests/UiScaleTest.cpp`
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `README.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Consumes: completed viewport interaction from Tasks 1–2.
- Produces: 50%–500% controls in 25% steps, “适配全图” reset wording, current operator instructions, and fresh verification evidence.

- [x] **Step 1: Update control wording**

Rename the reset button and tooltip to “适配全图”; keep `Ctrl+Shift+0` because Warlock owns `Ctrl+0`. Add an operator hint for wheel zoom and background dragging. The 50%–500% bounds and 25% step were introduced in Task 1 so its 200% viewport test could use the final policy.

- [x] **Step 2: Run focused and full automated verification**

```bash
scripts/ai_guard.sh static
scripts/ai_guard.sh test
ctest --test-dir /home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24 -R '^nrm_' --output-on-failure
git diff --check
```

Expected: 40/40 fixed tests and 44/44 NRM CTest pass with no static failures.

- [x] **Step 3: Verify in an isolated VNC process**

Start a second Warlock with an isolated temporary runtime root. Confirm wheel zoom keeps the cursor location stable, dragging moves the zoomed topology, fixed overlays/forms do not resize, node detail and source/destination selection still hit the intended node, and “适配全图” returns to 100%.

- [x] **Step 4: Record only observed evidence**

Update current capability and validation documents. Preserve the rejected whole-widget and whole-content scaling entries as historical records, explicitly superseded by the 2026-08-21 viewport implementation. Do not commit or push.
