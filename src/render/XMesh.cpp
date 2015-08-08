#include "XMesh.h"

#include "main/EventManager.h"

extern void gotoBrace(std::ifstream& inFile);

XMesh::XMesh(DX11SceneManager* pSceneManager)
{
    //SceneManager and shaderSystem for own uses
    m_pSceneManager = pSceneManager;
    m_pShaderSystem = pSceneManager->getShaderSystem();

    //Fix this as well, frame cannot be copied for some reason.
    m_frameContainer.reserve(10);
}

HRESULT XMesh::load(const char* strFilename, CoordinateSystem type)
{
    HRESULT hr = S_OK;

    m_type = type;

    std::ifstream inFile;
    //Try/catch blocks, meh
    try {
        inFile.open(strFilename);
        if (!inFile) {
            throw(GetLastError());
        }

        bool readBinary;

        std::string strLine;
        LoadLibrary
        getline(inFile, strLine);
        //Determine if the xfile is binary or text
        if (strLine.find("bin 0032") != strLine.npos)
            readBinary = true;
        else if (strLine.find("txt 0032") != strLine.npos)
            readBinary = false;
        else
            throw(E_CORRUPTEDFILE);

        while (getline(inFile, strLine)) {
            //Skip empty lines and don't read this way when binary
            if (!strLine.empty() && !readBinary) {
                if (strLine.find("template") != strLine.npos) {
                    gotoBrace(inFile);
                }
                else if (strLine.find("End of Root Frame") != strLine.npos) {
                    return S_OK;
                }
                else if (strLine.find("Frame") != strLine.npos) {
                    //Read every frame
                    readFrame(m_pSceneManager->getMainNode(), strLine, inFile);
                }
            }
        }

        inFile.close();
    }
    catch (HRESULT Error) {
        if (inFile) {
            inFile.close();
        }

        return EventManager::Instance().postException(Error, "Failed to load XMesh: %s", strFilename);
    }


    return S_OK;
}

void XMesh::setMatrixByName(char *Name, glm::mat4x4 Matrix)
{
    for (UINT i = 0; i < m_frameContainer.size(); i++)	{

    }
}

Frame* XMesh::getFrameByName(const char* strName)
{
    for (UINT i = 0; i < m_frameContainer.size(); i++) {
        if (::strcmp(strName, m_frameContainer[i].getName()) == 0) {
            return &m_frameContainer[i];
        }
    }

    return NULL;
}

