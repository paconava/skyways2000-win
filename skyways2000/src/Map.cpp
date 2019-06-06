/*
 * Map.cpp
 *
 *  Created on: 22/03/2019
 *      Author: Paco
 */

#include "Headers/Map.h"

#include <iostream>
#include <fstream>

Map::Map(const std::string& FileName, const std::string& CoinFile, const std::string& EndFile) {
    this->m_fileName = FileName;
    this->c_fileName = CoinFile;
    this->e_fileName = EndFile;
}

Map::~Map() {

}

bool Map::load(float factor, int level) {
    const char* filename = m_fileName.c_str();
    const char* coinfile = c_fileName.c_str();
    const char* endline = e_fileName.c_str();
    std::string map_data;
    std::string coin_data;
    std::string end_line;
    coinRot = 0.0f;
    changer = 0.0f;
    numWalls = 0;
    numRefuels = 0;
    numObstacles = 0;
    numFallSpaces = 0;
    numCoins = 0;
    obsMov = 0;
    // Lectura del mapa
    std::fstream map(filename);
    if (map.is_open()) {
        while (std::getline(map, map_data, '#')) {
            mapData.push_back(std::stof(map_data));
        }
    } else {
        std::cout << "No se encontro el archivo: " << m_fileName
                << " - Abortando." << std::endl;
        exit(-1);
    }

    // Lectura de monedas
    std::fstream cn(coinfile);
    if (cn.is_open()) {
        while (std::getline(cn, coin_data, '#')) {
            coinData.push_back(std::stof(coin_data));
        }
    } else {
        std::cout << "No se encontro el archivo: " << c_fileName
                << " - Abortando." << std::endl;
        exit(-1);
    }
    
    // Lectura de meta
    std::fstream endL(endline);
    if (endL.is_open()) {
        while (std::getline(endL, end_line, '#')) {
            endData.push_back(std::stof(end_line));
        }
    } else {
        std::cout << "No se encontro el archivo: " << e_fileName
                << " - Abortando." << std::endl;
        exit(-1);
    }
    
    
    // Load textures
    texture0 = new Texture(GL_TEXTURE_2D, "../Textures/road0p.png");
    texture0->load();
    texture1 = new Texture(GL_TEXTURE_2D, "../Textures/road1p.png");
    texture1->load();
    texture2 = new Texture(GL_TEXTURE_2D, "../Textures/road2p.png");
    texture2->load();
    texture3 = new Texture(GL_TEXTURE_2D, "../Textures/road31p.png");
    texture3->load();
    texture4 = new Texture(GL_TEXTURE_2D, "../Textures/road41p.png");
    texture4->load();
    texture5 = new Texture(GL_TEXTURE_2D, "../Textures/transparent.png");
    texture5->load();
    texture6 = new Texture(GL_TEXTURE_2D, "../Textures/fuel.jpg");
    texture6->load();
    
    mapLevel = level;
    switch(level){
        case 0:
            brickWall = new Texture(GL_TEXTURE_2D, "../Textures/squareb.jpg");
            brickWall->load();
            obsTexture = new Texture(GL_TEXTURE_2D, "../Textures/texturaLadrillos2.jpg");
            obsTexture->load();
            break;
        case 1:
            brickWall = new Texture(GL_TEXTURE_2D, "../Textures/rusty.jpg");
            brickWall->load();
            obsTexture = new Texture(GL_TEXTURE_2D, "../Textures/texturaLadrillos2.jpg");
            obsTexture->load();
            break;
        case 2:
            brickWall = new Texture(GL_TEXTURE_2D, "../Textures/oxidado.jpg");
            brickWall->load();
            obsTexture = new Texture(GL_TEXTURE_2D, "../Textures/texturaLadrillos2.jpg");
            obsTexture->load();
            break;
        case 3:
            brickWall = new Texture(GL_TEXTURE_2D, "../Textures/rusty.jpg");
            brickWall->load();
            obsTexture = new Texture(GL_TEXTURE_2D, "../Textures/texturaLadrillos2.jpg");
            obsTexture->load();
            break;
    }
    
    coinTexture = new Texture(GL_TEXTURE_2D, "../Textures/moneda.png");
    coinTexture->load();
    // Get coins
    numCoins = coinData.size() / 3;
    CoinStruct coins;
    
    for (unsigned int i = 0; i <= coinData.size() - 3; i += 3) {
        coins.coinPos =  glm::vec3(coinData[i], coinData[i + 1], coinData[i + 2]);
        coins.coinBB.center = glm::vec3(factor * coins.coinPos.x, 
                coins.coinPos.y, 
                factor * coins.coinPos.z);
        coins.coinBB.ratio = 1.0f;
        coins.notTaken = true;
        if (coinVector.size() < numCoins)
            coinVector.push_back(coins);
    }
    
    // Get number of walls
    for (unsigned int i = 0; i <= mapData.size() - 7; i += 7) {
        switch (static_cast<int> (mapData[i])) {
            case 0:
                numWalls += 2;
                break;
            case 1:
                numWalls += 3;
                numFallSpaces++;
                break;
            case 2:
                numWalls += 4;
                numFallSpaces++;
                break;
            case 3:
                numWalls += 2;
                break;
            case 4:
                numWalls += 2;
                break;
        }
        switch (static_cast<int> (mapData[i + 6])) {
            case 1:
                numObstacles += 4;
                break;
            case 2:
                numObstacles+=2;
                break;
            case 3:
                numObstacles += 4;
                break;
            default:
                break;
        }
        if (static_cast<int> (mapData[i + 5]))
            numRefuels++;
    }

    levelEnding.min = glm::vec3(endData[0] * factor - factor * 1.5f,
                0.0f,
                endData[2] * factor - factor * 1.5f);
    levelEnding.max = glm::vec3(endData[0] * factor + factor * 1.5f,
                20.0f,
                endData[2] * factor + factor * 1.5f);
    
    numWalls++;
    ObjStruct startWall;
    startWall.m_aabb.max = glm::vec3(0.5f * factor, 10.0f, -factor);    
    startWall.m_aabb.min = glm::vec3(-0.5f * factor, 0.0f, -2.0f * factor);
    wallbbs.push_back(startWall);
    return true;
}

