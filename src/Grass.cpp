#include "Grass.h"

#pragma once

void Grass::render( Camera& camera2, Shader& shader)
{
	

	for (GrassChunk chunk : grassChunks) {
		chunk.camera = &camera2;
		chunk.windDir = windDir; 
		chunk.bendScale = bendScale;
	
		chunk.render(grass, animator, animation, shader); 
	}
}

void Grass::initialize() {

	for (float y = start.y; y < stop.y; y += GRASS_CHUNK_SIZE) {
		for (float x = start.x; x <stop.x; x += GRASS_CHUNK_SIZE) {
			grassChunks.emplace_back(glm::vec2(x, y), glm::vec2(x + GRASS_CHUNK_SIZE, y + GRASS_CHUNK_SIZE), scarcity);
		}
	}
}



void GrassChunk::render(Model* grass, Animator* animator, Animation* animation, Shader& shader)
{
	for (glm::vec2 pos : grassPosiions) {

		//Check for body collision:
		glm::vec3 distance = glm::vec3(pos.x, 1, pos.y) - camera->position; 

		if (glm::length(distance) <= cameraVolume) {
			//cout << "Grass pushed" << endl; 
			float modifier = 5; 
			float displacementx = 20 * modifier; 
			float displacementz = 20 * modifier;
			if (distance.x > 0) {
				displacementx = -20 * modifier;
			}

			if (distance.z < 0) {
				displacementz = -20 * modifier;
			}
			shader.setVec4("vWind", (1.0f - (glm::length(distance)/cameraVolume))*15.0f* glm::vec4( distance.x, 0, 0, 0) + (1.0f - (glm::length(distance) / cameraVolume))* glm::vec4(displacementx, 0, displacementz, 0) + windDir);
			shader.setFloat("fBendScale", (1.0f - (glm::length(distance) / cameraVolume)) /200);

		}
		else {
			shader.setVec4("vWind", windDir);
			shader.setFloat("fBendScale", bendScale/200);
		}

		glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
		modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(pos.x, 1.0f,pos.y) /*+ glm::vec3(0.8f, 0.8f, 0.8f)*/ );
		modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(0.01f, 0.01f, 0.01f));
		//modelMatrixAnim = glm::rotate(modelMatrixAnim, 3.14159f, glm::vec3(0, 0, 1));
		//modelMatrixAnim = glm::rotate(modelMatrixAnim, 1.5708f, glm::vec3(0, 1, 0));
		//1.5708

		shader.use();
		shader.setMat4("model", modelMatrixAnim);

		grass->draw(shader);


	}

}

GrassChunk::GrassChunk(glm::vec2 start, glm::vec2 stop, float scarcity)
{
	//Initializing grass positions:
	glm::vec2 area = glm::abs(stop - start);
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(-scarcity / 2.0f, scarcity / 2.0f);
	numberOfBlades= (unsigned int)((area.x / scarcity) * (area.y / scarcity));
	unsigned int bladeNumber = 0;
	for (float y = start.y; y < stop.y; y += scarcity) {
		for (float x = start.x; x < stop.x; x += scarcity) {
			grassPosiions.push_back(glm::vec2(x + distr(eng), y + distr(eng)));
			bladeNumber++;
			if (bladeNumber >= numberOfBlades) {
				return;
			}
				
		}
	}


}

float const GrassChunk::getDistanceToCam(glm::vec3& cameraPosition)
{
	return cameraPosition.x;
}
