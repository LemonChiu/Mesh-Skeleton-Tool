#ifndef _TOOL_MANAGER3D_H_
#define _TOOL_MANAGER3D_H_

#include "tool_name.h"
#include "tool3D.h"
#include <map>

class PaintCanvas;

class ToolManager3D
{
public:
    ToolManager3D(PaintCanvas *canvas);
    virtual ~ToolManager3D();

    static std::string title() { return "[ToolManager3D]: "; }
    tool_skeleton::Tool3D *current_tool() { return current_tool_ ; }
    tool_skeleton::ToolName current_tool_name() const { return current_tool_name_; }
    void set_tool(tool_skeleton::ToolName name) ;
    void status(const std::string &value) ;
    virtual PaintCanvas *canvas() const;
    virtual void set_canvas(PaintCanvas *canvas);

    // switch between transformation and tool functions
    void record_current_tool();
    void restore_last_tool();

    tool_skeleton::Tool3D *last_tool() { return last_tool_ ; }
    tool_skeleton::ToolName last_tool_name() const { return last_tool_name_; }
    // prepare a tool specified by name
    tool_skeleton::Tool3D *prepare_tool(tool_skeleton::ToolName name);

    void clear() ;

protected:
    PaintCanvas *canvas_;
    tool_skeleton::Tool3D *current_tool_ ;
    tool_skeleton::Tool3D *last_tool_ ;
    tool_skeleton::ToolName current_tool_name_;
    tool_skeleton::ToolName last_tool_name_;
    typedef std::map<tool_skeleton::ToolName, Tool3D_var> ToolMap;
    ToolMap tools_;

    virtual tool_skeleton::Tool3D *create_new_tool(tool_skeleton::ToolName name);
};

#endif