HRESULT XMesh::readFrame(SceneNode* pParent, std::string currentLine, std::ifstream& inFile)
{
    m_frameContainer.push_back(Frame(m_pSceneManager, m_pShaderSystem));

    Frame* pCurrentFrame = &m_frameContainer.back();

    //Find positions
    size_t pos;
    //Position of the stream
    std::streampos streamPos;

    //Read the Frame text
    std::string strLine(currentLine);
    pos = strLine.find("Frame");
    strLine.erase(0, pos);

    //Find the name
    pos = strLine.find(" ");
    strLine.erase(0, pos + 1);

    pos = strLine.find(" ");
    strLine.erase(pos, strLine.length() - pos);

    std::string strName = std::string(strLine.c_str());

    //Create the SceneNode
    SceneNode* pSceneNode = m_pSceneManager->createSceneNode(strName, pParent);

    //Mesh properties
    glm::mat4x4 matrix;
    int NumOfVertices = 0;
    int NumOfIndices = 0;
    Vertex* pVertexArray = NULL;
    UINT*   pIndexArray = NULL;

    //Read mesh
    while (strLine.find("}") == strLine.npos) {
        //Read FrameTransformMatrix, the world matrix
        if (strLine.find("FrameTransformMatrix") != strLine.npos) {
            getline(inFile, strLine);

            float Variable;
            //Read the rows of the matrix, this is always right handed.
            for (int row = 0; row <= 3; row++) {
                //Read the colums of the matrix
                for (int column = 0; column <= 3; column++) {
                    sscanf_s(strLine.c_str(), "%f", &Variable);

                    matrix[row][column] = Variable;

                    strLine.erase(0, strLine.find(",") + 1);
                }
                //If the line is emptry try the next one, for multiple layouts.
                if (strLine.length() == 0) {
                    getline(inFile, strLine);
                }

            }
            gotoBrace(inFile);
        }
        //Multiple material lists (not really supported)
        else if (strLine.find("MeshMaterialList") != strLine.npos)
        {
            getline(inFile, strLine);

            int NumOfMaterials = 0;
            int NumOfLines = 0;

            sscanf_s(strLine.c_str(), "%i", &NumOfMaterials);

            getline(inFile, strLine);

            sscanf_s(strLine.c_str(), "%i", &NumOfLines);

            for (int i = 0; i < NumOfLines; i++) {
                getline(inFile, strLine);
            }
            //Read material file
            while (strLine.find("}") == strLine.npos) {
                if (strLine.find("Material") != strLine.npos) {
                    //pMaterial = new Material();
                    //pFrame->pMaterial->pTexture = new Texture();

                    //if(FAILED(readMaterialText(strLine, inFile, pMaterial))) return E_FAIL;		
                }
                getline(inFile, strLine);
            }
            gotoBrace(inFile);
        }
        //Texture UV coordinates
        else if (strLine.find("MeshTextureCoords") != strLine.npos) {
            getline(inFile, strLine);

            int NumOfLines = 0;

            sscanf_s(strLine.c_str(), "%i", &NumOfLines);

            for (int i = 0; i < NumOfVertices; i++) {
                getline(inFile, strLine);

                sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].TexCoord.x);

                strLine.erase(0, strLine.find(";") + 1);
                sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].TexCoord.y);
            }
            gotoBrace(inFile);
        }
        //Normals
        else if (strLine.find("MeshNormals") != strLine.npos) {
            getline(inFile, strLine);

            int NumOfLines = 0;
            int index = 0;

            sscanf_s(strLine.c_str(), "%i", &NumOfLines);

            glm::vec3* pNormalArray = new glm::vec3[NumOfLines];
            //Read all the normals and store them.
            for (int i = 0; i < NumOfLines; i++)
            {
                getline(inFile, strLine);
                if (m_type == LEFTHANDED)
                {
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].x);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].z);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].y);
                }
                else if (m_type == RIGHTHANDED)
                {
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].x);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].y);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pNormalArray[i].z);
                }
            }

            getline(inFile, strLine);

            sscanf_s(strLine.c_str(), "%i", &NumOfLines);

            //Read how many vertices there are in the face.
            UINT numOfNormalIndices = NumOfLines * 3;
            //The index of the vertex
            UINT normalIndex = 0;
            UINT vertexIndex = 0;
            //Add the normals to the existing vertices.
            for (int i = 0; i < NumOfLines; i++) {
                getline(inFile, strLine);

                sscanf_s(strLine.c_str(), "%i", &numOfNormalIndices);
                strLine.erase(0, strLine.find(";") + 1);

                for (UINT j = 0; j < numOfNormalIndices; j++) {
                    sscanf_s(strLine.c_str(), "%i", &normalIndex);
                    pos = strLine.find(",");
                    if (pos == strLine.length() - 1) {
                        strLine.erase(0, strLine.find(";") + 1);
                    }
                    else {
                        strLine.erase(0, pos + 1);
                    }

                    pVertexArray[pIndexArray[vertexIndex]].Normal = pNormalArray[normalIndex];

                    vertexIndex++;
                }
            }

            SAFE_DELETE_ARRAY(pNormalArray);

            gotoBrace(inFile);
        }
        else if (strLine.find("Mesh") != strLine.npos) {
            getline(inFile, strLine);

            int NumOfLines = 0;
            //Read the number of lines/vertices
            sscanf_s(strLine.c_str(), "%i", &NumOfLines);
            NumOfVertices = NumOfLines;
            //Create an array the size of the vectices
            pVertexArray = new Vertex[NumOfVertices];

            int i = 0;
            int index = 0;

            //Read the vertices and store the positions
            for (i = 0; i < NumOfLines; i++) {
                getline(inFile, strLine);

                if (m_type == LEFTHANDED) {
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.x);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.z);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.y);
                }
                else if (m_type == RIGHTHANDED) {
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.x);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.y);

                    strLine.erase(0, strLine.find(";") + 1);
                    sscanf_s(strLine.c_str(), "%f", &pVertexArray[i].Pos.z);
                }
            }

            getline(inFile, strLine);

            sscanf_s(strLine.c_str(), "%i", &NumOfLines);

            UINT verticesPerFace = 0;
            UINT vertexIndex = 0;
            UINT indiceIndex = 0;
            UINT indexCount = 0;

            std::streampos preCountPos;
            preCountPos = inFile.tellg();

            bool semicolon = false;

            //count the number of indices
            for (i = 0; i < NumOfLines; i++) {
                getline(inFile, strLine);

                sscanf_s(strLine.c_str(), "%i", &verticesPerFace);
                strLine.erase(0, strLine.find(";") + 1);
                //Sum of the number of vertices in each line
                indexCount += verticesPerFace;
                if (verticesPerFace == 4) {
                    indexCount += 2;
                }

                //Use this to determine the layout semicolon seperate or comma
                if (i == 0) {
                    pos = strLine.find(",");
                    if (pos == strLine.length() - 1) {
                        semicolon = true;
                    }
                }
            }

            //Create an index array
            pIndexArray = new UINT[indexCount];
            //Reset the inFile back to preCount.
            inFile.seekg(preCountPos);
            //Read the indices
            for (i = 0; i < NumOfLines; i++) {
                getline(inFile, strLine);
                //Read the number of vertices in this line
                sscanf_s(strLine.c_str(), "%i", &verticesPerFace);
                strLine.erase(0, strLine.find(";") + 1);

                //Read all the vertices and store them
                for (UINT j = 0; j < verticesPerFace; j++) {
                    sscanf_s(strLine.c_str(), "%i", &vertexIndex);
                    if (semicolon) {
                        strLine.erase(0, strLine.find(";") + 1);
                    }
                    else {
                        strLine.erase(0, strLine.find(",") + 1);
                    }

                    pIndexArray[indiceIndex] = vertexIndex;

                    indiceIndex++;
                }
                //Change from CCW to CW
                //Break up quad
                if (verticesPerFace == 4) {
                    //We read [0,1,2,3] in the previous step and indiceIndex is the next element
                    //Copy the last one [0,1,2,3,3]
                    pIndexArray[indiceIndex] = pIndexArray[indiceIndex - 1];
                    //Change the last one [0,1,2,2,3]
                    pIndexArray[indiceIndex - 1] = pIndexArray[indiceIndex - 2];
                    //Add one more and copy to first to the last [0,1,2,2,3,0]
                    indiceIndex++;
                    pIndexArray[indiceIndex] = pIndexArray[indiceIndex - 5];
                    indiceIndex++;
                }
            }

            NumOfIndices = indiceIndex;
        }
        else if (strLine.find("Frame") != strLine.npos) {
            //child frame
            readFrame(pSceneNode, strLine, inFile);
        }

        streamPos = inFile.tellg();
        getline(inFile, strLine);
    }

    getline(inFile, strLine);

    //sibling frame
    if (strLine.find("Frame") != strLine.npos && strLine.find("}") == strLine.npos)
    {
        readFrame(pParent, strLine, inFile);
    }

    pCurrentFrame->initialize(pSceneNode, matrix, strName, pVertexArray, NumOfVertices, pIndexArray, NumOfIndices);

    return S_OK;
}

