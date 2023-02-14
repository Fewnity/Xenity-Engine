#include "ui_manager.h"
#include "../main.h"
#include <ft2build.h>
#include <iostream>
#include <glad/glad.h>
#include "../engine_settings.h"
#include "window.h"
#include "../debug.h"
#include "../graphics/graphics.h"

#include FT_FREETYPE_H

struct Character 
{
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

unsigned int UiManager::textVAO, UiManager::textVBO;
std::vector<Font*> UiManager::fonts;

/// <summary>
/// Create vertex buffer for texts
/// </summary>
void UiManager::CreateTextBuffer()
{
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UiManager::DeleteFont(Font * font)
{
	int fontCount = fonts.size();
	for (int i = 0; i < fontCount; i++)
	{
		if (fonts[i] == font) {
			DeleteFont(i);
			break;
		}
	}
}

void UiManager::DeleteFont(int index)
{
	delete fonts[index];
	fonts.erase(fonts.begin() + index);
}

/// <summary>
/// Draw text
/// </summary>
/// <param name="s">Shader</param>
/// <param name="text">Text string</param>
/// <param name="x">X position</param>
/// <param name="y">Y position</param>
/// <param name="scale">Text's scale</param>
/// <param name="color">Text's color</param>
void UiManager::RenderText(Shader& s, std::string text, float x, float y, float angle, float scale, glm::vec3 color, Font * font)
{
	//y = Window::GetHeight() - y;

	float aspect = static_cast<float>((Window::GetWidth()) / static_cast<float>(Window::GetHeight()));

	x -= Graphics::usedCamera->gameObject->transform.GetPosition().x;
	y += Graphics::usedCamera->gameObject->transform.GetPosition().y;

	x = x / aspect * 1.7777f;
	y = y / aspect * 1.7777f;

	x += Window::GetWidth() / 100.0f / 2.0f;
	y += Window::GetHeight() / 100.0f / 2.0f;

	y *= 100;
	x *= 100;
	y = Window::GetHeight() - y;

	// activate corresponding render state	
	s.Use();
	s.SetShaderCameraPosition2D();
	s.SetShaderProjection2D();
	s.SetShaderModel(Vector3(x, y, 0), Vector3(0, 0, angle), Vector3(scale / aspect * 1.7777f, scale / aspect * 1.7777f, 1));

	s.SetShaderAttribut("textColor", Vector3(color.x, color.y, color.z));
	//glUniform3f(glGetUniformLocation(s.GetProgramId(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// iterate through all characters
	std::string::const_iterator c;
	int x2 = 0;

	float len = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font->Characters[*c];

		len += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font->Characters[*c];

		float xpos = x2 + ch.Bearing.x * scale - len /2.0f;
		float ypos = 0 - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x2 += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void UiManager::RenderTextCanvas(Shader& s, std::string text, float x, float y, float angle, float scale, glm::vec3 color, Font* font)
{
	y = Window::GetHeight() - y;

	// activate corresponding render state	
	s.Use();
	s.SetShaderProjection2D();
	s.SetShaderRotation(Vector3(0, 0, 0));

	s.SetShaderAttribut("textColor", Vector3(color.x, color.y, color.z));
	//glUniform3f(glGetUniformLocation(s.GetProgramId(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font->Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Load a font
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
Font* UiManager::CreateFont(std::string filePath) 
{
	Debug::Print("Loading font...");

	Font* font = new Font();

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		Debug::Print("ERROR: Could not init FreeType Library");
		return nullptr;
	}

	//Load font
	FT_Face face;
	std::string path = EngineSettings::RootFolder + filePath;
	if (FT_New_Face(ft, path.c_str(), 0, &face))
	{
		Debug::Print("ERROR: Failed to load font. Path: " + path);
		return nullptr;
	}

	//Load glyph
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 255; c++)
	{
		try
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			{
				Debug::Print("ERROR: Failed to load Glyph. Path: " + path);
				continue;
			}

			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			font->Characters.insert(std::pair<char, Character>(c, character));
		}
		catch (...)
		{
			Debug::Print("ERROR: Failed to load Glyph (Try Catch). Path: " + path);
			//std::cout << "ERROR::" << std::endl;
			return nullptr;
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	fonts.push_back(font);
	return font;
}

/// <summary>
/// Init Ui Manager
/// </summary>
/// <returns></returns>
int UiManager::Init()
{
	//Init librairy
	Font* font0 = CreateFont(R"(Xenity_Engine\Source\fonts\Roboto-Regular.ttf)");
	Font* font1 = CreateFont(R"(Xenity_Engine\Source\fonts\alagard.ttf)");

	CreateTextBuffer();

	Debug::Print("---- UI system initiated ----");

	return 0;
}

Font::~Font()
{
	for (const auto& kv : Characters) 
	{
		glDeleteTextures(1, &kv.second.TextureID);
	}
	Characters.clear();
}
