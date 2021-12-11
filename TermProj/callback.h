#pragma once
#include "game.h"

GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid UpKeyboard(unsigned char key, int x, int y);
GLvoid UpSpecial(int key, int x, int y);
GLvoid Special(int key, int x, int y);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid InitTexture();
GLvoid InitBuffer();
GLuint VAO[4];
GLuint VBO_position[4];
GLuint VBO_normal[4];
GLuint VBO_texture[4];

const int num_vertices = 3;
const int num_triangles = 1;

//--- load obj related variabales
int loadObj(const char* filename);
int loadObj_normalize_center(const char* filename);
float* sphere_object;
int num_Triangle;


void keyOperations(void) { //키 상호작용 함수
	if (keyStates[0]) tank.range += 0.1f;
	if (keyStates[1]) tank.headR += 5.0f;
	if (keyStates[2]) tank.range -= 0.1f;
	if (keyStates[3]) tank.headR -= 5.0f;
	if (keyStates[4]) tank.tankR += 1.0f; //왼
	if (keyStates[5]) tank.move(1); //앞
	if (keyStates[6]) tank.tankR -= 1.0f; //오
	if (keyStates[7]) tank.move(-1); //뒤
}

GLvoid drawScene(GLvoid)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(s_program[0]);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "modelTransform");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "viewTransform");
	unsigned int	projLoc = glGetUniformLocation(s_program[0], "projection");
	unsigned int viewPos = glGetUniformLocation(s_program[0], "viewPos");
	unsigned int objColorLocation = glGetUniformLocation(s_program[0], "objectColor");

	//뷰 변환
	glm::vec3 cameraPos = glm::vec3(tank.x, 5.0f, tank.z + 5.0f); //--- 카메라 위치
	glm::vec3 cameraTarget = glm::vec3(tank.x, 0.5f, tank.z);
	glm::vec3 cameraDirection = glm::normalize(-cameraPos + cameraTarget); //--- 카메라 바라보는 방향
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::vec3 cameraRight = glm::cross(Up, cameraDirection);
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(viewPos, 1, glm::value_ptr(cameraPos));
	//투영 변환
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 50.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);

	//조명
	unsigned int lightPosLocation = glGetUniformLocation(s_program[0], "lightPos");
	unsigned int lightColorLocation = glGetUniformLocation(s_program[0], "lightColor");
	glUniform3f(lightPosLocation, 0, 5.0f, 0);
	glUniform3f(lightColorLocation, 1.0f,1.0f,1.0f);

	//평면 출력
	glm::mat4 PLATE = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(PLATE));
	glUniform3f(objColorLocation, 0.5, 0.5, 0.5);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	draw(modelLocation, objColorLocation, VAO);

	//더블 버퍼링
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w': keyStates[0] = true; break;
	case 'a': keyStates[1] = true; break;
	case 's': keyStates[2] = true; break;
	case 'd': keyStates[3] = true; break;
	case ' ': tank.attack();
	}
}

GLvoid UpKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': keyStates[0] = false; break;
	case 'a': keyStates[1] = false; break;
	case 's': keyStates[2] = false; break;
	case 'd': keyStates[3] = false; break;
	}
}

GLvoid Special(int key, int x, int y)
{
	switch (key) {
	case 100: keyStates[4] = true; break;
	case 101: {keyStates[7] = false; tank.moving = 1; keyStates[5] = true; break; }
	case 102: keyStates[6] = true; break;
	case 103: {keyStates[5] = false; tank.moving = 1; keyStates[7] = true; break; }
	}
}

GLvoid UpSpecial(int key, int x, int y)
{
	switch (key) {
	case 100: keyStates[4] = false; break;
	case 101: tank.moving = 0; break;
	case 102: keyStates[6] = false; break;
	case 103: tank.moving = 0; break;
	}
}

GLvoid Timer(int value)
{
	keyOperations();
	update();
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
}

GLvoid InitBuffer()
{
	
	tank.obj = loadObj("tank_body.obj");
	rifle_obj = loadObj("rifle.obj");
	glGenVertexArrays(4, VAO);
	glGenBuffers(4, VBO_position);
	glGenBuffers(4, VBO_normal);
	glGenBuffers(4, VBO_texture);
	glUseProgram(s_program[0]);

	//평면
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plate), plate, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//장애물
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//탱크(임의)
	glGenBuffers(4, tank.VBO_pos);
	glGenBuffers(4, tank.VBO_nor);
	glGenBuffers(4, tank.VBO_tex);
	glGenVertexArrays(2, tank.VAO);
	glBindVertexArray(tank.VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, tank.VBO_pos[0]);
	glBufferData(GL_ARRAY_BUFFER, outvertex.size() * sizeof(glm::vec3), &outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, tank.VBO_nor[0]);
	glBufferData(GL_ARRAY_BUFFER, outnormal.size() * sizeof(glm::vec3), &outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, tank.VBO_tex[0]);
	glBufferData(GL_ARRAY_BUFFER, outuv.size() * sizeof(glm::vec2), &outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	//포물선 점
	glBindVertexArray(tank.VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, tank.VBO_pos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(track), track, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, tank.VBO_nor[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(track), track, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//라이플맨
	glGenBuffers(3, rifle_VBO);
	glGenVertexArrays(1, &rifle_VAO);
	glBindVertexArray(rifle_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, rifle_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, outvertex.size() * sizeof(glm::vec3), &outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, rifle_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, outnormal.size() * sizeof(glm::vec3), &outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, rifle_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, outuv.size() * sizeof(glm::vec2), &outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);

}

GLvoid InitTexture() { //여기 평면이랑 탱크 순서 바꿔뒀어열
	int widthImage, heightImage, numberOfChannel;
	stbi_set_flip_vertically_on_load(true);

	widthImage = 512;
	heightImage = 512;
	numberOfChannel = 0;

	glGenTextures(1, &ground_texture);

	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data0 = stbi_load("temp_ground.jpg", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data0);

	glGenTextures(1, &tank.body_texture);

	glBindTexture(GL_TEXTURE_2D, tank.body_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data1 = stbi_load("tank_body_diffuse.jpg", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);

	
	stbi_image_free(data0);
	stbi_image_free(data1);
}