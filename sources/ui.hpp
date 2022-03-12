#pragma once

#include <imgui/widgets.hpp>
#include <core/span.hpp>
#include "render/renderer.hpp"
#include "utils/defer.hpp"

namespace ImGui {

struct IdScope {
	IdScope(const char* str) {
		ImGui::PushID(ImGui::GetID(str));
	}

	~IdScope() {
		ImGui::PopID();
	}
};

#define IM_ID_SCOPE(string) IdScope __id(string)

void DragPointLight(PointLight &light) {
	ImGui::DragFloat3("Position", &light.Position[0], 0.1, -20, 20);
	ImGui::DragFloat("Radius", &light.Radius, 0.1, 0, 20);
	ImGui::ColorEdit3("Color", &light.Color[0]);
}

void DragPointLights(Span<PointLight> lights) {
	IM_ID_SCOPE("PointLights");

	for (int i = 0; i<lights.Size(); i++) {
		ImGui::PushID(i);
		ImGui::DragPointLight(lights[i]);
		ImGui::Separator();
		ImGui::PopID();
	}
}

bool NewPointLight(PointLight &light) {
	static constexpr const char *s_PopupName = "Point Light Settings";
	IM_ID_SCOPE(s_PopupName);

	if (ImGui::BeginPopupContextItem(s_PopupName)) {
		defer(ImGui::EndPopup());

		ImGui::DragPointLight(light);

		if(ImGui::Button("Add"))
			return ImGui::CloseCurrentPopup(), true;
	}

	if(ImGui::Button("New Point Light"))
		ImGui::OpenPopup(s_PopupName);

	return false;
}

void DragDirLight(DirLight &light) {
	ImGui::DragFloat3("Direction", &light.Direction[0], 0.1, -20, 20);
	ImGui::ColorEdit4("Color", &light.Color[0]);
}

void DragDirLights(Span<DirLight> lights) {
	IM_ID_SCOPE("DirLights");

	for (int i = 0; i<lights.Size(); i++) {
		ImGui::PushID(i);
		ImGui::DragDirLight(lights[i]);
		ImGui::Separator();
		ImGui::PopID();
	}
}

void DragSpotlight(Spotlight& spotlight) { 
	ImGui::DragFloat3("Position", &spotlight.Position[0], 0.1, -20, 20);
	ImGui::DragFloat3("Direction", &spotlight.Direction[0], 0.1, -20, 20);
	ImGui::DragFloat("CutoffAngle", &spotlight.CutoffAngle, 1, 0, 100);
	ImGui::DragFloat("DimAngle", &spotlight.DimAngle, 1, 0, 45);
	ImGui::ColorEdit4("Color", &spotlight.Color[0]);
}

void DragSpotlights(Span<Spotlight> lights) {
	IM_ID_SCOPE("Spotlights");

	for (int i = 0; i<lights.Size(); i++) {
		ImGui::PushID(i);
		ImGui::DragSpotlight(lights[i]);
		ImGui::Separator();
		ImGui::PopID();
	}
}

void DragInstance(Instance& instance) {
	ImGui::Text("Instance: %s", instance.Mesh->Name().Data());
	ImGui::DragFloat3("Position", &instance.Transform.Position[0], 0.1, -20, 20);
	ImGui::DragFloat3("Rotation", &instance.Transform.Rotation[0], 1, -180, 180);
	ImGui::DragFloat3("Scale", &instance.Transform.Scale[0], 0.1, 0, 20);
}

void DragInstances(Span<Instance> instances) {
	IM_ID_SCOPE("Instances");

	for (int i = 0; i<instances.Size(); i++) {
		ImGui::PushID(i);
		ImGui::DragInstance(instances[i]);
		ImGui::Separator();
		ImGui::PopID();
	}
}


}//namespace ImGui::