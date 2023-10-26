#include "Overlay.h"

bool boxESP;
bool bRounded;
float rounding;
bool SkeletonESP;
bool lineESP;
bool pieceESP;
bool playerNameESP;
bool healthESP;
bool quishaESP;
bool bAimbot;
int aimKey = 0;
char keyName[128] = "Aim Key";
static const char* aimModes[]{ "CrossHair", "Distance" };
int  aimPriority = 0;
float smoothX = 1.f;
float smoothY = 1.f;
bool bKillChanger;
int killCount;
bool infiniteStamina;
bool distanceESP;
bool teamESP;
bool itemESP;
bool drawAllItems;

float itemDistance = 75.f;


bool BoneScrap;

 bool CookLock;

 bool BloodBucket;

 bool LargeHealth;

 bool SmallHealth;

 bool UnlockTool;

 bool BoneChime;

 bool Chicken;

 bool Fuse;

 bool Grandpa;

 bool Valve;


ImFont* Font;



void overlay::menuDrawing()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImFont* Font = ImGui::GetFont();
	ImGui::SetNextWindowSize(ImVec2(600, 400));


	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration);
	{


		ImGui::Checkbox("Box", &boxESP);

		if (boxESP) {

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);
			ImGui::Checkbox("Rounded", &bRounded);
			if (bRounded) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

				ImGui::SliderFloat("Box Rounding", &rounding, 0.f, 15.f);
			}


		}


		ImGui::Checkbox("Skeleton", &SkeletonESP);

		ImGui::Checkbox("Line", &lineESP);

		ImGui::Checkbox("Quisha", &quishaESP);

		ImGui::Checkbox("Distance" , &distanceESP);

		ImGui::Checkbox("Player Name", &playerNameESP);

		ImGui::Checkbox("Team ESP", &teamESP);

		ImGui::Checkbox("Health ESP", &healthESP);


		ImGui::Checkbox("Item ESP", &itemESP);

		if (itemESP) {





			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);
			ImGui::SliderFloat("Item Distance", &itemDistance, 0.f, 150.f);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);


			ImGui::Checkbox("Bone Scrap", &BoneScrap);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Cook Lock", &CookLock);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Blood Bucket", &BloodBucket);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Large Health", &LargeHealth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Small Health", &SmallHealth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Unlock Tool", &UnlockTool);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Bone Chime", &BoneChime);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Chicken", &Chicken);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Fuse", &Fuse);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Grandpa", &Grandpa);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);

			ImGui::Checkbox("Valve", &Valve);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.f);




		}

		ImGui::Checkbox("Draw All Actors", &drawAllItems);

		ImGui::SetCursorPos({ ImGui::GetWindowSize().x - 305.f, 5.f });
		

		ImGui::Image((void*)vtLogoImg, { 348, 61 });




	}
	ImGui::End();
}


void overlay::drawText(Vector3 World2Screen, float yOffset, const char* text)
{
	Font = ImGui::GetFont();

	ImVec2 textSize = ImGui::CalcTextSize(text);

	Font->Scale = 1.5f;

	ImGui::PushFont(Font);

	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, (World2Screen.y + yOffset - textSize.y) - 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, (World2Screen.y + yOffset - textSize.y) + 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ World2Screen.x - (textSize.x / 2) * (Font->Scale) , World2Screen.y + yOffset - textSize.y }, ImColor{ 1.f, 1.f, 1.f, 1.f }, text);

	Font->Scale = 1.f;

	ImGui::PopFont();



}

void overlay::drawText(Vector3 World2Screen, float yOffset, const char* text, ImColor color)
{
	Font = ImGui::GetFont();

	ImVec2 textSize = ImGui::CalcTextSize(text);

	Font->Scale = 1.5f;

	ImGui::PushFont(Font);

	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, (World2Screen.y + yOffset - textSize.y) - 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, (World2Screen.y + yOffset - textSize.y) + 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ World2Screen.x - (textSize.x / 2) * (Font->Scale) , World2Screen.y + yOffset - textSize.y }, color, text);

	Font->Scale = 1.f;

	ImGui::PopFont();



}

void overlay::drawTextSmall(Vector3 World2Screen, float yOffset, const char* text)
{
	Font = ImGui::GetFont();

	ImVec2 textSize = ImGui::CalcTextSize(text);

	Font->Scale = 1.f;

	ImGui::PushFont(Font);

	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, World2Screen.y + yOffset - textSize.y }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) - 1.f, (World2Screen.y + yOffset - textSize.y) - 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ (World2Screen.x - (textSize.x / 2) * (Font->Scale)) + 1.f, (World2Screen.y + yOffset - textSize.y) + 1.f }, ImColor{ 0.f, 0.f, 0.f, 1.f }, text);
	ImGui::GetBackgroundDrawList()->AddText({ World2Screen.x - (textSize.x / 2) * (Font->Scale) , World2Screen.y + yOffset - textSize.y }, ImColor{ 1.f, 1.f, 1.f, 1.f }, text);

	Font->Scale = 1.f;

	ImGui::PopFont();



}