HRESULT XMesh::readMaterialText(std::string CurrentLine, std::ifstream& inFile, Material& material)
{
    std::string strLine(CurrentLine);

    int pos;

    pos = strLine.find("Material");
    strLine.erase(0, pos);

    pos = strLine.find(" ");
    strLine.erase(0, pos + 1);

    if (strLine.find("{") != strLine.npos) {
        material.strName = "standard_material";
    }
    else {
        pos = strLine.find(" ");
        strLine.erase(pos, strLine.length() - pos);
        material.strName = strLine;
    }

    m_pSceneManager->loadTexture(material.diffuseTex, "default.dds");

    gotoBrace(inFile);

    return S_OK;
}

int Frame::initialize(SceneNode* pSceneNode, glm::mat4x4 world, std::string strName, Vertex* pVertexArray, UINT vertexCount, UINT* pIndexArray, UINT indexCount)
{
    m_strName = strName;
    m_pSceneNode = pSceneNode;
    m_pSceneNode->setMatrix(world);

    if (vertexCount > 0) {
        //There are vertices so a model is created
        m_pModel = m_pSceneManager->createModel();

        if (FAILED(m_pModel->createMesh(pVertexArray, vertexCount, pIndexArray, indexCount))) return E_FAIL;
        //m_pModel->setMatrix(world);

        //getline(inFile, strLine);
        //Initialize the default material if none is available
        //if(m_material.strName.length() == 0)
        //{
        m_pModel->setShader(m_pShaderSystem->initShader("diffuseShader.fx", VERTEX));
        m_pSceneManager->loadTexture(m_material.diffuseTex, "default.dds");

        setMaterial(m_material);
        //}

        //Non functional bounding boxes YEAH
        glm::vec3 boundingBox[4];
        boundingBox[0] = glm::vec3(0.0f, 0.0f, 0.0f);
        boundingBox[1] = glm::vec3(1.0f, 0.0f, 0.0f);
        boundingBox[2] = glm::vec3(0.0f, 0.0f, 1.0f);
        boundingBox[3] = glm::vec3(1.0f, 0.0f, 1.0f);

        m_pModel->setBoundingBox(boundingBox);

        m_pSceneNode->attachModel(m_pModel->getID());
        m_pSceneNode->setCastShadow(true);
    }

    return S_OK;
}

void gotoBrace(std::ifstream& inFile)
{
    std::string strLine;

    while (strLine.find("}") == strLine.npos)
    {
        getline(inFile, strLine);
    }
}