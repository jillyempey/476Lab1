#include <iostream>
#include "Drawable.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


void Drawable::setModel(shared_ptr<Program> prog, shared_ptr<MatrixStack>M) {
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    }
void Drawable::SetMaterial(int i, shared_ptr<Program> prog) {
        switch (i) {
            case 0: //shiny blue plastic
                glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
                glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9); glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8); glUniform1f(prog->getUniform("shine"), 120.0);
                break;
            case 1: // flat grey
                glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
                glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
                glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
                glUniform1f(prog->getUniform("shine"), 4.0);
                break;
            case 2: //hot pink1
                glUniform3f(prog->getUniform("MatAmb"), 1, 0.714, 0.757);
                glUniform3f(prog->getUniform("MatDif"), 1, 0.714, 0.757);
                glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;
            case 3: // green / emerald - my unique material
                glUniform3f(prog->getUniform("MatAmb"), 0.0215, 0.1745, 0.0215);
                glUniform3f(prog->getUniform("MatDif"), 0.03568, 0.41424, 0.03568);
                glUniform3f(prog->getUniform("MatSpec"), 0.633, 0.727811, 0.633);
                glUniform1f(prog->getUniform("shine"), 0.6);
                break;
            case 4: //hot pink1
                glUniform3f(prog->getUniform("MatAmb"), 1, 0.07843, 0.576);
                glUniform3f(prog->getUniform("MatDif"), 1, 0.07843, 0.576);
                glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;
            case 5: // hot pink2
                glUniform3f(prog->getUniform("MatAmb"), 0.35, 0.0725, 0.2725);
                glUniform3f(prog->getUniform("MatDif"), 1.0, .78, .682);
                glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.1, 0.5);
                glUniform1f(prog->getUniform("shine"), .078125);
                break;
            case 6: // neon green
                glUniform3f(prog->getUniform("MatAmb"), 0.2, 0.40725, 0.10725);
                glUniform3f(prog->getUniform("MatDif"), 0.78, 1.0, .396);
                glUniform3f(prog->getUniform("MatSpec"), 0.4, 0.7, 0.4);
                glUniform1f(prog->getUniform("shine"), .078125);
                break;
            case 7: //black rubber
                glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.02, 0.02);
                glUniform3f(prog->getUniform("MatDif"), 0.01, 0.01, 0.01);
                glUniform3f(prog->getUniform("MatSpec"), 0.4, 0.4, 0.4);
                glUniform1f(prog->getUniform("shine"), .078125);
                break;
            case 8: //jade
                glUniform3f(prog->getUniform("MatAmb"), 0.135, 0.2225, 0.1575);
                glUniform3f(prog->getUniform("MatDif"), .54, .89, 0.63);
                glUniform3f(prog->getUniform("MatSpec"), 0.316228, 0.316228, 0.316228);
                glUniform1f(prog->getUniform("shine"), 0.1);
                break;
            case 9: //bronze
                glUniform3f(prog->getUniform("MatAmb"), 0.25, 0.148, 0.06475);
                glUniform3f(prog->getUniform("MatDif"), .4, .2368, 0.1036);
                glUniform3f(prog->getUniform("MatSpec"), 0.774597, 0.458561, 0.200621);
                glUniform1f(prog->getUniform("shine"), 76.8);
                break;
            case 10: //pearl
                glUniform3f(prog->getUniform("MatAmb"), 0.25, 0.20725, 0.20725);
                glUniform3f(prog->getUniform("MatDif"), 1.0, .829, 0.829);
                glUniform3f(prog->getUniform("MatSpec"), 0.296648, 0.296648, 0.296648);
                glUniform1f(prog->getUniform("shine"), 11.264);
                break;
            case 11: //ruby
                glUniform3f(prog->getUniform("MatAmb"), 0.1745, 0.01175, 0.01175);
                glUniform3f(prog->getUniform("MatDif"), .61424, .04136, 0.04136);
                glUniform3f(prog->getUniform("MatSpec"), 0.727811, 0.626959, 0.626959);
                glUniform1f(prog->getUniform("shine"), 76.8);
                break;
        }
    }