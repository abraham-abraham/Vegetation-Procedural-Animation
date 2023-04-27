#include "Rain.h"

Rain::Rain()
{
    initialize();
}

void Rain::initialize() {
    for (int i = 0; i < maxParticles; i++) {

        for (int j = 0; j < maxParticles; j++) {
            initializeHelper(i, j);
        }
    }
}

void Rain::initializeHelper(int i, int j) {
    par_sys[i][j].alive = true;
    par_sys[i][j].life = 3.0;
    par_sys[i][j].fade = float(rand() % 100) / 1000.0f + 0.003f;

    par_sys[i][j].position = glm::vec3(WCX*i -10, 20, WCY* j -3);

    par_sys[i][j].vel_y = velocity;
    par_sys[i][j].gravity = -2;//-0.8;
}

void Rain::drawRain(Model& m, Shader& s)
{
    
    for (int i = 0; i < maxParticles; i++) {
        for (int j = 0; j < maxParticles; j++) {
            if (par_sys[i][j].alive) {
                
              
                //Model matrix:
                glm::mat4 modelMatrixRain = glm::mat4(1.0f);
                modelMatrixRain = glm::translate(modelMatrixRain, glm::vec3(0, 1.0f, 0) + par_sys[i][j].position);
                modelMatrixRain = glm::scale(modelMatrixRain, glm::vec3(0.05f, 0.05f, 0.05f));


                s.setMat4("model", modelMatrixRain);

                m.draw(s);

                par_sys[i][j].position.y += par_sys[i][j].vel_y / (slowdown * 1000);
                par_sys[i][j].vel_y += par_sys[i][j].gravity;
                par_sys[i][j].life -= par_sys[i][j].fade;


                if (par_sys[i][j].position.y <= 1.0f) {
                    par_sys[i][j].life = -1.0;
                }

                /*for (int k = 0; k < 4; k++) {
                    if (glm::abs(par_sys[i][j].position.x - positionsToNeutralize[k].x) <= 1 && glm::abs(par_sys[i][j].position.z - positionsToNeutralize[k].z) <= 1) {
                        if (par_sys[i][j].position.y <= 5) {
                            par_sys[i][j].life = -1.0;
                        }
                    }
                }*/

                if (par_sys[i][j].life < 0.0) {
                    initializeHelper(i, j);

                }
            }
        }
    
    }
}
