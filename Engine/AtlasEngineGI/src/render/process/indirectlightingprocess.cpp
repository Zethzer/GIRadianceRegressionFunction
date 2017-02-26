#include "indirectlightingprocess.h"
#include "scene.h"
#include "quad.h"
#include "neuralnetworkloader.h"
#include "neuralnetwork.h"
#include "qapplication.h"

/*
* Utility functions
* */
static int nextPowerOfTwo(int x) {
	x--;
	x |= x >> 1; // handle 2 bit numbers
	x |= x >> 2; // handle 4 bit numbers
	x |= x >> 4; // handle 8 bit numbers
	x |= x >> 8; // handle 16 bit numbers
	x |= x >> 16; // handle 32 bit numbers
	x++;
	return x;
}

// Scaling Layer 
float* ScalingLayerCalculateOutputs(float *inputs, ScalingLayer scaling_layer)
{
	float* outputs = (float *)malloc(sizeof(float) * 20);

	for (uint i = 0; i < 12 /*scaling_layer.statistics_size*/; ++i)
	{
		if ((scaling_layer.statistics[i].maximum - scaling_layer.statistics[i].minimum) < 1e-99)
		{
			outputs[i] = inputs[i];
		}
		else
		{
			outputs[i] = 2.0 * (inputs[i] - scaling_layer.statistics[i].minimum * (1.0 / (scaling_layer.statistics[i].maximum - scaling_layer.statistics[i].minimum)) - 1.0);
		}
	}

	return outputs;
}

// Combination
/**************************************************************************************************************************************/
float PerceptronLayerCalculateCombination(float *inputs, Perceptron perceptron, uint inputs_number)
{
	float combination = perceptron.bias;

	for (uint i = 0; i < inputs_number; ++i)
	{
		combination += perceptron.synaptic_weights[i] * inputs[i];
	}
	return combination;
}



float* PerceptronLayerCalculateCombinations(float *inputs, PerceptronLayer perceptron_layer, uint nb_perceptrons, uint inputs_number)
{
	float *combination = (float *)malloc(sizeof(float) * 20);
	;

	for (uint i = 0; i < nb_perceptrons /*perceptron_layer.perceptrons_size*/; ++i)
	{
		combination[i] = PerceptronLayerCalculateCombination(inputs, perceptron_layer.perceptrons[i], inputs_number);
	}
	return combination;
}
/**************************************************************************************************************************************/


// Activation
/**************************************************************************************************************************************/
float PerceptronCalculateActivation(float combination, Perceptron perceptron)
{
	switch (perceptron.activation_function) {
	case Linear:
		return combination;
	case HyperbolicTangent:
		return (1.0 - 2.0 / (exp(float(2.0 * combination)) + 1.0));
	}
}

float* PerceptronLayerCalculateActivations(float *combinations, PerceptronLayer perceptron_layer, uint nb_perceptrons)
{
	float *activations = (float *)malloc(sizeof(float) * 20);

	for (uint i = 0; i < nb_perceptrons /*perceptron_layer.perceptrons_size*/; ++i)
	{
		activations[i] = PerceptronCalculateActivation(combinations[i], perceptron_layer.perceptrons[i]);
	}
	return activations;
}
/**************************************************************************************************************************************/

// Multilayer Perceptron
float* MultilayerPerceptronCalculateOutputs(float *inputs, MultilayerPerceptron multilayer_perceptron)
{
	float *outputs = (float *)malloc(sizeof(float) * 20);

	uint architecture[4];
	architecture[0] = 12;
	architecture[1] = 20;
	architecture[2] = 10;
	architecture[3] = 3;

	if (multilayer_perceptron.layers_size == 0)
	{
		return inputs;
	}
	else
	{
		outputs = PerceptronLayerCalculateActivations(PerceptronLayerCalculateCombinations(inputs, multilayer_perceptron.perceptron_layers[0], architecture[1], architecture[0]), multilayer_perceptron.perceptron_layers[0], architecture[1]);

		for (uint i = 1; i < 3 /*multilayer_perceptron.layers_size*/; ++i)
		{
			outputs = PerceptronLayerCalculateActivations(PerceptronLayerCalculateCombinations(outputs, multilayer_perceptron.perceptron_layers[i], architecture[i + 1], architecture[i]), multilayer_perceptron.perceptron_layers[i], architecture[i + 1]);
		}
	}
	return outputs;
}

// Unscaling Layer
float* UnscalingLayerCalculateOutputs(float *inputs, UnscalingLayer unscaling_layer)
{
	float *outputs = (float *)malloc(sizeof(float) * 20);
	for (uint i = 0; i < 3 /*unscaling_layer.statistics_size*/; ++i)
	{
		if ((unscaling_layer.statistics[i].maximum - unscaling_layer.statistics[i].minimum) < 1e-99)
		{
			outputs[i] = inputs[i];
		}
		else
		{
			outputs[i] = 0.5 * (inputs[i] + 1.0) * (unscaling_layer.statistics[i].maximum - unscaling_layer.statistics[i].minimum) + unscaling_layer.statistics[i].minimum;
		}
	}
	return outputs;
}


