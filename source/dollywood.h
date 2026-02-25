#ifndef DOLLYWOOD_H__
#define DOLLYWOOD_H__

#include "c4d.h"
#include "c4d_tooldata.h"


#define PLUGIN_ID_DOLLYWOOD 1053605

class DollyWoodToolData : public cinema::ToolData
{
public:
    virtual maxon::Int32 GetState(cinema::BaseDocument* doc);
    virtual maxon::Bool MouseInput(cinema::BaseDocument* doc, cinema::BaseContainer& data, cinema::BaseDraw* bd, cinema::EditorWindow* win, const cinema::BaseContainer& msg);
    virtual maxon::Bool GetCursorInfo(cinema::BaseDocument* doc, cinema::BaseContainer& data, cinema::BaseDraw* bd, maxon::Float x, maxon::Float y, cinema::BaseContainer& bc);
};

maxon::Bool RegisterDollyWood();

#endif