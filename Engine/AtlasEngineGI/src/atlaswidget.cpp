#include <fstream>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

#include "include/atlaswidget.h"
#include "include/render/renderer.h"
#include "include/data/model.h"

#include "include/render/process/geometryrenderprocess.h"
#include "include/render/process/lightingrenderprocess.h"
#include "include/render/process/hdrrenderprocess.h"
#include "include/render/process/shadowmaprenderprocess.h"
#include "include/render/process/computeshaderprocess.h"
#include "include/render/process/indirectlightingprocess.h"

#include "include/loader/neuralnetworkloader.h"

AtlasWidget::AtlasWidget(QWidget * parent) :
#if QT_VERSION >= 0x050000
    QOpenGLWidget(parent),
#else
    QGLWidget(parent),
#endif
    m_menu(this, Qt::FramelessWindowHint),
    m_paused(false),
    m_last_frame(0.0),
    m_fullscreen(false),
    m_current_scene(0),
    m_current_scene_index(-1)
{
    setFocus();

    setMouseTracking(true);

    m_path = QApplication::applicationFilePath().toStdString();

    m_path = m_path.substr(0, m_path.find_last_of('/'));
#if defined(__APPLE__) || defined(__linux__)
    m_path = m_path.substr(0, m_path.find_last_of('/'));
#endif

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(41);
    m_time.start();

    for(GLuint i = 0; i < 1024; ++i)
        m_keys[i] = false;
}

AtlasWidget::~AtlasWidget()
{
    for(GLuint i = 0; i < m_scenes.size(); ++i)
        delete m_scenes[i];

    m_scenes.clear();
}

void AtlasWidget::initializeGL()
{
#if defined(_WIN32)
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    makeCurrent();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    m_renderer.init();

    /*  MATERIALS MODIFICATION */

    m_material_library.addMaterial(new Material(glm::vec3(1.f), 0.2f, 1.f, 1.f, 1.f), "default");
    m_material_library.addMaterial(new Material(glm::vec3(1.f, 1.f, 1.f), 1.f, 0.f, 1.f, 1.f), "white");
    m_material_library.addMaterial(new Material(glm::vec3(0.8f, 0.8f, 0.8f), 1.f, 0.f, 1.f, 1.f), "grey");
    m_material_library.addMaterial(new Material(glm::vec3(1.f, 0.f, 0.f), 1.f, 0.f, 1.f, 1.f), "red");
    m_material_library.addMaterial(new Material(glm::vec3(1.f, 1.f, 0.f), 0.2f, 0.f, 1.f, 1.f), "yellow");
    m_material_library.addMaterial(new Material(glm::vec3(0.5f, 0.5f, 0.5f), 0.01f, 1.f, 1.f, 1.f), "glossy");

    /*  END OF MATERIALS MODIFICATION */


    /*  SCENES MODIFICATION */

    createRenderScene();

    /*  END OF SCENES MODIFICATION */

    for(GLuint i = 0; i < m_scenes.size(); ++i)
    {
        m_scenes[i]->buildModelList();
        m_scenes[i]->buildAABB();
        Camera *new_camera = new Camera();
        new_camera->setPosition(m_scenes[i]->getAABB().getCenter());
        m_scenes[i]->addCamera(new_camera);
    }

    /*  PIPELINES MODIFICATIONS */

    GeometryRenderProcess *geometry_render_process = new GeometryRenderProcess();
    LightingRenderProcess *lighting_render_process = new LightingRenderProcess(m_current_scene->numberOfPointLights());
    HDRRenderProcess *hdr_render_process = new HDRRenderProcess();
    ShadowMapRenderProcess *shadow_map_render_process = new ShadowMapRenderProcess(m_current_scene->numberOfPointLights());
	ComputeShaderProcess *compute_shader_render_process = new ComputeShaderProcess();
	IndirectLightingProcess *indirect_lighting_process = new IndirectLightingProcess(
		m_current_scene->getCurrentCamera()->getPosition(),
		m_current_scene->getPointLight(0)->getPosition());

    //connectProcesses(geometry_render_process, lighting_render_process, {0, 1, 2, 3}, {0, 1, 2, 3});
    //connectProcesses(shadow_map_render_process, lighting_render_process, {0}, {4});
    //connectProcesses(lighting_render_process, hdr_render_process, {0, 1, 2}, {0, 1, 2});

    Pipeline *default_pipeline = new Pipeline(window()->width(), window()->height());

    //default_pipeline->setLastProcess(hdr_render_process);
	//default_pipeline->setLastProcess(compute_shader_render_process);
	default_pipeline->setLastProcess(indirect_lighting_process);

    m_renderer.addPipeline(default_pipeline, "default");

    setCurrentPipeline("default");

    /*  END OF PIPELINES MODIFICATION */

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
}