bool Map::draw(GLint modelLoc, float factor, Shader * sd) {
	GLint lightPosLoc;
	SBB fallSpace;
	selector = sd->getUniformLocation("selector");
	glUniform1i(selector, 0);
	glUniform1f(sd->getUniformLocation("light.cutOff"), glm::cos(glm::radians(60.5f)));
	glUniform1f(sd->getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(90.5f)));
	glUniform3f(sd->getUniformLocation("light.direction"), 0.0, -1.0, 0.0);
	glUniform3f(sd->getUniformLocation("light.ambient"), 0.8f, 0.8f, 0.8f);
	glUniform3f(sd->getUniformLocation("light.diffuse"), 1.0f, 0.2f, 0.4f);
	glUniform3f(sd->getUniformLocation("light.specular"), 0.1f, 0.1f, 0.1f);
	for (unsigned int i = 0; i <= mapData.size() - 7; i += 7) {
		glm::mat4 pista = glm::translate(glm::mat4(1.0f), glm::vec3(factor * mapData[i + 1], -0.5f + mapData[i + 2], factor * mapData[i + 3]));
		switch (static_cast<int> (mapData[i])) {
		case 0:
			// Bind Texture
			texture0->bind(GL_TEXTURE0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);

			pista = glm::rotate(pista, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
			pista = glm::scale(pista, glm::vec3(factor, 1.0f, factor * 2.0f));
			break;
		case 1:
			texture1->bind(GL_TEXTURE0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);
			pista = glm::rotate(pista, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
			pista = glm::scale(pista, glm::vec3(factor * 2.0f, 1.0f, factor * 2.0f));
			if (mapData[i + 4] == 0.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] + factor, -0.5f + mapData[i + 2], factor * mapData[i + 3] - factor);
			else if (mapData[i + 4] == 90.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] - factor, -0.5f + mapData[i + 2], factor * mapData[i + 3] - factor);
			else if (mapData[i + 4] == 180.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] - factor, -0.5f + mapData[i + 2], factor * mapData[i + 3] + factor);
			else if (mapData[i + 4] == 270.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] + factor, -0.5f + mapData[i + 2], factor * mapData[i + 3] + factor);
			fallSpace.ratio = factor;
			if (numFallSpaces > falls.size())
				falls.push_back(fallSpace);
			break;
		case 2:
			pista = glm::rotate(pista, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
			pista = glm::scale(pista, glm::vec3(factor * 3.0f, 1.0f, factor * 3.0f));
			if (mapData[i + 4] == 0.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] + (factor * 1.5f), -0.5f + mapData[i + 2], factor * mapData[i + 3] - (factor * 1.5f));
			else if (mapData[i + 4] == 90.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] - (factor * 1.5f), -0.5f + mapData[i + 2], factor * mapData[i + 3] - (factor * 1.5f));
			else if (mapData[i + 4] == 180.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] - (factor * 1.5f), -0.5f + mapData[i + 2], factor * mapData[i + 3] + (factor * 1.5f));
			else if (mapData[i + 4] == 270.0f)
				fallSpace.center = glm::vec3(factor * mapData[i + 1] + (factor * 1.5f), -0.5f + mapData[i + 2], factor * mapData[i + 3] + (factor * 1.5f));
			fallSpace.ratio = factor * 2.0f;
			if (numFallSpaces > falls.size())
				falls.push_back(fallSpace);
			texture2->bind(GL_TEXTURE0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);
			break;
		case 3:
			pista = glm::rotate(pista, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
			pista = glm::scale(pista, glm::vec3(factor * 2.0f, 1.0f, factor * 3.0f));
			texture3->bind(GL_TEXTURE0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);
			break;
		case 4:
			pista = glm::rotate(pista, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
			pista = glm::scale(pista, glm::vec3(factor * 2.0f, 1.0f, factor * 3.0f));
			texture4->bind(GL_TEXTURE0);
			glUniform1i(sd->getUniformLocation("texture0"), 0);
			break;
		default:
			break;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform3f(sd->getUniformLocation("light.position"), mapData[i + 1] * factor, 2.0f, mapData[i + 3] * factor);
		glUniform1i(selector, 0);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pista));
		glDrawArrays(GL_TRIANGLES, 30, 6);
		glDisable(GL_BLEND);
	}

	texture5->bind(GL_TEXTURE0);
	glUniform1i(sd->getUniformLocation("texture0"), 0);
	glm::mat4 grass = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
	grass = glm::scale(grass, glm::vec3(10000.f, 0.0f, 10000.f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grass));
	glDrawArrays(GL_TRIANGLES, 30, 6);
	glUniform1i(selector, 1);
	brickWall->bind(GL_TEXTURE0);

	glm::mat4 salida = glm::scale(glm::mat4(1.0f), glm::vec3(factor, 10.0f, factor));
	salida = glm::translate(salida, glm::vec3(-1.0f, 0.0f, -1.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(salida));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	salida = glm::translate(salida, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(salida));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	salida = glm::translate(salida, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(salida));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	coinTexture->bind(GL_TEXTURE0);
	glm::mat4 meta = glm::translate(glm::mat4(1.0f), glm::vec3(endData[0] * factor, 0.0f, endData[2] * factor));
	meta = glm::scale(meta, glm::vec3(factor * 3.0f, factor, factor * 3.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(meta));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform1i(selector, 0);
	return true;
}

void Map::drawInteractive(GLint modelLoc, float factor, Shader * sd) {
    selector = sd->getUniformLocation("selector");
    changerLoc = sd->getUniformLocation("changer");
    changer += 0.003;
    coinRot += 0.5f;
    obsMov += 5.0f;
    obstaclebbs.clear();
    if(obsMov == 360.0f)
        obsMov = 0.0f;
    float yMax = 1.0f;
    float yMin = -1.0f;
    float wallScaleY = 10.0f;
    glUniform1f(changerLoc, changer);
    ObjStruct temp1;
    ObjStruct obstacle[2];
    ObjStruct walls[4];
    // Bind Texture
    coinTexture->bind(GL_TEXTURE0);
    glUniform1i(sd->getUniformLocation("texture0"), 0);
    glUniform3f(sd->getUniformLocation("light.ambient"), 0.8f, 0.8f, 0.8f);
	
	for (unsigned int i = 0; i < numCoins; i++) {
		glm::mat4 coin = glm::mat4(1.0f);

		if (coinVector[i].notTaken) {
			coin = glm::translate(coin, coinVector[i].coinBB.center);
			glUniform3f(sd->getUniformLocation("light.position"),
				coinVector[i].coinBB.center.x,
				coinVector[i].coinBB.center.y + 3.0f,
				coinVector[i].coinBB.center.z);

			coin = glm::rotate(coin, glm::radians(coinRot), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(coin));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
    
    glUniform3f(sd->getUniformLocation("light.ambient"), 1.0f, 1.0f, 1.0f);
    for (unsigned int i = 0; i <= mapData.size() - 7; i += 7) {
        glm::mat4 fuel = glm::mat4(1.0f);
        glm::mat4 wall = glm::translate(glm::mat4(1.0f), glm::vec3(factor * mapData[i + 1], 0.0f, factor * mapData[i + 3]));
        glm::mat4 obs = glm::translate(glm::mat4(1.0f), glm::vec3(factor * mapData[i + 1], 0.0f, factor * mapData[i + 3]));
        glUniform3f(sd->getUniformLocation("light.position"), mapData[i + 1] * factor, 2.0f, mapData[i + 3] * factor);
        // Bind Texture
        brickWall->bind(GL_TEXTURE0);
        glUniform1i(sd->getUniformLocation("texture6"), 6);
        glUniform1i(selector, 0);
        glUniform3f(sd->getUniformLocation("light.position"), mapData[i + 1] * factor, 2.0f, mapData[i + 3] * factor);
        isRefuel = mapData[i + 5];
        obstacleType = mapData[i + 6];
        switch (static_cast<int> (mapData[i])) {
            case 0:
                // Wall 0
                wall = glm::rotate(wall, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                wall = glm::translate(wall, glm::vec3(factor, 0.0f, factor / 2.0f));
                wall = glm::scale(wall, glm::vec3(factor, wallScaleY, factor));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 1
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 2
                wall = glm::translate(wall, glm::vec3(-2.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 3
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                if (mapData[i + 4] == 0.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 0.5f),
                            yMin,
                            factor * mapData[i + 3] - factor);
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + factor);
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - factor);
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 0.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + factor);
                } else if (mapData[i + 4] == 90.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMin,
                            factor * mapData[i + 3] + (factor * 0.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 0.5f));
                }
                for (unsigned int i = 0; i < 2; i++)
                    if (numWalls > wallbbs.size())
                        wallbbs.push_back(walls[i]);
                if (isRefuel) {
                    glUniform1i(selector, 1);
                    fuel = glm::translate(glm::mat4(1.0f), glm::vec3(factor * mapData[i + 1], -0.5f + mapData[i + 2], factor * mapData[i + 3]));
                    fuel = glm::rotate(fuel, glm::radians(mapData[i + 4] + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    fuel = glm::scale(fuel, glm::vec3(factor, 1.0f, factor * 2.0f));
                    fuel = glm::translate(fuel, glm::vec3(0.375f, 0.1f, 0.0f));
                    fuel = glm::scale(fuel, glm::vec3(0.25f, 1.0f, 1.0f));
                    texture6->bind(GL_TEXTURE0);
                    glUniform1i(sd->getUniformLocation("texture1"), 0);
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fuel));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    brickWall->bind(GL_TEXTURE0);
                    if (mapData[i + 4] == 0.0f) {
                        temp1.m_aabb.max = glm::vec3((factor * mapData[i + 1] - (factor * 3.0f / 8.0f) + (factor / 8.0f)),
                                0.15f,
                                factor * mapData[i + 3] + factor);
                        temp1.m_aabb.min = glm::vec3((factor * mapData[i + 1] - (factor * 3.0f / 8.0f) - (factor / 8.0f)),
                                -1.5f,
                                factor * mapData[i + 3] - factor);
                        temp1.m_type = 1;
                    } else if (mapData[i + 4] == 90.0f) {
                        temp1.m_aabb.max = glm::vec3(factor * mapData[i + 1] + factor,
                                0.15f,
                                (factor * mapData[i + 3] + (factor * 3.0f / 8.0f) + (factor / 8.0f)));
                        temp1.m_aabb.min = glm::vec3(factor * mapData[i + 1] - factor,
                                -1.5f,
                                (factor * mapData[i + 3] + (factor * 3.0f / 8.0f) - (factor / 8.0f)));
                        temp1.m_type = 1;
                    }
                    if (numRefuels > bbs.size())
                        bbs.push_back(temp1);
                }
                obsTexture->bind(GL_TEXTURE0);
                glUniform1i(selector, 1);
                switch (obstacleType) {
                    case 1:
                        // Obstacle left
                        
                        obs = glm::rotate(obs, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                        obs = glm::translate(obs, glm::vec3(factor / 3.0f, 0.0f, 0.0f));
                        obs = glm::scale(obs, glm::vec3(factor / 3.0f, 3.0f, (factor * 2.0f)));
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        // Obstacle right
                        obs = glm::translate(obs, glm::vec3(-2.0f, 0.0f, 0.0f));
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);

                        if (mapData[i + 4] == 0.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f),
                                    yMin,
                                    factor * mapData[i + 3] - factor);
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.5f),
                                    yMax,
                                    factor * mapData[i + 3] + factor);
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.5f),
                                    yMin,
                                    factor * mapData[i + 3] - factor);
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f),
                                    yMax,
                                    factor * mapData[i + 3] + factor);
                        } else if (mapData[i + 4] == 90.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] - factor,
                                    yMin,
                                    factor * mapData[i + 3] + (factor * 0.16f));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + factor,
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.5f));
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - factor,
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.5f));
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] + factor,
                                    yMax,
                                    factor * mapData[i + 3] - (factor * 0.16f));
                        }
                        for (unsigned int i = 0; i < 2; i++)
                            if (numObstacles > obstaclebbs.size())
                                obstaclebbs.push_back(obstacle[i]);
                        break;
                    case 2:
                        // Obstacle center
                        obs = glm::rotate(obs, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                        //obs = glm::translate(obs, glm::vec3(factor / 3.0f, 0.0f, 0.0f));
                        obs = glm::scale(obs, glm::vec3(factor / 3.0f, 3.0f, (factor * 2.0f)));
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        if (mapData[i + 4] == 0.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f),
                                    yMin,
                                    factor * mapData[i + 3] - factor);
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f),
                                    yMax,
                                    factor * mapData[i + 3] + factor);
                        } else if (mapData[i + 4] == 90.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] - factor,
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.16f));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + factor,
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.16f));
                        }
                        if (numObstacles > obstaclebbs.size())
                            obstaclebbs.push_back(obstacle[0]);
                        break;
                    default:
                        break;
                }
                break;
            case 1:
                // Wall 0
                brickWall->bind(GL_TEXTURE0);
                glUniform1i(selector, 0);
                wall = glm::rotate(wall, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                wall = glm::translate(wall, glm::vec3(factor * 0.5f, 0.0f, (factor * 1.5f)));
                wall = glm::scale(wall, glm::vec3(factor, wallScaleY, factor));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 1
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 2
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 3
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 4
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 5
                wall = glm::translate(wall, glm::vec3(3.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                if (mapData[i + 4] == 0.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - factor);
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] - factor);
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + factor);
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] + factor);
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                } else if (mapData[i + 4] == 90.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] - factor);
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMin,
                            factor * mapData[i + 3] + factor);
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] - factor);
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                } else if (mapData[i + 4] == 180.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] + factor);
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] - factor);
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + factor);
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - factor);
                } else if (mapData[i + 4] == 270.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] + factor);
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] - factor);
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + factor);
                }
                for (unsigned int i = 0; i < 3; i++)
                    if (numWalls > wallbbs.size())
                        wallbbs.push_back(walls[i]);
                break;
            case 2:
                brickWall->bind(GL_TEXTURE0);
                glUniform1i(selector, 0);
                // Wall 0
                wall = glm::rotate(wall, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                wall = glm::translate(wall, glm::vec3(factor * 1.0f, 0.0f, (factor * 2.0f)));
                wall = glm::scale(wall, glm::vec3(factor, wallScaleY, factor));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 1
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 2
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 3
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 4
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 5
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 6
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 7
                wall = glm::translate(wall, glm::vec3(4.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 8
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 9
                wall = glm::translate(wall, glm::vec3(-1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                if (mapData[i + 4] == 0.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 0.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 0.5f));
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                    walls[3].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[3].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                } else if (mapData[i + 4] == 90.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 0.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 0.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[3].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[3].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                } else if (mapData[i + 4] == 180.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 0.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 0.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[3].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[3].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                } else if (mapData[i + 4] == 270.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 0.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + (factor * 0.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                    walls[2].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[2].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[3].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[3].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.5f));
                }
                for (unsigned int i = 0; i < 4; i++)
                    if (numWalls > wallbbs.size())
                        wallbbs.push_back(walls[i]);

                break;
            case 3:
                // Wall 0
                brickWall->bind(GL_TEXTURE0);
                glUniform1i(selector, 0);
                wall = glm::rotate(wall, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                wall = glm::translate(wall, glm::vec3(factor * 1.5f, 0.0f, (factor * 2.0f)));
                wall = glm::scale(wall, glm::vec3(factor, wallScaleY, factor));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 1
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 2
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 3
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 4
                wall = glm::translate(wall, glm::vec3(-3.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 5
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 6
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 7
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                if (mapData[i + 4] == 0.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                } else if (mapData[i + 4] == 90.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.0f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.0f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                }
                for (int i = 0; i < 2; i++)
                    if (numWalls > wallbbs.size())
                        wallbbs.push_back(walls[i]);
                obsTexture->bind(GL_TEXTURE0);
                glUniform1i(selector, 1);
                if(obstacleType == 3){
                        // Obstacle front
                        obs = glm::rotate(obs, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                        
                        obs = glm::scale(obs, glm::vec3(factor / 3.0f, 3.0f, factor / 3.0f));
                        obs = glm::translate(obs, glm::vec3(glm::sin(glm::radians(obsMov)), 0.0f, 2.0f));
                        
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        
                        obs = glm::translate(obs, glm::vec3(-2.0f * glm::sin(glm::radians(obsMov)), 0.0f, -4.0f));
                        
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        // Obstacle back
                        
                        if (mapData[i + 4] == 0.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMin,
                                    factor * mapData[i + 3] + (factor / 2.0f));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 5.0f / 6.0f));
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 5.0f / 6.0f));
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMax,
                                    factor * mapData[i + 3] - (factor / 2.0f));
                        } else if (mapData[i + 4] == 90.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] + (factor / 2.0f),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 5.0f / 6.0f),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 5.0f / 6.0f),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] - (factor / 2.0f),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                        }
                        for( int i = 0; i < 2; i++)
                            if (numObstacles > obstaclebbs.size())
                                obstaclebbs.push_back(obstacle[i]);
                }
                break;
            case 4:
                // Wall 0
                brickWall->bind(GL_TEXTURE0);
                glUniform1i(selector, 0);
                wall = glm::rotate(wall, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                wall = glm::translate(wall, glm::vec3(factor * 1.5f, 0.0f, factor));
                wall = glm::scale(wall, glm::vec3(factor, wallScaleY, factor));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 1
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 2
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 3
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, -1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 4
                wall = glm::translate(wall, glm::vec3(-3.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 5
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 6
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Wall 7
                wall = glm::translate(wall, glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                if (mapData[i + 4] == 0.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) + factor,
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.5f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.0f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 1.5f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.0f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 1.5f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) - factor,
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.5f));
                } else if (mapData[i + 4] == 90.0f) {
                    walls[0].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 2.5f),
                            yMin,
                            factor * mapData[i + 3] - (factor * 2.0f));
                    walls[0].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 1.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] - (factor * 1.0f));
                    walls[1].m_aabb.min = glm::vec3((factor * mapData[i + 1]) - (factor * 1.5f),
                            yMin,
                            factor * mapData[i + 3] + (factor * 1.0f));
                    walls[1].m_aabb.max = glm::vec3((factor * mapData[i + 1]) + (factor * 2.5f),
                            yMax * 10.0f,
                            factor * mapData[i + 3] + (factor * 2.0f));
                }
                for (int i = 0; i < 2; i++)
                    if (numWalls > wallbbs.size())
                        wallbbs.push_back(walls[i]);
                obsTexture->bind(GL_TEXTURE0);
                glUniform1i(selector, 1);
                if(obstacleType == 3){
                        // Obstacle front
                        obs = glm::rotate(obs, glm::radians(mapData[i + 4]), glm::vec3(0.0f, 1.0f, 0.0f));
                        
                        obs = glm::scale(obs, glm::vec3(factor / 3.0f, 3.0f, factor / 3.0f));
                        obs = glm::translate(obs, glm::vec3(glm::sin(glm::radians(obsMov)), 0.0f, 2.0f));
                        
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        
                        obs = glm::translate(obs, glm::vec3(-2.0f * glm::sin(glm::radians(obsMov)), 0.0f, -4.0f));
                        
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obs));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        // Obstacle back
                        
                        if (mapData[i + 4] == 0.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMin,
                                    factor * mapData[i + 3] + (factor / 2.0f));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 5.0f / 6.0f));
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 5.0f / 6.0f));
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))),
                                    yMax,
                                    factor * mapData[i + 3] - (factor / 2.0f));
                        } else if (mapData[i + 4] == 90.0f) {
                            obstacle[0].m_aabb.min = glm::vec3(factor * mapData[i + 1] + (factor / 2.0f),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[0].m_aabb.max = glm::vec3(factor * mapData[i + 1] + (factor * 5.0f / 6.0f),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.16f) - (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[1].m_aabb.min = glm::vec3(factor * mapData[i + 1] - (factor * 5.0f / 6.0f),
                                    yMin,
                                    factor * mapData[i + 3] - (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                            obstacle[1].m_aabb.max = glm::vec3(factor * mapData[i + 1] - (factor / 2.0f),
                                    yMax,
                                    factor * mapData[i + 3] + (factor * 0.16f) + (factor / 3.0f * glm::sin(glm::radians(obsMov))));
                        }
                        for( int i = 0; i < 2; i++)
                            if (numObstacles > obstaclebbs.size())
                                obstaclebbs.push_back(obstacle[i]);
                }
                break;
            default:
                break;
        }
        
        glUniform1i(selector, 0);
    }
}
