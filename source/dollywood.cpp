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
    // ── ВХОДНЫЕ ДАННЫЕ ──
    Float mx = msg.GetFloat(BFM_INPUT_X);
    Float my = msg.GetFloat(BFM_INPUT_Y);

    if (msg.GetInt32(BFM_INPUT_CHANNEL) != BFM_INPUT_MOUSELEFT)
        return true;

    // ── КАМЕРА ──
    CameraObject* activeCamera = (CameraObject*)bd->GetSceneCamera(doc);
    if (!activeCamera)
        activeCamera = (CameraObject*)doc->GetRenderBaseDraw()->GetSceneCamera(doc);

    if (!activeCamera || !activeCamera->IsInstanceOf(Ocamera))
        return false;

    // ── 🎯 TARGET ОПРЕДЕЛЕНИЕ (ПРИОРИТЕТ: DOLLYTARGET В СЦЕНЕ) ──
    BaseObject* targetObject = doc->SearchObject("DollyTarget"_s);
    
    // Fallback на активный объект только если DollyTarget не найден
    if (!targetObject || !targetObject->IsInstanceOf(Onull))
    {
        targetObject = doc->GetActiveObject();
    }

    // ── ✅ КОНСТАНТЫ (ФИКСИРУЕМ ДО ДРАГА!) ──
    GeData fovData;
    activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_FOV), fovData, DESCFLAGS_GET::NONE);
    Float originalFOV = fovData.GetFloat();

    // ── 📐 SENSOR WIDTH (для расчёта focal length) ──
    GeData apertureData;
    activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_APERTURE), apertureData, DESCFLAGS_GET::NONE);
    Float sensorWidth = apertureData.GetFloat();
    if (sensorWidth <= 0.0) sensorWidth = 36.0;  // default full-frame 36mm

    Matrix originalMatrix = activeCamera->GetMg();
    Vector originalViewDir = originalMatrix.sqmat.v3;
    originalViewDir.Normalize();

    Float originalDistance;
    Vector fixedSubjectPos;

    // ── 🎯 РАСчёт ЦЕЛЕВОЙ ТОЧКИ ──
    if (targetObject)
    {
        Vector targetPos = targetObject->GetMg().off;
        Vector toTarget = targetPos - originalMatrix.off;
        originalDistance = toTarget.x * originalViewDir.x + toTarget.y * originalViewDir.y + toTarget.z * originalViewDir.z;

        if (originalDistance < 10.0) originalDistance = 200.0;
        fixedSubjectPos = originalMatrix.off + originalViewDir * originalDistance;
    }
    else
    {
        // Без цели - используем TargetDistance камеры
        GeData distData;
        activeCamera->GetParameter(ConstDescIDLevel(CAMERAOBJECT_TARGETDISTANCE),
                                   distData, DESCFLAGS_GET::NONE);
        originalDistance = distData.GetFloat();
        if (originalDistance <= 10.0) originalDistance = 200.0;

        fixedSubjectPos = originalMatrix.off + originalViewDir * originalDistance;
    }

    if (originalDistance <= 0.01) originalDistance = 200.0;
    
    Float tanHalfOrigFOV = cinema::Tan(originalFOV * 0.5);
    Float origMx = mx;

    // ── 📊 НАЧАЛЬНЫЕ ЗНАЧЕНИЯ В СТАТУС-БАР ──
    Float startFOVDeg = ::maxon::RadToDeg((::maxon::Float64)originalFOV);
    Float startFocalLength = (sensorWidth * 0.5) / cinema::Tan(originalFOV * 0.5);
    
    ::maxon::String targetInfo = targetObject ? "Target: DollyTarget"_s : "Target: Camera"_s;
    StatusSetText(FormatString("Dolly Zoom | @ | FOV: @° | Focal: @ mm | Dist: @ cm",
        targetInfo,
        ::maxon::String::FloatToString(startFOVDeg, -1, 1),
        ::maxon::String::FloatToString(startFocalLength, -1, 1),
        ::maxon::String::FloatToString(originalDistance, -1, 1))
    );

    // ── UNDO ──
    doc->StartUndo();
    doc->AddUndo(UNDOTYPE::CHANGE, activeCamera);
    if (targetObject)
        doc->AddUndo(UNDOTYPE::CHANGE, targetObject);

    // ── 🖱️ MOUSE DRAG ──
    BaseContainer device;
    win->MouseDragStart(KEY_MLEFT, mx, my,
                       MOUSEDRAGFLAGS::DONTHIDEMOUSE | MOUSEDRAGFLAGS::NOMOVE);

    Float dx, dy;
    while (win->MouseDrag(&dx, &dy, &device) == MOUSEDRAGRESULT::CONTINUE)
    {
        if (dx == 0.0 && dy == 0.0) continue;

        mx += dx;
        Float deltaX = mx - origMx;

        // ── СКОРОСТЬ (Shift/Control) ──
        Float amount = 1.0;
        Int32 qualifier = device.GetInt32(BFM_INPUT_QUALIFIER);

        if (qualifier & QSHIFT)
            amount = 1.0 + deltaX / 5000.0;
        else if (qualifier & QCTRL)
            amount = 1.0 + deltaX / 500.0;
        else
            amount = 1.0 + deltaX / 125.0;

        if (amount <= 0.01) amount = 0.01;

        // ── 🧮 DOLLY ZOOM ──
        Float newFOV = originalFOV / amount;
        if (newFOV > 0.001 && newFOV < maxon::PI * 0.99)
        {
            Float tanHalfNewFOV = cinema::Tan(newFOV * 0.5);
            Float newDistance = originalDistance * tanHalfOrigFOV / tanHalfNewFOV;
            if (newDistance < 0.01) newDistance = 0.01;

            Vector newCamPos = fixedSubjectPos - originalViewDir * newDistance;

            // ПРИМЕНЯЕМ ИЗМЕНЕНИЯ
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

            // ── 📊 ВЫВОД FOV + FOCAL LENGTH В СТАТУС-БАР ──
            Float fovDegrees  = ::maxon::RadToDeg((::maxon::Float64)newFOV);
            Float focalLength = (sensorWidth * 0.5) / tanHalfNewFOV;

            ::maxon::String info = "Dolly Zoom | FOV: "_s + ::maxon::String::FloatToString(fovDegrees, -1, 1)
                        + "° | Focal: "_s + ::maxon::String::FloatToString(focalLength, -1, 1)
                        + "mm | Dist: "_s + ::maxon::String::FloatToString(newDistance, -1, 1);
            StatusSetText(info);
        }

        DrawViews(DRAWFLAGS::ONLY_ACTIVE_VIEW | DRAWFLAGS::NO_THREAD | DRAWFLAGS::NO_ANIMATION);
    }

    // ── FINISH ──
    if (win->MouseDragEnd() == MOUSEDRAGRESULT::ESCAPE)
    {
        doc->DoUndo(true);
        StatusSetText("Dolly Zoom cancelled"_s);
    }
    else
    {
        // ── 📊 ФИНАЛЬНЫЕ ЗНАЧЕНИЯ ──
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
        AutoBitmap("dollywood.tif"_s),
        "Dolly Zoom PERFECT\n\u2022 Автоматически использует DollyTarget\n\u2022 Выделенный объект как fallback\n\u2022 Shift/Control = скорость"_s,
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
