#include "pch.h"
#include "CPhysxActorUI.h"

#include "CGameObject.h"
#include "CPhysxActor.h"

CPhysxActorUI::CPhysxActorUI()
	: CComponentUI(COMPONENT_TYPE::PHYSXACTOR)
{
}

CPhysxActorUI::~CPhysxActorUI()
{
}

void CPhysxActorUI::Update_Com()
{
}

void CPhysxActorUI::Render_Com()
{
	float tab = 130;
	
	PxRigidActor* pBody = m_TargetObj->PhysxActor()->GetRigidBody();

	// Rigid Type
	ImGui::Text("Rigid Type");
	ImGui::SameLine(tab);
	int TypeIdx = (int)m_TargetObj->PhysxActor()->GetRigidType();
	const char* const Type[] = { "KINEMATIC", "DYNAMIC", "STATIC", "NONE"};
	if (ImGui::Combo("##RigidType", &TypeIdx, Type, 4))
	{
		m_TargetObj->PhysxActor()->SetRigidType((RIGID_TYPE)TypeIdx);
		pBody = m_TargetObj->PhysxActor()->GetRigidBody();
	}

	// NONE 이 아닌 경우 강체 정보 표시
	if ((RIGID_TYPE)TypeIdx != RIGID_TYPE::NONE)
	{
		// Density
		ImGui::Text("Density");
		ImGui::SameLine(tab);
		float Dens = m_TargetObj->PhysxActor()->GetDensity();
		if (ImGui::DragFloat("##Density", &Dens, 1.f, 0.1f))
		{
			m_TargetObj->PhysxActor()->SetDensity(Dens);
		}

		// Static 이 아닌 경우 물리 시뮬레이션 속성 표시
		if ((RIGID_TYPE)TypeIdx != RIGID_TYPE::STATIC)
		{
			PxRigidDynamic* pDynamicBody = (PxRigidDynamic*)pBody;
			// LockFlag
			UINT Flags = pDynamicBody->getRigidDynamicLockFlags();
			bool LockState[6] = { Flags & LINEAR_X, Flags & LINEAR_Y, Flags & LINEAR_Z, Flags & ANGULAR_X, Flags & ANGULAR_Y, Flags & ANGULAR_Z };
			ImGui::Text("Lock Flags");
			if (ImGui::BeginTable("##LockFlags", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
			{
				for (int i = 0; i < 6; i++)
				{
					ImGui::TableNextColumn();
					if (ImGui::Selectable(to_str(LOCK_FLAG_WSTR[i]).c_str(), &LockState[i]))
					{
						m_TargetObj->PhysxActor()->CkeckLockFlag((LOCK_FLAG)(1 << i));
					}
				}
				ImGui::EndTable();
			}

			// Gravity
			ImGui::Text("Gravity");
			bool bGravity = m_TargetObj->PhysxActor()->GetGravity();
			if (ImGui::Checkbox("##Gravity", &bGravity))
			{
				m_TargetObj->PhysxActor()->SetGravity(bGravity);
			}

			// Linear Velocity
			ImGui::Text("Linear Velocity");
			float LinVel[3] = { pDynamicBody->getLinearVelocity().x, pDynamicBody->getLinearVelocity().y, pDynamicBody->getLinearVelocity().z };
			if (ImGui::DragFloat3("##LinearVelocity", LinVel))
			{
				pDynamicBody->setLinearVelocity(PxVec3(LinVel[0], LinVel[1], LinVel[2]));
			}

			// Angular Velocity
			ImGui::Text("Angular Velocity");
			float AngVel[3] = { pDynamicBody->getAngularVelocity().x, pDynamicBody->getAngularVelocity().y, pDynamicBody->getAngularVelocity().z };
			if (ImGui::DragFloat3("##AngularVelocity", AngVel))
			{
				pDynamicBody->setAngularVelocity(PxVec3(AngVel[0], AngVel[1], AngVel[2]));
			}
		}

		// 충돌체 정보 표시
		if (ImGui::BeginTable("##ColliderList", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableSetupColumn("Collider Offset");
			ImGui::TableSetupColumn("Edit Desc");
			ImGui::TableHeadersRow();

			for (int i = 0; i < m_TargetObj->PhysxActor()->GetColliderCount(); ++i)
			{
				ImGui::TableNextColumn();
				string str = "##collideroffset_" + to_string(i);
				PxVec3 vOffset = m_TargetObj->PhysxActor()->GetColliderOffset(i);
				float offset[3] = { vOffset.x, vOffset.y, vOffset.z };
				if (ImGui::DragFloat3(str.c_str(), offset))
				{
					m_TargetObj->PhysxActor()->SetColliderOffset(i, PxVec3(offset[0], offset[1], offset[2]));
				}

				ImGui::TableNextColumn();
				str = "Edit_" + to_string(i);
				string strEdit = "EditDesc_" + to_string(i);
				if (ImGui::Button(str.c_str()))
				{
					ImGui::OpenPopup(strEdit.c_str());
				}
				if (ImGui::BeginPopupModal(strEdit.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					EditColliderDescPopup(i);
					ImGui::EndPopup();
				}

				ImGui::TableNextColumn();
				str = "Delete_" + to_string(i);
				if (ImGui::Button(str.c_str()))
				{
					m_TargetObj->PhysxActor()->DeleteCollider(i);
				}
			}
			ImGui::EndTable();
		}


		// 충돌체 추가 버튼
		if (ImGui::Button("Add Collider"))
		{
			ImGui::OpenPopup("AddCol");
		}
		if (ImGui::BeginPopupModal("AddCol", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			float tab = 130.f;
			ImGui::Text("Add collider to PhysX actor.");
			ImGui::NewLine();

			static COLLIDER_DESC Desc;

			// 충돌체 디스크립션
			ImGui::Text("Shape Flag");
			UINT ShapeFlag = Desc.ShapeFlag;
			if (ImGui::CheckboxFlags("eSIMULATION_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eSIMULATION_SHAPE))
			{
				if (ShapeFlag & PxShapeFlag::Enum::eTRIGGER_SHAPE)
					ShapeFlag = ShapeFlag ^ PxShapeFlag::Enum::eTRIGGER_SHAPE;
				Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
			}
			if (ImGui::CheckboxFlags("eSCENE_QUERY_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eSCENE_QUERY_SHAPE))
			{
				Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
			}
			if (ImGui::CheckboxFlags("eTRIGGER_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eTRIGGER_SHAPE))
			{
				if (ShapeFlag & PxShapeFlag::Enum::eSIMULATION_SHAPE)
					ShapeFlag = ShapeFlag ^ PxShapeFlag::Enum::eSIMULATION_SHAPE;
				Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
			}

			ImGui::Text("FilterLayer_Self");
			for (int i = 0; i < COLLISION_LAYER::END; ++i)
			{
				string str = to_str(COLLISION_LAYER_WSTR[i]) + "_Self";
				ImGui::CheckboxFlags(str.c_str(), &Desc.FilterLayer_Self, (1 << i));
			}

			ImGui::Text("FilterLayer_Other");
			for (int i = 0; i < COLLISION_LAYER::END; ++i)
			{
				string str = to_str(COLLISION_LAYER_WSTR[i]) + "_Other";
				ImGui::CheckboxFlags(str.c_str(), &Desc.FilterLayer_Other, (1 << i));
			}

			ImGui::Text("StaticFriction");
			float StaticFriction = Desc.StaticFriction;
			if (ImGui::InputFloat("##StaticFriction", &StaticFriction))
			{
				Desc.StaticFriction = StaticFriction;
			}

			ImGui::Text("DynamicFriction");
			float DynamicFriction = Desc.DynamicFriction;
			if (ImGui::InputFloat("##DynamicFriction", &DynamicFriction))
			{
				Desc.DynamicFriction = DynamicFriction;
			}

			ImGui::Text("Restitution");
			float Restitution = Desc.Restitution;
			if (ImGui::InputFloat("##Restitution", &Restitution))
			{
				Desc.Restitution = Restitution;
			}


			// 충돌체 크기
			ImGui::Text("Scale");
			static float Scale[3] = {};
			Scale[0] = max(1, Scale[0]);
			Scale[1] = max(1, Scale[1]);
			Scale[2] = max(1, Scale[2]);
			ImGui::InputFloat3("##Scale", Scale);

			// 충돌체 오프셋
			ImGui::Text("Offset");
			static float Offset[3] = {};
			ImGui::InputFloat3("##Offset", Offset);


			if (ImGui::Button("Add"))
			{

				m_TargetObj->PhysxActor()->AddCollider(Desc, PxVec3(Scale[0], Scale[1], Scale[2]), PxVec3(Offset[0], Offset[1], Offset[2]));
				COLLIDER_DESC DescClear;
				Desc = DescClear;
				memset(Scale, 0, sizeof(Scale)); memset(Offset, 0, sizeof(Offset));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				COLLIDER_DESC DescClear;
				Desc = DescClear;
				memset(Scale, 0, sizeof(Scale)); memset(Offset, 0, sizeof(Offset));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}





	}
}

void CPhysxActorUI::EditColliderDescPopup(int _Idx)
{
	static bool Init = false;
	static COLLIDER_DESC Desc;
	if (!Init)
	{
		Init = true;
		Desc = m_TargetObj->PhysxActor()->GetColliderDesc(_Idx);
	}
	

	float tab = 130.f;
	ImGui::Text("Edit collider description.");
	ImGui::NewLine();

	// 충돌체 디스크립션
	ImGui::Text("Shape Flag");
	UINT ShapeFlag = Desc.ShapeFlag;
	if (ImGui::CheckboxFlags("eSIMULATION_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eSIMULATION_SHAPE))
	{
		if (ShapeFlag & PxShapeFlag::Enum::eTRIGGER_SHAPE)
			ShapeFlag = ShapeFlag ^ PxShapeFlag::Enum::eTRIGGER_SHAPE;
		Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
	}
	if (ImGui::CheckboxFlags("eSCENE_QUERY_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eSCENE_QUERY_SHAPE))
	{
		Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
	}
	if (ImGui::CheckboxFlags("eTRIGGER_SHAPE", &ShapeFlag, PxShapeFlag::Enum::eTRIGGER_SHAPE))
	{
		if (ShapeFlag & PxShapeFlag::Enum::eSIMULATION_SHAPE)
			ShapeFlag = ShapeFlag ^ PxShapeFlag::Enum::eSIMULATION_SHAPE;
		Desc.ShapeFlag = (PxShapeFlags)ShapeFlag;
	}

	ImGui::Text("FilterLayer_Self");
	for (int i = 0; i < COLLISION_LAYER::END; ++i)
	{
		string str = to_str(COLLISION_LAYER_WSTR[i]) + "_Self";
		ImGui::CheckboxFlags(str.c_str(), &Desc.FilterLayer_Self, (1 << i));
	}

	ImGui::Text("FilterLayer_Other");
	for (int i = 0; i < COLLISION_LAYER::END; ++i)
	{
		string str = to_str(COLLISION_LAYER_WSTR[i]) + "_Other";
		ImGui::CheckboxFlags(str.c_str(), &Desc.FilterLayer_Other, (1 << i));
	}

	ImGui::Text("StaticFriction");
	float StaticFriction = Desc.StaticFriction;
	if (ImGui::InputFloat("##StaticFriction", &StaticFriction))
	{
		Desc.StaticFriction = StaticFriction;
	}

	ImGui::Text("DynamicFriction");
	float DynamicFriction = Desc.DynamicFriction;
	if (ImGui::InputFloat("##DynamicFriction", &DynamicFriction))
	{
		Desc.DynamicFriction = DynamicFriction;
	}

	ImGui::Text("Restitution");
	float Restitution = Desc.Restitution;
	if (ImGui::InputFloat("##Restitution", &Restitution))
	{
		Desc.Restitution = Restitution;
	}


	if (ImGui::Button("Apply"))
	{
		m_TargetObj->PhysxActor()->SetColliderDesc(_Idx, Desc);
		Init = false;
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		Init = false;
		ImGui::CloseCurrentPopup();
	}
}
