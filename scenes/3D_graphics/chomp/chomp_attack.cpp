
#include "chomp_attack.hpp"


#ifdef SCENE_CHOMP_ATTACK

using namespace vcl;


void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{

}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

}


void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();

}

#endif

