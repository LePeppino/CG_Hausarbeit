#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include <stdio.h>
#include "Application.h"
#include "freeimage.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "assets/"
#endif

void PrintOpenGLVersion();


int main () {
    FreeImage_Initialise();
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	const char* glsl_version = "#version 400";
    //window Size
    const int WindowWidth = 1280;
    const int WindowHeight = 720;
    
    //Create window
    GLFWwindow* window = glfwCreateWindow (WindowWidth, WindowHeight, "Computergrafik - Hausarbeit", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);

	//IMGUI
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGuiIO& io = ImGui::GetIO();
	ImFont* fontSmall = io.Fonts->AddFontFromFileTTF(ASSET_DIRECTORY "fonts/DroidSans.ttf", 16);
	ImFont* fontMedium = io.Fonts->AddFontFromFileTTF(ASSET_DIRECTORY "fonts/DroidSans.ttf", 20);
	ImFont* fontLarge = io.Fonts->AddFontFromFileTTF(ASSET_DIRECTORY "fonts/DroidSans.ttf", 24);
	// Setup style
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 0.8f;
	style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);
	

    //using OpenGL Extension Weangler Library on windows 
#if WIN32
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    PrintOpenGLVersion();

    {
        double lastTime=0;
        Application App(window);
        App.start();

        while (!glfwWindowShouldClose (window)) {
            double now = glfwGetTime();
            double delta = now - lastTime;
            lastTime = now;

            // once per frame
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			glfwPollEvents();
            App.update((float)delta);
            App.draw();

			//UI Overlay
			{
				ImGui::PushFont(fontMedium);
				ImGui::Begin("Fly through the rings but don't crash!", NULL);
				ImGui::PushFont(fontLarge);
				ImGui::Text("SCORE: %d / 50", App.score);
				ImGui::Text("CRASHES: %d", App.crashes);
				ImGui::PushFont(fontSmall);
				ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers (window);
        }
        App.end();
    }

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}


void PrintOpenGLVersion()
{
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
}