void overlay::drawBox(Vector3 World2Screen) 
{


	
	if (World2Screen.x <= 4000.f && World2Screen.x >= -2000.f) {

		if (World2Screen.y <= 4000.f && World2Screen.y>= -2000.f) {

			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x - 1.75f , World2Screen.y - boxDimensions.y - 1.75f }, { World2Screen.x + boxDimensions.x + 1.75f, World2Screen.y + boxDimensions.y + 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), 0.f, 0, 1.5f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x , World2Screen.y-boxDimensions.y  }, { World2Screen.x + boxDimensions.x , World2Screen.y + boxDimensions.y}, ImColor(1.f, 0.f, 0.f), 0.f, 0, 2.f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x + 1.75f , World2Screen.y - boxDimensions.y + 1.75f }, { World2Screen.x +  boxDimensions.x - 1.75f, World2Screen.y + boxDimensions.y - 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), 0.f, 0, 1.5f);

		}


	}

}
void overlay::drawBox(Vector3 World2Screen, float rounding, ImColor color)
{



	if (World2Screen.x <= 4000.f && World2Screen.x >= -2000.f) {

		if (World2Screen.y <= 4000.f && World2Screen.y >= -2000.f) {

			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x - 1.75f , World2Screen.y - boxDimensions.y - 1.75f }, { World2Screen.x + boxDimensions.x + 1.75f, World2Screen.y + boxDimensions.y + 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), rounding, 0, 1.5f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x , World2Screen.y - boxDimensions.y }, { World2Screen.x + boxDimensions.x , World2Screen.y + boxDimensions.y }, color, rounding, 0, 2.f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x + 1.75f , World2Screen.y - boxDimensions.y + 1.75f }, { World2Screen.x + boxDimensions.x - 1.75f, World2Screen.y + boxDimensions.y - 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), rounding, 0, 1.5f);

		}


	}

}
void overlay::drawBox(Vector3 World2Screen, ImColor color)
{



	if (World2Screen.x <= 4000.f && World2Screen.x >= -2000.f) {

		if (World2Screen.y <= 4000.f && World2Screen.y >= -2000.f) {

			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x - 1.75f , World2Screen.y - boxDimensions.y - 1.75f }, { World2Screen.x + boxDimensions.x + 1.75f, World2Screen.y + boxDimensions.y + 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), rounding, 0, 1.5f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x , World2Screen.y - boxDimensions.y }, { World2Screen.x + boxDimensions.x , World2Screen.y + boxDimensions.y }, color, rounding, 0, 2.f);
			ImGui::GetBackgroundDrawList()->AddRect({ World2Screen.x - boxDimensions.x + 1.75f , World2Screen.y - boxDimensions.y + 1.75f }, { World2Screen.x + boxDimensions.x - 1.75f, World2Screen.y + boxDimensions.y - 1.75f }, ImColor(0.f, 0.f, 0.f, 1.f), rounding, 0, 1.5f);

		}


	}

}



void overlay::drawLine(Vector3 World2Screen) 
{
	ImGui::GetBackgroundDrawList()->AddLine({ winProperties.width / 2 + winProperties.x, winProperties.height / 2  + winProperties.y}, { World2Screen.x, World2Screen.y }, ImColor{ 1.f, 0.f, 0.f }, 2.f);
}

void overlay::drawLine(Vector3 World2Screen, Vector3 Origin)
{
	ImGui::GetBackgroundDrawList()->AddLine({ Origin.x,Origin.y }, { World2Screen.x, World2Screen.y }, ImColor{ 1.f, 0.f, 0.f }, 2.f);
}


void overlay::drawHealth(Vector3 World2Screen, float health)
{
	if (health > 0.f && health <= 104.3f) {

		ImColor color;

		if (health >= 97.f) {

			color = ImColor(0.f, 1.f, 0.f, 1.f);

		}
		else if (health >= 75.f) {

			color = ImColor(1.f, 1.f, 0.f, 1.f);

		}
		else if (health >= 50.f) {

			color = ImColor(1.f, 0.5f, 0.f, 1.f);

		}
		else if (health >= 25.f) {

			color = ImColor(1.f, 0.f, 0.f, 1.f);

		}
		else if (health > 0.f) {

			color = ImColor(.5f, 0.2f, 0.f, 1.f);

		}

		ImGui::GetBackgroundDrawList()->AddLine({ World2Screen.x - boxDimensions.x - 5.f, (World2Screen.y + boxDimensions.y) - (2 * boxDimensions.y * (health / 100.f))}, { World2Screen.x - boxDimensions.x - 5.f, World2Screen.y + boxDimensions.y  }, ImColor(0.f, 0.f, 0.f, 1.f), 5.f);

		ImGui::GetBackgroundDrawList()->AddLine({ World2Screen.x - boxDimensions.x - 5.f, (World2Screen.y + boxDimensions.y) - (2 * boxDimensions.y * (health / 100.f)) +1.f }, { World2Screen.x - boxDimensions.x - 5.f, World2Screen.y + boxDimensions.y -1.f }, color, 3.f);
	}
}





