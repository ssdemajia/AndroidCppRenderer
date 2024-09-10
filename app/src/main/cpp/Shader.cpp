#include "Shader.h"

#include "AndroidOut.h"
#include "Model.h"
#include "Utility.h"

Shader* Shader::loadShader(AAssetManager *assetManager, const std::string &vertexPath, const std::string &fragmentPath)
{
    auto vertexAsset = AAssetManager_open(
            assetManager,
            vertexPath.c_str(),
            AASSET_MODE_BUFFER);
    const void *pVertexBuffer = AAsset_getBuffer(vertexAsset);
    size_t vertexLength = AAsset_getLength(vertexAsset);
    std::string vertexSource((char*)pVertexBuffer, vertexLength);

    auto fragmentAsset = AAssetManager_open(
            assetManager,
            fragmentPath.c_str(),
            AASSET_MODE_BUFFER);
    const void *pFragmentBuffer = AAsset_getBuffer(fragmentAsset);
    size_t FragmentLength = AAsset_getLength(fragmentAsset);
    std::string fragmentSource((char*)pFragmentBuffer, FragmentLength);
    return loadShader(vertexSource, fragmentSource);
}

Shader* Shader::loadShader(AAssetManager *assetManager, const std::string &computePath)
{
    auto asset = AAssetManager_open(
            assetManager,
            computePath.c_str(),
            AASSET_MODE_BUFFER);
    const void *pBuffer = AAsset_getBuffer(asset);
    size_t Length = AAsset_getLength(asset);
    std::string Source((char*)pBuffer, Length);

    return loadShader(Source);
}

Shader* Shader::loadShader(const std::string& computeSource)
{
    Shader *shader = nullptr;

    GLuint computeShader = loadShader(GL_COMPUTE_SHADER, computeSource);
    if (!computeShader) {
        return nullptr;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, computeShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            // If we fail to link the shader program, log the result for debugging
            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                aout << "Failed to link program with:\n" << log << std::endl;
                delete[] log;
            }

            glDeleteProgram(program);
        }
        else
        {
            shader = new Shader(program);
        }
    }
    glDeleteShader(computeShader);

    return shader;
}
Shader *Shader::loadShader(
        const std::string &vertexSource,
        const std::string &fragmentSource) {
    Shader *shader = nullptr;

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return nullptr;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return nullptr;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            // If we fail to link the shader program, log the result for debugging
            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                aout << "Failed to link program with:\n" << log << std::endl;
                delete[] log;
            }

            glDeleteProgram(program);
        } else {
            // Get the attribute and uniform locations by name. You may also choose to hardcode
            // indices with layout= in your shader, but it is not done in this sample
//            GLint positionAttribute = glGetAttribLocation(program, positionAttributeName.c_str());
//            GLint uvAttribute = glGetAttribLocation(program, uvAttributeName.c_str());
//            GLint projectionMatrixUniform = glGetUniformLocation(
//                    program,
//                    projectionMatrixUniformName.c_str());
//            GLint viewMatrixUniform = glGetUniformLocation(
//                    program,
//                    viewMatrixUniformName.c_str());
            // Only create a new shader if all the attributes are found.
            shader = new Shader(program);
//            if (positionAttribute != -1
//                && uvAttribute != -1
//                && projectionMatrixUniform != -1) {
//
//
//            } else {
//                glDeleteProgram(program);
//            }
        }
    }

    // The shaders are no longer needed once the program is linked. Release their memory.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

GLuint Shader::loadShader(GLenum shaderType, const std::string &shaderSource) {
    Utility::assertGlError();
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        auto *shaderRawString = (GLchar *) shaderSource.c_str();
        GLint shaderLength = shaderSource.length();
        glShaderSource(shader, 1, &shaderRawString, &shaderLength);
        glCompileShader(shader);

        GLint shaderCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

        // If the shader doesn't compile, log the result to the terminal for debugging
        if (!shaderCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

            if (infoLength) {
                auto *infoLog = new GLchar[infoLength];
                glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
                aout << "Failed to compile with:\n" << infoLog << std::endl;
                delete[] infoLog;
            }

            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

void Shader::activate() const {
    glUseProgram(program_);
}

void Shader::deactivate() const {
    glUseProgram(0);
}
