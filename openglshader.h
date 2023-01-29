// generates the shader on the fly
// ------------------------------------------------------------------------
#pragma once
#ifndef SHADER_H
#define SHADER_H
#include<fstream>
#include<sstream>
#include<iostream>
//using namespace std;  //不推荐在.h头文件中 使用using，容易导致命名冲突

class Shader
{
public:
	Shader(std::string vertexPath = "D:\\vertexShader.txt", std::string  fragmentPath = "D:\\fragmentShader.txt", const char* geometryPath = nullptr) //这里设置了默认参数值，如果在构造的时候不填入参数 则是默认的参数
	{
		std::string vertexCode;	// 1. retrieve the vertex/fragment source code from filePath
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile(vertexPath);
		std::ifstream fShaderFile(fragmentPath);
		std::ifstream gShaderFile;	
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);	// ensure ifstream objects can throw exceptions:
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			//vShaderFile.open(vertexPath.c_str());//open files：位置值在这个文件里面被设置了 所以如果要改 就要去改文件
			//fShaderFile.open(fragmentPath.c_str());
			std::stringstream vShaderStream, fShaderStream; //创建字符串流对象
			vShaderStream << vShaderFile.rdbuf();//把文件中的字符串放到流中：read file's buffer contents into streams
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close(); // close file handlers
			fShaderFile.close();
		
			vertexCode = vShaderStream.str();	  // convert stream into string
			fragmentCode = fShaderStream.str();		
			if (geometryPath != nullptr)	// if geometry shader path is present, also load a geometry shader
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		
		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);	 // vertex shader
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);  // 2. compile shaders	
		checkCompileErrors(vertex, "VERTEX");
		
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER); // fragment Shader
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
	
		unsigned int geometry;
		if (geometryPath != nullptr) 	// if geometry shader is given, compile geometry shader
		{
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
	
		ID = glCreateProgram(); 	// shader Program
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
	
		glDeleteShader(vertex); 	// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);

	}

	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const
	{
		setMat4("projection", projection);
		setMat4("view", view);
		setMat4("model", model);
	}

	void use() const 	// activate the shader
	{
		glUseProgram(ID);
	}
	
	void setBool(const std::string& name, bool value) const // utility uniform functions
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	void checkCompileErrors(GLuint shader, std::string type)  // utility function for checking shader compilation/linking errors.
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);  //用于检测编译是否成功
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);  //获取信息日志 日志存在infoLog中
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);  //用于检测程序是否链接成功
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);  //这里一样
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	unsigned int ID;
};
#endif