float* computeNeuralNetworkResult(NeuralNetwork neural_network) {

	float inputs[12];
	inputs[0] = 1.0;
	inputs[1] = 2.0;
	inputs[2] = 3.0;
	inputs[3] = 4.0;
	inputs[4] = 5.0;
	inputs[5] = 6.0;
	inputs[6] = 7.0;
	inputs[7] = 8.0;
	inputs[8] = 9.0;
	inputs[9] = 10.0;
	inputs[10] = 11.0;
	inputs[11] = 12.0;


	float *outputs_scaling = ScalingLayerCalculateOutputs(inputs, neural_network.scaling_layer);
	float * outputs_multilayer = MultilayerPerceptronCalculateOutputs(outputs_scaling, neural_network.multilayer_perceptron);
	outputs_scaling = UnscalingLayerCalculateOutputs(outputs_multilayer, neural_network.unscaling_layer);

	return outputs_scaling;
}

IndirectLightingProcess::IndirectLightingProcess() :
	RenderProcess(2)
{
	
}

void IndirectLightingProcess::init(const GLuint &width, const GLuint &height)
{
	RenderProcess::init(width, height);

	//m_shader = ComputeShader("shaders/compute_optimise_2_2.comp");
	m_shader = ComputeShader("shaders/computeshader_neuralnetwork_optimise");
	
	/*
	* Inputs
	* */
	m_shader.use();
	glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gPositionDepth"), 0);
	glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gNormal"), 1);
	glUseProgram(0);

	/*
	* Creating the Output Texture
	* */
	m_out_texture = Texture(GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT,
		NULL, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

	m_tex_w = width;
	m_tex_h = height;	 

	/*
	* Determining the Work Group Size
	* */

	// Maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &m_work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &m_work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &m_work_grp_cnt[2]);
	//printf ("max global (total) work group size x:%i y:%i z:%i\n",
	//m_work_grp_cnt[0], m_work_grp_cnt[1], m_work_grp_cnt[2]);

	// Maximum local work group (one shader's slice)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &m_work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &m_work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &m_work_grp_size[2]);
	//printf ("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
	//m_work_grp_size[0], m_work_grp_size[1], m_work_grp_size[2]);

	// Maximum compute shader invocations (x * y * z)
#if defined (__linux__)
	glGetIntegerv(GL_MAX_COMPUTE_LOCAL_INVOCATIONS, &m_work_grp_inv);
#else
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &m_work_grp_inv);
#endif
	//printf ("max computer shader invocations %i\n", m_work_grp_inv);

	// Neural network buffer
	GLuint neural_network_buffer;
	glGenBuffers(1, &neural_network_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, neural_network_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(NeuralNetwork), NULL, GL_STATIC_DRAW);
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ; // invalidate makes a ig difference when re-writting

	NeuralNetwork *neural_network;
	neural_network = (NeuralNetwork *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(NeuralNetwork), bufMask);

	NeuralNetworkLoader neural_network_loader;
	neural_network_loader.loadFile("networksXML/neural_network_pierre.xml", *neural_network);
	
	float *rgb = computeNeuralNetworkResult(*neural_network);
	std::cout << rgb[0] << std::endl;
	std::cout << rgb[1] << std::endl;
	std::cout << rgb[2] << std::endl;
	

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, neural_network_buffer);

	// Outputs
	m_out_textures.push_back(&m_out_texture);

}

void IndirectLightingProcess::initMenuElement()
{

}

void IndirectLightingProcess::resize(const GLuint &width, const GLuint &height)
{
	RenderProcess::resize(width, height);

	m_out_texture = Texture(GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT,
		NULL, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

	m_tex_w = width;
	m_tex_h = height;
}

void IndirectLightingProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean(&keys)[1024])
{
	// Working groups
	const int localWorkGroupSize = 16;

	// Inputs
	glm::vec3 camera_position = scene.getCurrentCamera()->getPosition();
	glm::vec3 point_light_position = scene.getPointLight(0)->getPosition();


	m_out_texture.bindImage(0);

	// Launch compute shader
	m_shader.use();
	
	glActiveTexture(GL_TEXTURE0);
	bindPreviousTexture(0);
	glActiveTexture(GL_TEXTURE1);
	bindPreviousTexture(1);

	// Uniform inputs
	glUniform3f(glGetUniformLocation(m_shader.getProgram(), "camera_position"), camera_position.x, camera_position.y, camera_position.z);
	glUniform3f(glGetUniformLocation(m_shader.getProgram(), "point_light_position"), point_light_position.x, point_light_position.y, point_light_position.z);

	glDispatchCompute(nextPowerOfTwo(m_tex_w / localWorkGroupSize), nextPowerOfTwo(m_tex_h / localWorkGroupSize), 1); // using working spaces
	//glDispatchCompute(m_tex_w, m_tex_h, 1); //using no working spaces

	// Prevent samplign before all writes to image are done
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}