void AtlasWidget::resizeGL(int w, int h)
{
    window()->setMaximumSize(QSize(w, h));
    window()->setMinimumSize(QSize(w, h));
    setMaximumSize(QSize(w, h));
    setMinimumSize(QSize(w, h));

    m_renderer.resize(w, h);

    m_mouse_last_x = w * 0.5;
    m_mouse_last_y = h * 0.5;

    QPoint glob = mapToGlobal(QPoint(m_mouse_last_x, m_mouse_last_y));
    QCursor::setPos(glob);
}

void AtlasWidget::paintGL()
{
    GLuint current_time = m_time.elapsed();
    m_render_time = current_time;

    m_renderer.drawScene(*m_current_scene, (float)(current_time - m_last_frame), m_keys);

    m_last_frame = current_time;
}

void AtlasWidget::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        pause();
        break;

    case Qt::Key_F:
        if(m_fullscreen)
        {
            resizeGL(400, 300);
            window()->setWindowState(Qt::WindowMaximized);
        }
        else
        {
            window()->setWindowState(Qt::WindowFullScreen);
            QRect rec = QApplication::desktop()->screenGeometry();
            GLuint height = rec.height(),
                   width = rec.width();

            resizeGL(width, height);
        }
        m_fullscreen = !m_fullscreen;
        break;

    default:
        if(e->key() >= 0 && e->key() < 1024)
            m_keys[e->key()] = true;
        break;
    }
}

void AtlasWidget::keyReleaseEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Escape)
        QCoreApplication::instance()->quit();
    else if(e->key() >= 0 && e->key() < 1024)
        m_keys[e->key()] = false;
}

void AtlasWidget::mouseMoveEvent(QMouseEvent *e)
{
    GLfloat xoffset = QCursor::pos().x() - m_mouse_last_x;
    GLfloat yoffset = m_mouse_last_y - QCursor::pos().y();

    if(xoffset != 0 || yoffset != 0)
        m_current_scene->updateCamera(xoffset, yoffset);

     QCursor::setPos(QPoint(m_mouse_last_x, m_mouse_last_y));
}

void AtlasWidget::setCurrentPipeline(const std::string &pipeline_name)
{
    m_renderer.setCurrentPipeline(pipeline_name);

    m_menu.setGraphicsMenuElements(m_renderer.getGraphicsMenuElements());

    m_menu.init();
}

void AtlasWidget::unPause()
{
    if(m_paused == true)
    {
        m_paused = false;

        QApplication::setOverrideCursor(Qt::BlankCursor);
    }
}

void AtlasWidget::pause()
{
    if(m_paused == false)
    {
        m_paused = true;

        QApplication::setOverrideCursor(Qt::ArrowCursor);
        m_menu.exec();
    }
}

void AtlasWidget::createRenderScene()
{
    NeuralNetworkLoader neural_network_loader;
    NeuralNetwork neural_network;
    //neural_network_loader.loadFile("neuralnetwork.xml", neural_network);

    addScene();

    m_obj_loader.loadFile("scenes/cornell.obj", m_current_scene, m_material_library);
	//m_pbrt_loader.loadFile("scenes/cornell-box.pbrt", m_current_scene, m_material_library);

    //m_current_scene->addPointLight(new PointLight(glm::normalize(glm::vec3(17.f, 12.f, 4.f)), 10.f, glm::vec3(0.f, 1.5f, 0.f)));
    //m_current_scene->addPointLight(new PointLight(glm::normalize(glm::vec3(17.f, 12.f, 4.f)), 100.f, glm::vec3(0.f, 15.f, 0.f)));
    m_current_scene->scale(glm::vec3(10.f), "root");
}
