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

IndirectLightingProcess::IndirectLightingProcess(const glm::vec3 &camera_position, const glm::vec3 &point_light_position) :
	RenderProcess(0)
{
	m_camera_position = camera_position;
	m_point_light_position = point_light_position;
}

void IndirectLightingProcess::init(const GLuint &width, const GLuint &height)
{
	RenderProcess::init(width, height);

	m_shader = ComputeShader("shaders/computeshader2");

	/*
	* Neural Network initialization
	* */
	NeuralNetworkLoader neural_network_loader;
	neural_network_loader.loadFile("networksXML/neuralnetwork.xml", m_neural_network);

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

	int work_grp_cnt[3], work_grp_inv;
	// Maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	//printf ("max global (total) work group size x:%i y:%i z:%i\n",
	//work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];
	// Maximum local work group (one shader's slice)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	//printf ("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
	//work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	// Maximum compute shader invocations (x * y * z)
	//glGetIntegerv(GL_MAX_COMPUTE_LOCAL_INVOCATIONS, &work_grp_inv); // LINUX
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	//printf ("max computer shader invocations %i\n", work_grp_inv);

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

	/*
	* Working groups
	* */
	const int globalWorkGroupSize = m_tex_w*m_tex_h;
	const int localWorkGroupSize = 32;

	/*
	* Inputs buffers
	* */
	GLuint structBufferNeuralNetwork;
	glGenBuffers(1, &structBufferNeuralNetwork);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, structBufferNeuralNetwork);
	glBufferData(GL_SHADER_STORAGE_BUFFER, globalWorkGroupSize * sizeof(NeuralNetwork), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // invalidate makes a ig difference when re-writting

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, structBufferNeuralNetwork);

	m_out_texture.bindImage(0);

	// Launch compute shader
	m_shader.use();

	// Uniform inputs
	glUniform3fv(glGetUniformLocation(m_shader.getProgram(), "camera_position"), 3, glm::value_ptr(m_camera_position));
	glUniform3fv(glGetUniformLocation(m_shader.getProgram(), "point_light_position"), 3, glm::value_ptr(m_point_light_position));

	//glDispatchCompute(nextPowerOfTwo(m_tex_w / localWorkGroupSize), nextPowerOfTwo(m_tex_h / localWorkGroupSize), 1); // using working spaces
	glDispatchCompute(m_tex_w, m_tex_h, 1); //using no working spaces

	// Prevent samplign before all writes to image are done
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}