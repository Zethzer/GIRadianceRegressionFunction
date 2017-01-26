#include "include/data/camera.h"
#include "include/render/shader.h"

Camera::Camera() :
    m_current_offset_x(0.0f),
    m_current_offset_y(0.0f),
    m_saved_offset_x(0.0f),
    m_saved_offset_y(0.0f),
    m_yaw(0.0f),
    m_pitch(0.0f),
    m_fov(45.5f),
    m_speed(0.005f),
    m_position(glm::vec3(0.0f, 0.0f,  0.0f)),
    m_front(glm::vec3(1.0f, 0.0f, 0.0f)),
    m_up(glm::vec3(0.0f, 1.0f,  0.0f)),
    m_near(1.f),
    m_far(100.f)
{

}

Camera::Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, const GLfloat& speed, const GLfloat& fov) :
    m_current_offset_x(0.0f),
    m_current_offset_y(0.0f),
    m_saved_offset_x(0.0f),
    m_saved_offset_y(0.0f),
    m_yaw(0.0f),
    m_pitch(0.0f),
    m_fov(fov),
    m_speed(speed),
    m_position(pos),
    m_front(front),
    m_up(up),
    m_near(1.f),
    m_far(100.f)
{

}

Camera::~Camera()
{

}


void Camera::init(const glm::vec3 &pos, const glm::vec3 &front, const glm::vec3 &up, const GLfloat &speed, const GLfloat &fov)
{
    m_fov = fov;
    m_position = pos;
    m_front = front;
    m_up = up;
    m_speed = speed;
}

void Camera::move(const GLboolean keys[], const GLfloat &delta_time)
{
    float speed = m_speed * delta_time;
    if(keys[Qt::Key_Z])
        m_position += m_front * speed;
    if(keys[Qt::Key_S])
        m_position -= m_front * speed;
    if(keys[Qt::Key_Q])
        m_position -= glm::cross(m_front, m_up) * speed;
    if(keys[Qt::Key_D])
        m_position += glm::cross(m_front, m_up) * speed;
}

void Camera::orientate()
{
    if((m_current_offset_x != m_saved_offset_x) && (m_current_offset_y != m_saved_offset_y))
    {
        GLfloat sensitivity = 0.15f;

        m_current_offset_x *= sensitivity;
        m_current_offset_y *= sensitivity;

        m_yaw   += m_current_offset_x;
        m_pitch += m_current_offset_y;

        if(m_pitch > 89.0f)
            m_pitch =  89.0f;
        if(m_pitch < -89.0f)
            m_pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
        front.y = sin(glm::radians(m_pitch));
        front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

        glm::vec3 up;
        up.x = cos(glm::radians(m_pitch + 90.f)) * cos(glm::radians(m_yaw));
        up.y = sin(glm::radians(m_pitch + 90.f));
        up.z = cos(glm::radians(m_pitch + 90.f)) * sin(glm::radians(m_yaw));

        m_front = glm::normalize(front);
        m_up = glm::normalize(up);

        m_saved_offset_x = m_current_offset_x;
        m_saved_offset_y = m_current_offset_y;
    }
}

void Camera::sendDatas(const Shader& shader, const GLfloat& screen_width, const GLfloat& screen_height)
{
    m_view = glm::lookAt(m_position, m_position + m_front, m_up);
    m_projection = glm::perspective(m_fov, screen_width * (1.f / screen_height), 1.f, 100.0f);

    glGetUniformLocation(shader.getProgram(), "viewPos");
    glUniform3f(glGetUniformLocation(shader.getProgram(), "viewPos"), m_position.x, m_position.y, m_position.z);
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
}
