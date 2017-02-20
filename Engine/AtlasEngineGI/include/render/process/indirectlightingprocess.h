#pragma once
#ifndef INDIRECTLIGHTINGPROCESS_H
#define INDIRECTLIGHTINGPROCESS_H

#include "renderprocess.h"
#include "framebuffer.h"
#include "shader.h"
#include "computeshader.h"
#include "neuralnetwork.h"
#include "neuralnetworkloader.h"

/*
* IN:
* -RGB			  : fragment's position
* -RGB			  : normals
* -vec3			  : camera's position
* -vec3			  : point light's position
* -Neural Network : neural network
*
* OUT:
* -RGB  : fragment color
* */

class IndirectLightingProcess : public RenderProcess
{
public:
	IndirectLightingProcess(const glm::vec3 &camera_position, const glm::vec3 &point_light_position);

	virtual void init(const GLuint &width, const GLuint &height);
	virtual void initMenuElement();

	virtual void resize(const GLuint &width, const GLuint &height);
	virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean(&keys)[1024]);

	// Getters
	inline ComputeShader getShader() const { return m_shader; }

private:

	ComputeShader m_shader;
	NeuralNetwork m_neural_network;
	glm::vec3 m_camera_position;
	glm::vec3 m_point_light_position;

	Texture m_out_texture;
	int m_tex_w;
	int m_tex_h;
};

#endif // INDIRECTLIGHTINGPROCESS_H
