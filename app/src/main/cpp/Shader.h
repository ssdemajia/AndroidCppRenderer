#ifndef ANDROIDGLINVESTIGATIONS_SHADER_H
#define ANDROIDGLINVESTIGATIONS_SHADER_H

#include <string>
#include <android/asset_manager.h>
#include <GLES3/gl31.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AndroidOut.h"

class Model;

/*!
 * A class representing a simple shader program. It consists of vertex and fragment components. The
 * input attributes are a position (as a Vector3) and a uv (as a Vector2). It also takes a uniform
 * to be used as the entire model/view/projection matrix. The shader expects a single texture for
 * fragment shading, and does no other lighting calculations (thus no uniforms for lights or normal
 * attributes).
 */
class Shader {
public:
    static Shader* loadShader(AAssetManager *assetManager, const std::string &vertexPath, const std::string &fragmentPath);

    static Shader* loadShader(AAssetManager *assetManager, const std::string &computePath);

    static Shader *loadShader(const std::string &vertexSource, const std::string &fragmentSource);

    static Shader * loadShader(const std::string& computeSource);

    inline ~Shader() {
        if (program_) {
            glDeleteProgram(program_);
            program_ = 0;
        }
    }

    void Set(const std::string& name, glm::mat4& mat) const
    {
        int location = glGetUniformLocation(program_, name.c_str());
        if (location == -1)
            aout << "[ERROR]SetMatrix(\"" << name << "\") Failed" << std::endl;
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Set(const std::string& name, bool value) const
    {
        int location = glGetUniformLocation(program_, name.c_str());
        if (location == -1)
            aout << "[ERROR]Set bool(\"" << name << "\") Failed" << std::endl;
        glUniform1i(location, (int)value);
    }
    /*!
     * Prepares the shader for use, call this before executing any draw commands
     */
    void activate() const;

    /*!
     * Cleans up the shader after use, call this after executing any draw commands
     */
    void deactivate() const;

//    /*!
//     * Renders a single model
//     * @param model a model to render
//     */
//    void drawModel(const Model &model) const;

//    /*!
//     * Sets the model/view/projection matrix in the shader.
//     * @param projectionMatrix sixteen floats, column major, defining an OpenGL projection matrix.
//     */
//    void setProjectionMatrix(float *projectionMatrix) const;
//
//    void setViewMatrix(float* InViewMatrix) const;
private:
    /*!
     * Helper function to load a shader of a given type
     * @param shaderType The OpenGL shader type. Should either be GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     * @param shaderSource The full source of the shader
     * @return the id of the shader, as returned by glCreateShader, or 0 in the case of an error
     */
    static GLuint loadShader(GLenum shaderType, const std::string &shaderSource);

    /*!
     * Constructs a new instance of a shader. Use @a loadShader
     * @param program the GL program id of the shader
     * @param position the attribute location of the position
     * @param uv the attribute location of the uv coordinates
     * @param projectionMatrix the uniform location of the projection matrix
     */
    constexpr Shader(GLuint program)
            : program_(program) {}

    GLuint program_;

};

#endif //ANDROIDGLINVESTIGATIONS_SHADER_H