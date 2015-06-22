#include "tool_manager3d.h"

#include "select_node_tool.h"
#include "select_slice_tool.h"
#include "qglviewer.h"
#include "object.h"
#include "point_set.h"

using namespace tool_skeleton;

ToolManager3D::ToolManager3D(PaintCanvas *canvas)
: canvas_(canvas)
, current_tool_(NULL)
, last_tool_(NULL)
, last_tool_name_(TOOL_EMPTY)
{
    set_tool(TOOL_EMPTY);
}

ToolManager3D::~ToolManager3D()
{
    delete canvas_;
    canvas_ = NULL;

    delete current_tool_;
    delete last_tool_;

    // set pointers to NULL
    clear();
}

void ToolManager3D::set_tool(ToolName name)
{
    ToolMap::iterator it = tools_.find(name) ;
    if (it == tools_.end()) {
        Tool3D *tool = create_new_tool(name);
        if (tool != NULL) {
            tools_[name] = tool;
        }
    }

    it = tools_.find(name) ;
    if (it == tools_.end()) {
        std::cout << title() << "no such tool" << std::endl;
        current_tool_ = NULL ;
        current_tool_name_ = TOOL_EMPTY;
        return ;
    }
    current_tool_ = it->second ;
    current_tool_name_ = name;
}

tool_skeleton::Tool3D *ToolManager3D::prepare_tool(tool_skeleton::ToolName name)
{
    ToolMap::iterator it = tools_.find(name) ;
    if (it == tools_.end()) {
        Tool3D *tool = create_new_tool(name);
        if (tool != NULL) {
            tools_[name] = tool;
            return tool;
        }
    }

    it = tools_.find(name) ;
    if(it == tools_.end()) {
        std::cout<<title() << "no such tool" << std::endl;
        current_tool_ = NULL ;
        current_tool_name_ = TOOL_EMPTY;
        return NULL;
    }

    return it->second ;
}

void ToolManager3D::status(const std::string &value) {
    std::cout << value << "TODO: prompt user in status bar" << std::endl;
    // status_message() ;
}

PaintCanvas *ToolManager3D::canvas() const {
    return canvas_;
}

void ToolManager3D::clear() {
    current_tool_ = NULL;
    last_tool_ = NULL;
    current_tool_name_ = TOOL_EMPTY;
    last_tool_name_ = TOOL_EMPTY;
}

void ToolManager3D::set_canvas(PaintCanvas* canvas) {
    canvas_ = canvas;
}

Tool3D *ToolManager3D::create_new_tool(tool_skeleton::ToolName name) {
    Tool3D *tool = NULL;
    switch (name)
    {
    case TOOL_EMPTY:
        tool = new EmptyTool3D(canvas_);
        break;
    case TOOL_SELECT_VERTICES:
        tool = new SelectSliceTool(canvas_);
        break;
    case TOOL_SELECT_NODE:
        tool = new SelectOneNodeTool(canvas_);
        break;
    case TOOL_SELECT_TWONODES:
        tool = new SelectTwoNodeTool(canvas_);
        break;
    case TOOL_SELECT_BONE:
        tool = new SelectOneBoneTool(canvas_);
        break;
    default:
        std::cout << title() << "no such tool defined" << std::endl;
        break;
    }

    return tool;
}

void ToolManager3D::record_current_tool() {
    last_tool_ = current_tool_;
    last_tool_name_ = current_tool_name_;

    current_tool_ = NULL;
    current_tool_name_ = TOOL_EMPTY;
}

void ToolManager3D::restore_last_tool() {
    current_tool_ = last_tool_;
    current_tool_name_ = last_tool_name_;

    last_tool_ = NULL;
    last_tool_name_ = TOOL_EMPTY;
}