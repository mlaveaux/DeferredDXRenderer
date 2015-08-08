#ifndef SCENENODE_H
#define SCENENODE_H

#include "render/DX11/DX11SceneManager.h"

#include <windows.h>
#include <string.h>

class SceneNode
{
public:
	SceneNode(DX11SceneManager* pCreator, SceneNode* pParent, std::string strName) 
	{
		m_pCreator = pCreator;
		m_pParent  = pParent;
		m_strName  = strName; 

		m_castShadow     = false;
		m_castReflection = false;
		m_isVisible      = true;
		m_modelID    = EMPTY_MODEL;
		
		identity();
	}
	~SceneNode()
	{		
		//for(UINT i = 0;i < m_childArray.size();i++)
		//{
		//	SAFE_DELETE(m_childArray[i]);
		//}

		m_pParent->removeNode(this);
		m_pCreator->removeNode(this);
	}

	void update(glm::mat4x4 parentMatrix)
	{
		m_matrix = m_nodeMatrix * parentMatrix;

		for(UINT i = 0;i < m_childArray.size();i++)
		{
			m_childArray[i]->update(m_matrix);
		}
	}

	void identity()
	{
        m_nodeMatrix = glm::mat4x4(1.0f);
	}

	void rotate(float x, float y, float z, float angle)
	{
		glm::mat4x4 rotationMatrix(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(x, y, z));

		m_nodeMatrix *= rotationMatrix;
	}

	void translate(float x, float y, float z)
	{
		glm::mat4x4 translateMatrix(1.0f);
        translateMatrix = glm::translate(translateMatrix, glm::vec3(x, y, z));

		m_nodeMatrix *= translateMatrix;
	}

	void scale(float x, float y, float z)
	{
		glm::mat4x4 scaleMatrix(1.0f);		
        scaleMatrix = glm::scale(scaleMatrix, glm::vec3(x, y, z));

		m_nodeMatrix *= scaleMatrix;
	}

	void setMatrix(glm::mat4x4 matrix)
	{
		m_nodeMatrix = matrix;
	}

	glm::mat4x4 getMatrix()
	{
		return m_matrix;
	}

	void addNode(SceneNode* pChild)
	{
		m_childArray.push_back(pChild);
	}

	void removeNode(SceneNode* pChild)
	{
		for(UINT i = 0;i <m_childArray.size();i++)
		{
			if((long)pChild == (long)m_childArray[i])
			{
				m_childArray.erase(m_childArray.begin()+i);
			}
		}
	}

	void attachModel(UINT modelID)
	{
		m_modelID = modelID;
	}

	bool isVisible()
	{
		return m_isVisible;
	}

	void setVisible(bool bValue){ m_isVisible = bValue; }
	void setCastShadow(bool bValue){ m_castShadow = bValue; }
	void setCastReflection(bool bValue) { m_castReflection = bValue; }

	bool castShadow(){ return m_castShadow; }
	bool castReflection() { return m_castReflection; }
	UINT getModelID(){ return m_modelID;    }
private:
	DX11SceneManager* m_pCreator;
	SceneNode*        m_pParent;
	
	std::vector<SceneNode*> m_childArray;

	std::string m_strName;
	glm::mat4x4  m_nodeMatrix;
	glm::mat4x4  m_matrix;
	UINT        m_modelID;

	bool m_isVisible;
	bool m_castShadow;
	bool m_castReflection;
};

#endif