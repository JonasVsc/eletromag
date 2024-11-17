#include <glad/gl.h>
#include <GLFW/glfw3.h>


int main() {

	GLFWwindow* window;
	
	glfwInit();

	window = glfwCreateWindow(1280, 720, "OpenGL Application", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}