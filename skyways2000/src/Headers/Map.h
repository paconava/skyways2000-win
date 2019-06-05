/*
 * Map.h
 *
 *  Created on: 22/03/2019
 *      Author: Paco
 */

#include <string>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "definition.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Map {
public:
    float coinRot;
    
    struct ObjStruct {
        AABB m_aabb;
        int m_type;
    };

    struct CoinStruct {
        glm::vec3 coinPos;
        bool notTaken;
        SBB coinBB;
    };

    std::vector<CoinStruct> coinVector;
    
    Map(const std::string& FileName, const std::string& CoinFile, const std::string& EndFile);

    bool load(float factor, int level);
    void drawInteractive(GLint modelLoc, float factor, Shader * sd);
    bool draw(GLint modelLoc, float factor, Shader * sd);
    virtual ~Map();

    std::vector<ObjStruct> getBBS() {
        return bbs;
    }

    std::vector<ObjStruct> getWallbbs() {
        return wallbbs;
    }

    std::vector<ObjStruct> getObstaclebbs() {
        return obstaclebbs;
    }

    std::vector<SBB> getFallsbbs() {
        return falls;
    }
    
    AABB getLevelEnding() {
        return levelEnding;
    }
    
    std::vector<CoinStruct> getCoinsbbs () {
        return coinVector;
    }
    
    void freeMem() {
        bbs.clear();
        wallbbs.clear();
        obstaclebbs.clear();
        falls.clear();
        mapData.clear();
        coinData.clear();
    }
    
private:
    std::string m_fileName;
    std::string c_fileName;
    std::string e_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    std::string pieceId, posX, posY, rot;
    std::vector<float> mapData;
    std::vector<float> coinData;
    std::vector<float> endData;
    bool isRefuel;
    int obstacleType;
    float obsMov;
    Texture * texture0;
    Texture * texture1;
    Texture * texture2;
    Texture * texture3;
    Texture * texture4;
    Texture * texture5;
    Texture * texture6;
    Texture * brickWall;
    Texture * obsTexture;
    Texture * coinTexture;
    GLint selector;
    GLint changerLoc;
    float changer;
    int numWalls;
    int numObstacles;
    int numRefuels;
    int numFallSpaces;
    int numCoins;
    int mapLevel;
    std::vector<ObjStruct> bbs;
    std::vector<ObjStruct> wallbbs;
    std::vector<ObjStruct> obstaclebbs;
    std::vector<SBB> falls;
    AABB levelEnding;
};
