#include "switchlightingprocess.h"
#include "quad.h"

#include <qlabel.h>
#include <qcombobox.h>
#include <qboxlayout.h>

SwitchLightingProcess::SwitchLightingProcess() :
	RenderProcess(2)
{

}

void SwitchLightingProcess::init(const GLuint &width, const GLuint &height)
{
	RenderProcess::init(width, height);

	m_shader.initDefine("direct_lighting", Define("#define DIRECT_LIGHTING", GL_TRUE));
	m_shader.initDefine("indirect_lighting", Define("#define INDIRECT_LIGHTING", GL_FALSE));
	m_shader.initDefine("complete_lighting", Define("#define COMPLETE_LIGHTING", GL_FALSE));
	m_shader.init("shaders/switch.vert", "shaders/switch.frag");

	m_buffer.init(width, height);
	std::vector<FramebufferTextureDatas> lighting_textures_datas;
	lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
	lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
	lighting_textures_datas.push_back(FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER));
	m_buffer.attachTextures(lighting_textures_datas);

	m_shader.use();
	glUniform1i(glGetUniformLocation(m_shader.getProgram(), "directLighting"), 0);
	glUniform1i(glGetUniformLocation(m_shader.getProgram(), "indirectLighting"), 1);
	glUseProgram(0);

	m_out_textures.push_back(m_buffer.getTexture(0));
	m_out_textures.push_back(m_buffer.getTexture(1));
	m_out_textures.push_back(m_buffer.getTexture(2));
}

void SwitchLightingProcess::initMenuElement()
{
	QLabel *rendering_label = new QLabel("Rendering type");

	QComboBox *combo_box = new QComboBox;

	combo_box->addItem("Only direct lighting");
	combo_box->addItem("Only indirect lighting");
	combo_box->addItem("Both");

	MenuElement::connect(combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(switchLightingType(int)));

	QVBoxLayout *vertical_layout = new QVBoxLayout;
	vertical_layout->addWidget(rendering_label);
	vertical_layout->addWidget(combo_box);

	m_menu_element = new MenuElement();
	m_menu_element->setLayout(vertical_layout);
}

void SwitchLightingProcess::resize(const GLuint &width, const GLuint &height)
{
	m_buffer.resize(width, height);
}

void SwitchLightingProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean(&keys)[1024])
{
	m_buffer.bind();
	glViewport(0, 0, m_buffer.width(), m_buffer.height());
	glClear(GL_COLOR_BUFFER_BIT);
	m_shader.use();

	glActiveTexture(GL_TEXTURE0);
	bindPreviousTexture(0);
	glActiveTexture(GL_TEXTURE1);
	bindPreviousTexture(1);

	quad.draw();
}

void SwitchLightingProcess::switchLightingType(int index) 
{
	switch (index)
	{
	case DIRECT_LIGHTING:
		m_shader.setDefined("direct_lighting", GL_TRUE);
		m_shader.setDefined("indirect_lighting", GL_FALSE);
		m_shader.setDefined("complete_lighting", GL_FALSE);
		break;
	case INDIRECT_LIGHTING:
		m_shader.setDefined("direct_lighting", GL_FALSE);
		m_shader.setDefined("indirect_lighting", GL_TRUE);
		m_shader.setDefined("complete_lighting", GL_FALSE);
		break;
	case COMPLETE_LIGHTING:
		m_shader.setDefined("direct_lighting", GL_FALSE);
		m_shader.setDefined("indirect_lighting", GL_FALSE);
		m_shader.setDefined("complete_lighting", GL_TRUE);
		break;
	}

	m_shader.reload();

}