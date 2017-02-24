#ifndef SWITCHLIGHTINGPROCESS_H
#define SWITCHLIGHTINGPROCESS_H

#include "openglincludes.h"
#include "renderprocess.h"
#include "texture.h"
#include "shader.h"
#include "framebuffer.h"

#include <qobject.h>


/*
* IN:
* -RGBA	: direct lighting
* -RGBA	: indirect lighting
*
* OUT:
* -RGB  : fragment color
* */

enum SwitchRenderType {
	DIRECT_LIGHTING = 0,
	INDIRECT_LIGHTING = 1,
	COMPLETE_LIGHTING = 2
};

class SwitchLightingProcess : public QObject, public RenderProcess
{
	Q_OBJECT
public:
	SwitchLightingProcess();

	virtual void init(const GLuint &width, const GLuint &height);
	virtual void initMenuElement();

	virtual void resize(const GLuint &width, const GLuint &height);
	virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean(&keys)[1024]);
	
private slots:
	void switchLightingType(int index);

private:

	Shader m_shader;

	Framebuffer m_buffer;
};

#endif // INDIRECTLIGHTINGPROCESS_H
