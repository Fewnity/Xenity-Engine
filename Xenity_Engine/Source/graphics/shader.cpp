#include "shader.h"
#include "graphics.h"
#include "../ui/window.h"
#include <iostream>
#include "../file_system/file.h"

GLuint vertexShaderId, fragmentShaderId, programId;

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	LoadShader(vertexShaderPath, fragmentShaderPath);
}

GLuint Shader::GetProgramId() {
	return this->programId;
}


void Shader::LoadShader(std::string vertexPath, std::string fragmentPath) {
	//Load vertex shader text
	std::string vertex_shader_text_temp = File::LoadShaderData(vertexPath);
	const char* vertex_shader_text = vertex_shader_text_temp.c_str();
	//Create vertex shader
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertex_shader_text, NULL); // Load shader text
	glCompileShader(vertexShaderId);

	//Load fragment shader text
	std::string fragment_shader_text_temp = File::LoadShaderData(fragmentPath);
	const char* fragment_shader_text = fragment_shader_text_temp.c_str();
	//Create fragment shader
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragment_shader_text, NULL);// Load shader text
	glCompileShader(fragmentShaderId);

	//Link shaders to a program
	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
}

void Shader::SetShaderCameraPosition() {
	Use();
	//Camera position
	if (Graphics::usedCamera != nullptr) 
	{
	Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
	vect.x += Graphics::usedCamera->gameObject->transform.position.x;
	vect.y += Graphics::usedCamera->gameObject->transform.position.y;
	vect.z += Graphics::usedCamera->gameObject->transform.position.z;

	glm::mat4 camera = glm::lookAt(glm::vec3(Graphics::usedCamera->gameObject->transform.position.x, Graphics::usedCamera->gameObject->transform.position.y, Graphics::usedCamera->gameObject->transform.position.z), glm::vec3(vect.x, vect.y, vect.z), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(programId, "camera"), 1, false, glm::value_ptr(camera));
	}
}

void Shader::SetShaderProjection() {
	Use();
	if (Graphics::usedCamera != nullptr)
	{
		//Projection
		glm::mat4 projection = glm::perspective(glm::radians(Graphics::usedCamera->GetFov()), (double)Window::GetWidth() / (double)Window::GetHeight(), 0.1, 100.0);
		glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, false, glm::value_ptr(projection));
	}
}

void Shader::SetShaderProjection2D() {
	Use();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::GetWidth()), 0.0f, static_cast<float>(Window::GetHeight()));
	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Shader::SetShaderPosition(Vector3 position) {
	Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(position.x, position.y, position.z));
	//trans = glm::rotate(trans, (float)0, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(programId, "offset"), 1, false, glm::value_ptr(trans));
}

void Shader::SetShaderRotation(Vector3 eulerAngle) {
	Use();
	//Rotation
	glm::quat MyQuaternion;
	// Spécification directe des quatre composantes
	// Vous ne faites presque jamais cela directement
	// MyQuaternion = glm::quat(w, x, y, z);
	// Conversion à partir des angles d'Euler (en radians) vers un quaternion
	// radians = (degrees * pi) / 180 ;
	// degrees = radians * (180.0 / 3.141592653589793238463)
	//glm::vec3 EulerAngles(eulerAngle.x, eulerAngle.y, eulerAngle.z);
	glm::vec3 EulerAngles((eulerAngle.x * M_PI) / 180.0, (eulerAngle.y * M_PI) / 180.0, (eulerAngle.z * M_PI) / 180.0);
	MyQuaternion = glm::quat(EulerAngles);
	// Conversion à partir d'un axe/angle
	// Dans GLM l'angle doit être en degrés. Ici, donc, convertissez-le.
	// MyQuaternion = gtx::quaternion::angleAxis(glm::degrees(RotationAngle), RotationAxis);

	GLuint location = glGetUniformLocation(programId, "rotation");
	glm::mat4 rotation = glm::toMat4(MyQuaternion);
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(rotation));
}

void Shader::SetShaderScale(Vector3 scale) {
	Use();
	glm::mat3 scaleMat = glm::mat3(glm::vec3(scale.x, 0.0, 0.0),
	                               glm::vec3(0.0, scale.y, 0.0),
	                               glm::vec3(0.0, 0.0, scale.z));

	glUniformMatrix3fv(glGetUniformLocation(programId, "scale"), 1, false, glm::value_ptr(scaleMat));
}

void Shader::SetShaderAttribut(std::string attribut, Vector4 value) {
	Use();
	glUniform4f(glGetUniformLocation(programId, attribut.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::SetShaderAttribut(std::string attribut, Vector3 value) {
	Use();
	glUniform3f(glGetUniformLocation(programId, attribut.c_str()), value.x, value.y, value.z);
}

void Shader::SetShaderAttribut(std::string attribut, float value) {
	Use();
	glUniform1f(glGetUniformLocation(programId, attribut.c_str()), value);
}

void Shader::SetShaderTexture(std::string attribut, Texture texture) {
	Use();
	glUniform1i(glGetUniformLocation(programId, attribut.c_str()), texture.GetTextureIndex());
}

void Shader::Use() {
	glUseProgram(programId);
}