#include "dollywood.h"
#include "c4d_basecontainer.h"
#include "c4d_baseobject.h"
#include "c4d_general.h"

using namespace cinema;
using namespace maxon;

Int32 DollyWoodToolData::GetState(BaseDocument* doc)
{
    return CMD_ENABLED;
}

Bool DollyWoodToolData::GetCursorInfo(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, Float x, Float y, BaseContainer& bc)
{
    if (bc.GetId() == BFM_CURSORINFO_REMOVE)
        return true;

    bc.SetInt32(RESULT_CURSOR, MOUSE_CROSS);
    return true;
}

Bool DollyWoodToolData::MouseInput(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, EditorWindow* win, const BaseContainer& msg)
{
    // INPUT DATA
    Float mx = msg.GetFloat(BFM_INPUT_X);
    Float my = msg.GetFloat(BFM_INPUT_Y);

    if (msg.GetInt32(BFM_INPUT_CHANNEL) != BFM_INPUT_MOUSELEFT)
        return true;

    // CAMERA
    CameraObject* activeCamera = (CameraObject*)bd->GetSceneCamera(doc);
    if (!activeCamera)
        activeCamera = (CameraObject*)doc->GetRenderBaseDraw()->GetSceneCamera(doc);

    if (!activeCamera || !activeCamera->IsInstanceOf(Ocamera))
        return false;

    // TARGET DEFINITION (PRIORITY: DOLLYTARGET IN SCENE)
    BaseObject* targetObject = doc->SearchObject("DollyTarget"_s);
    
    // Fallback to active object only if DollyTarget is not found
    if (!targetObject || !targetObject->IsInstanceOf(Onull))
    {
        targetObject = doc->GetActiveObject();
    }

    // CONSTANTS (FIX BEFORE DRAG!)
    GeData fovData;
    activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_FOV), fovData, DESCFLAGS_GET::NONE);
    Float originalFOV = fovData.GetFloat();

    // SENSOR WIDTH (for focal length calculation)
    GeData apertureData;
    activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_APERTURE), apertureData, DESCFLAGS_GET::NONE);
    Float sensorWidth = apertureData.GetFloat();
    if (sensorWidth <= 0.0) sensorWidth = 36.0;  // default full-frame 36mm

    Matrix originalMatrix = activeCamera->GetMg();
    Vector originalViewDir = originalMatrix.sqmat.v3;
    originalViewDir.Normalize();

    Float originalDistance;
    Vector fixedSubjectPos;

    // TARGET POINT CALCULATION
    if (targetObject)
    {
        Vector targetPos = targetObject->GetMg().off;
        
        // COMPONENT SELECTION SUPPORT
        if (targetObject->IsInstanceOf(Opoint))
        {
            PointObject* po = (PointObject*)targetObject;
            const Vector* points = po->GetPointR();
            Int32 pointCount = po->GetPointCount();
            
            Vector sumPos(0);
            Int32 selectedCount = 0;

            // 1. Check Points
            const BaseSelect* ptSel = po->GetPointS();
            if (ptSel && ptSel->GetCount() > 0)
            {
                for (Int32 i = 0; i < pointCount; ++i)
                {
                    if (ptSel->IsSelected(i))
                    {
                        sumPos += points[i];
                        selectedCount++;
                    }
                }
            }
            // 2. Check Edges (if no points selected)
            else if (targetObject->IsInstanceOf(Opolygon))
            {
                PolygonObject* polyObj = (PolygonObject*)targetObject;
                const BaseSelect* edgeSel = polyObj->GetEdgeS();
                if (edgeSel && edgeSel->GetCount() > 0)
                {
                    // Use a simple bitset or boolean array to avoid double-counting points
                    // For brevity in this tool, we'll use a local array. 
                    // In a production environment, maxon::BaseArray<Bool> or similar would be better.
                    maxon::BaseArray<Bool> usedPoints;
                    usedPoints.Resize(pointCount) iferr_ignore("Point map resize");
                    for (Int32 i = 0; i < pointCount; i++) usedPoints[i] = false;

                    const CPolygon* polys = polyObj->GetPolygonR();
                    Int32 polyCount = polyObj->GetPolygonCount();

                    for (Int32 i = 0; i < polyCount; ++i)
                    {
                        const CPolygon& p = polys[i];
                        for (Int32 side = 0; side < 4; ++side)
                        {
                            if (side == 3 && p.c == p.d) continue; // Triangle
                            
                            Int32 edgeIdx = i * 4 + side;
                            if (edgeSel->IsSelected(edgeIdx))
                            {
                                Int32 a = (side == 0) ? p.a : (side == 1) ? p.b : (side == 2) ? p.c : p.d;
                                Int32 b = (side == 0) ? p.b : (side == 1) ? p.c : (side == 2) ? p.d : p.a;
                                
                                if (!usedPoints[a]) { sumPos += points[a]; selectedCount++; usedPoints[a] = true; }
                                if (!usedPoints[b]) { sumPos += points[b]; selectedCount++; usedPoints[b] = true; }
                            }
                        }
                    }
                }
                // 3. Check Polygons (if no points or edges selected)
                if (selectedCount == 0)
                {
                    const BaseSelect* polySel = polyObj->GetPolygonS();
                    if (polySel && polySel->GetCount() > 0)
                    {
                        maxon::BaseArray<Bool> usedPoints;
                        usedPoints.Resize(pointCount) iferr_ignore("Point map resize");
                        for (Int32 i = 0; i < pointCount; i++) usedPoints[i] = false;

                        const CPolygon* polys = polyObj->GetPolygonR();
                        Int32 polyCount = polyObj->GetPolygonCount();

                        for (Int32 i = 0; i < polyCount; ++i)
                        {
                            if (polySel->IsSelected(i))
                            {
                                const CPolygon& p = polys[i];
                                Int32 indices[4] = { p.a, p.b, p.c, p.d };
                                Int32 sides = (p.c == p.d) ? 3 : 4;
                                for (Int32 s = 0; s < sides; ++s)
                                {
                                    Int32 idx = indices[s];
                                    if (!usedPoints[idx])
                                    {
                                        sumPos += points[idx];
                                        selectedCount++;
                                        usedPoints[idx] = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (selectedCount > 0)
            {
                targetPos = targetObject->GetMg() * (sumPos / (Float)selectedCount);
            }
        }

        Vector toTarget = targetPos - originalMatrix.off;
        originalDistance = toTarget.x * originalViewDir.x + toTarget.y * originalViewDir.y + toTarget.z * originalViewDir.z;

        if (originalDistance < 10.0) originalDistance = 200.0;
        fixedSubjectPos = originalMatrix.off + originalViewDir * originalDistance;
    }
    else
    {
        // DOF Auto Mode: target = world origin (0, 0, 0)
        Vector sceneCenter(0.0, 0.0, 0.0);

        Vector toCenter = sceneCenter - originalMatrix.off;
        originalDistance = toCenter.x * originalViewDir.x
                         + toCenter.y * originalViewDir.y
                         + toCenter.z * originalViewDir.z;

        if (originalDistance < 10.0) originalDistance = 200.0;

        fixedSubjectPos = originalMatrix.off + originalViewDir * originalDistance;
    }

    if (originalDistance <= 0.01) originalDistance = 200.0;
    
    Float tanHalfOrigFOV = cinema::Tan(originalFOV * 0.5);
    Float origMx = mx;

    // INITIAL VALUES IN STATUS BAR
    Float startFOVDeg = ::maxon::RadToDeg((::maxon::Float64)originalFOV);
    Float startFocalLength = (sensorWidth * 0.5) / cinema::Tan(originalFOV * 0.5);
    
    ::maxon::String targetInfo = targetObject ? "Target: DollyTarget"_s : "Target: World Origin"_s;
    StatusSetText(FormatString("Dolly Zoom | @ | FOV: @° | Focal: @ mm | Dist: @ cm",
        targetInfo,
        ::maxon::String::FloatToString(startFOVDeg, -1, 1),
        ::maxon::String::FloatToString(startFocalLength, -1, 1),
        ::maxon::String::FloatToString(originalDistance, -1, 1))
    );

    // UNDO
    doc->StartUndo();
    doc->AddUndo(UNDOTYPE::CHANGE, activeCamera);
    if (targetObject)
        doc->AddUndo(UNDOTYPE::CHANGE, targetObject);

    // MOUSE DRAG
    BaseContainer device;
    win->MouseDragStart(KEY_MLEFT, mx, my,
                       MOUSEDRAGFLAGS::DONTHIDEMOUSE | MOUSEDRAGFLAGS::NOMOVE);

    Float dx, dy;
    while (win->MouseDrag(&dx, &dy, &device) == MOUSEDRAGRESULT::CONTINUE)
    {
        if (dx == 0.0 && dy == 0.0) continue;

        mx += dx;
        Float deltaX = mx - origMx;

        // SPEED (Shift/Control)
        Float amount = 1.0;
        Int32 qualifier = device.GetInt32(BFM_INPUT_QUALIFIER);

        if (qualifier & QSHIFT)
            amount = 1.0 + deltaX / 5000.0;
        else
            amount = 1.0 + deltaX / 125.0; // Default speed for no key OR result of snapping logic

        if (amount <= 0.01) amount = 0.01;

        // DOLLY ZOOM
        Float newFOV = originalFOV / amount;

        // Snapping: if Ctrl is pressed, lock to integer focal length
        if (qualifier & QCTRL)
        {
            Float focalMm = (sensorWidth * 0.5) / cinema::Tan(newFOV * 0.5);
            Float snapped = cinema::Floor(focalMm + 0.5);
            if (snapped < 1.0) snapped = 1.0;
            newFOV = 2.0 * cinema::ATan(sensorWidth * 0.5 / snapped);
        }

        if (newFOV > 0.001 && newFOV < maxon::PI * 0.99)
        {
            Float tanHalfNewFOV = cinema::Tan(newFOV * 0.5);
            Float newDistance = originalDistance * tanHalfOrigFOV / tanHalfNewFOV;
            if (newDistance < 0.01) newDistance = 0.01;

            Vector newCamPos = fixedSubjectPos - originalViewDir * newDistance;

            // APPLY CHANGES
            activeCamera->SetParameter(ConstDescIDLevel(CAMERAOBJECT_FOV),
                                       GeData(newFOV), DESCFLAGS_SET::NONE);
            
            if (targetObject)
            {
                activeCamera->SetParameter(ConstDescIDLevel(CAMERAOBJECT_TARGETDISTANCE),
                                           GeData(newDistance), DESCFLAGS_SET::NONE);
            }

            Matrix newMatrix = originalMatrix;
            newMatrix.off = newCamPos;
            activeCamera->SetMg(newMatrix);

            // OUTPUT FOV + FOCAL LENGTH TO STATUS BAR
            Float fovDegrees  = ::maxon::RadToDeg((::maxon::Float64)newFOV);
            Float focalLength = (sensorWidth * 0.5) / tanHalfNewFOV;

            ::maxon::String info = "Dolly Zoom | FOV: "_s + ::maxon::String::FloatToString(fovDegrees, -1, 1)
                        + "° | Focal: "_s + ::maxon::String::FloatToString(focalLength, -1, 1)
                        + "mm | Dist: "_s + ::maxon::String::FloatToString(newDistance, -1, 1);
            StatusSetText(info);
        }

        DrawViews(DRAWFLAGS::ONLY_ACTIVE_VIEW | DRAWFLAGS::NO_THREAD | DRAWFLAGS::NO_ANIMATION);
    }

    // FINISH
    if (win->MouseDragEnd() == MOUSEDRAGRESULT::ESCAPE)
    {
        doc->DoUndo(true);
        StatusSetText("Dolly Zoom cancelled"_s);
    }
    else
    {
        // FINAL VALUES
        GeData finalFovData;
        activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_FOV), finalFovData, DESCFLAGS_GET::NONE);
        Float finalFOV = finalFovData.GetFloat();
        Float finalTan = cinema::Tan(finalFOV * 0.5);
        Float finalFovDeg = ::maxon::RadToDeg((::maxon::Float64)finalFOV);
        Float finalFocal  = (sensorWidth * 0.5) / finalTan;

        GeData finalDistData;
        activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_TARGETDISTANCE), finalDistData, DESCFLAGS_GET::NONE);
        Float finalDist = finalDistData.GetFloat();

        ::maxon::String info = "Done | FOV: "_s + ::maxon::String::FloatToString(finalFovDeg, -1, 1)
                    + "° | Focal: "_s + ::maxon::String::FloatToString(finalFocal, -1, 1)
                    + "mm | Dist: "_s + ::maxon::String::FloatToString(finalDist, -1, 1);
        StatusSetText(info);
    }

    doc->EndUndo();
    EventAdd();
    return true;
}

Bool RegisterDollyWood()
{
    return RegisterToolPlugin(
        PLUGIN_ID_DOLLYWOOD,
        "Dolly Zoom Tool"_s,
        0,
        AutoBitmap("dollywood.png"_s),
        "Dolly Zoom PERFECT\n- Automatically uses DollyTarget\n- Selected object as fallback\n- Shift/Control = speed"_s,
        NewObjClear(DollyWoodToolData)
    );
}

namespace cinema
{
Bool PluginStart()
{
    return ::RegisterDollyWood();
}

void PluginEnd()
{
}

Bool PluginMessage(Int32 id, void* data)
{
    switch (id)
    {
        case C4DPL_INIT_SYS:
            if (!g_resource.Init()) return false;
            return true;
        case C4DMSG_PRIORITY:
            return true;
        case C4DPL_BUILDMENU:
            break;
    }
    return false;
}
}
